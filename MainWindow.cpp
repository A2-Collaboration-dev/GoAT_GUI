#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <TROOT.h>
#include <TSystemFile.h>
#include <TSystemDirectory.h>

#include <chrono>
#include <thread>

#include <QFileInfo>
#include "TKey.h"
#include "TFile.h"
#include "TSystem.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TDirectoryFile.h"
#include "TH1.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    TransientState = false;


    /*
     *  Check if GoAT Config file exists
     */
    QFile Fout((QCoreApplication::applicationDirPath().toStdString() + std::string("/config/GoAT-config.dat")).c_str());

    if(!Fout.exists())
    {
        configurationDialog = new ConfigurationDialog(this);
        configurationDialog->setModal(true);
        configurationDialog->exec();
    }

    tabRunByRun = new TabRunByRun(parent);
    tabComplete = new TabComplete(parent);
    tabLog = new TabLog(parent);

    /*
     * Must initialize to connect buttons.
     */
    queueEditDialog = new QueueEditDialog(this);

    /*
     * Constructing control tab widget
     */
    ui->tabWidget->addTab(tabRunByRun, QString("Run By Run"));
    ui->tabWidget->addTab(tabComplete, QString("Accumulated"));
    ui->tabWidget->addTab(tabLog, QString("Events Log"));

    /*
     * Calling GUI file and checking if it was read
     */

    if (!configGUI.loadGUIConfigFile(QCoreApplication::applicationDirPath().toStdString() + std::string("/config/settings.gui")))
    {
        std::cout << "GUI Config file not found at: " <<
                     QCoreApplication::applicationDirPath().toStdString() + std::string("/config/settings.gui") << std::endl;
        return;
        this->close();
    }
    configGUI.setConfigFilePath(QCoreApplication::applicationDirPath().toStdString() + std::string("/config/settings.gui"));
    configGUI.PrintAll();
    std::cout << "Application path:" << QCoreApplication::applicationDirPath().toStdString() << std::endl;

    QFileInfo GoAT(configGUI.getGoATExe().c_str());
    if (!GoAT.exists())
    {
        std::cout << "GoAT executable does not exists at:" << configGUI.getGoATExe() << std::endl;
        tabLog->AppendTextNL(TabLog::ColorB("Hey Patrik, oh by the way GoAT executable was not found.", "Crimson"));
    }

    QFileInfo PhysicsExe(configGUI.getPhysExe().c_str());
    if (!PhysicsExe.exists())
    {
        std::cout << "Physics executable does not exists at:" << configGUI.getPhysExe() << std::endl;
        tabLog->AppendTextNL(TabLog::ColorB("Hey Patrik, oh by the way Physics executable was not found.", "Crimson"));

    }

    /*
     * Setting labels
     */
    tabLog->setLabelLastACQU(configGUI.getLastFile());
    tabLog->setLabelLastGoAT(configGUI.getLastGoATFile());
    tabLog->setLabelLastPhys(configGUI.getLastPhysFile());


    /*
     * Using QFileSystemWatcher to check if ACQU directory was updated
     */
    this->curFile = this->configGUI.getLastFile();
    this->continueScanning = true;
    this->OpeningAtempt = 0;
    this->watcher = new QFileSystemWatcher(this);
    this->watcher->addPath(this->configGUI.getACQUDir().c_str());
    connect(watcher, SIGNAL(directoryChanged(const QString &)), this, SLOT(ACQUdirChanged(const QString &)));

    /*
     * Signal of when GoAT is finished
     */
    this->GoATProcess = new QProcess(this);
    this->GoATarguments = new QStringList;
    connect(GoATProcess, SIGNAL(finished(int)), this, SLOT(newGoatFile()));

    /*
     * Signal of when Physics Analysis is finished
     */
    this->PhysicsProcess = new QProcess(this);
    this->PhysicsArguments = new QStringList;
    connect(PhysicsProcess, SIGNAL(finished(int)), this, SLOT(newPhysicsFile()));


    /*
     * Log tab buttons
     */
    connect(tabLog->getButtonRunGoAT(), SIGNAL(clicked()), this, SLOT(ForceRunGoAT()));
    connect(tabLog->getButtonKillGoAT(), SIGNAL(clicked()), this, SLOT(killGoatProcess()));
    connect(tabLog->getButtonRunPhys(), SIGNAL(clicked()), this, SLOT(ForceRunPhysics()));
    connect(tabLog->getButtonKillPhys(), SIGNAL(clicked()), this, SLOT(killPhysicsProcess()));
    connect(tabLog->getButtonList(), SIGNAL(clicked()), this, SLOT(ListQueue()));
    connect(tabLog->getButtonStartQueue(), SIGNAL(clicked()), this, SLOT(StartQueue()));

    /*
     * Complete Window Save button
     */
    connect(tabComplete->getButtonSave(), SIGNAL(clicked()), this, SLOT(CompleteButtonSave()));
    connect(tabComplete->getButtonSave2(), SIGNAL(clicked()), this, SLOT(CompleteButtonSave()));

    /*
     * Connecting QueueEditDialog
     */
    connect(tabLog->getButtonEditQueue(), SIGNAL(clicked()), this, SLOT(EditQueueDialog()));
    connect(queueEditDialog->getButtonSave(), SIGNAL(clicked()), this, SLOT(QueueDialogConfirm()));

    this->tabLog->AppendTextNL("GUI Initialized.");

    std::cout << "Accumulated ACQU: " << configGUI.getCompleteACQUFile() << std::endl;
    std::cout << "Accumulated Physics: " << configGUI.getCompletePhysicsFile() << std::endl;

    /*
     * Excluding all current files in ACQU directory (including the newest (optional))
    */
    ExcludeFiles(configGUI.getACQUDir(), "*.root", false);
}



MainWindow::~MainWindow()
{
    continueScanning = false;
    GoATProcess->kill();
    GoATProcess->close();
    delete GoATProcess;
    delete ui;
    std::cout << "~MainWindow()" << std::endl;
}

void MainWindow::setAbsolutePath(std::string path)
{
    this->absolutePath = path;
}

void MainWindow::FileChecker()
{

}

void MainWindow::ACQUdirChanged(QString path)
{
    std::string newFile = this->getNewestFile(configGUI.getACQUDir(), "*.root");

    if (newFile == "") goto StartQueue;

    if(!(std::find(ACQUFilesQueue.begin(), ACQUFilesQueue.end(), newFile) != ACQUFilesQueue.end())) {
        ACQUFilesQueue.push_back(newFile);
        tabLog->AppendTextNL("New file detected: " + TabLog::Color(newFile, "DarkOliveGreen"));
        if (ACQUFilesQueue.size() == 1)
            tabLog->AppendTextNL(TabLog::ColorB("GoAT", "BlueViolet") + " is already running or GUI is busy. File added to queue: " + TabLog::Color(newFile, "DarkOliveGreen")  + " [" + std::to_string(ACQUFilesQueue.size()) + "]");
        std::cout << "file in queue" + newFile << std::endl;
    }

    StartQueue:
    if ((this->GoATProcess->state() == QProcess::NotRunning) && (this->PhysicsProcess->state() == QProcess::NotRunning) && (TransientState == false) && !ACQUFilesQueue.empty())
    {
        this->curFile = this->ACQUFilesQueue[0];
        this->ACQUFilesQueue.erase(ACQUFilesQueue.begin());
        tabLog->setLabelLastACQU(this->curFile);
        configGUI.setLastACQUFile(this->curFile);
        std::cout << "Starting GoAT" << std::endl;
        RunGoat();
    }
}


void MainWindow::RunGoat(bool Accumulate)
{
    if (this->GoATProcess->state() != QProcess::NotRunning)
        return;

    if (this->PhysicsProcess->state() != QProcess::NotRunning)
        return;

    QFileInfo QcheckFile(this->curFile.c_str());
    if (!QcheckFile.exists())
    {
        tabLog->AppendTextNL("File not found: " + TabLog::Color(this->curFile, "DarkOliveGreen"));
        this->OpeningAtempt = 0;
        TransientState = false;
        return;
    }

    int MaxContinueAttempts = 5;
    int AttemptEveryThisSeconds = 15;
    int FileEditedThisSecondsAgo = 5;


    // are we attempting to check file?
    if (this->OpeningAtempt >= MaxContinueAttempts)
    {
        tabLog->AppendText1L("File Error: ", "DarkMagenta", "Could not open " + this->curFile + ". File added to the end of the queue.");
        this->OpeningAtempt = 0;
        TransientState = false;
        ACQUFilesQueue.push_back(this->curFile);
        ACQUdirChanged("check_for_new_file && FINISH");
        return;
    }

    if (this->curFile.empty())
        return;

    TransientState = true;
    *GoATarguments << "-f" << this->curFile.c_str()
                   << "-D" << configGUI.getGoATDir().c_str()
                   << "-p" << configGUI.getACQUPrefix().c_str()
                   << "-P" << configGUI.getGoATPrefix().c_str()
                   << (QCoreApplication::applicationDirPath().toStdString() + std::string("/config/GoAT-config.dat")).c_str();

    /*
     * Arguments for GoAT analysis
     */
    std::cout << "GoAT call Arguments: " << std::endl
              << "-f" << " " << this->curFile.c_str() << " "
              << "-D" << " " << configGUI.getGoATDir().c_str() << " "
              << "-p" << " " << configGUI.getACQUPrefix().c_str() << " "
              << "-P" << " " << configGUI.getGoATPrefix().c_str() << " "
              << (QCoreApplication::applicationDirPath().toStdString() + std::string("/config/GoAT-config.dat")).c_str() << std::endl;


    /*
     * Checking if file is in use
     */

    QFileInfo qfile(this->curFile.c_str());
    //this->FinishedACQUFiles.push_back(this->curFile); // Experimental

    /*
     * Check if files was
     *  modified in x seconds, else use it
     */
    if (qfile.lastModified() > QDateTime::currentDateTime().addSecs(-FileEditedThisSecondsAgo))
    {
        tabLog->AppendTextNL("Could not open " + TabLog::Color(this->curFile, "DarkOliveGreen") + ", trying again in 5 seconds. (" + std::to_string(MaxContinueAttempts - this->OpeningAtempt) + ")");
        std::cout << "Could not open file, maybe being written. " << this->OpeningAtempt << std::endl;
        this->OpeningAtempt++;
        //ACQUdirChanged("check_for_new_file"); // EXP
        TakeANap(AttemptEveryThisSeconds * 1000);
        //this->FinishedACQUFiles.push_back(this->curFile);
        this->RunGoat();
        return;
    }


    /* File seems to be okay, taking a nap and starting GoAT */
    this->OpeningAtempt = 0;

    tabRunByRun->updateRootFile(this->curFile.c_str());
    tabRunByRun->UpdateGraphicsDetectors();

    /*
     * Copies histograms from new ACQU file to Master complete file
     * Very important to put this in correct place, or data dublicates may arise
     * Checking if ACQU files was used for calculations before.
    */
    //if(!(std::find(FinishedACQUFiles.begin(), FinishedACQUFiles.end(), this->curFile) != FinishedACQUFiles.end()))
    {
        TakeANap(100);
        if(Accumulate)
        {
            UpdateCompleteACQU(this->curFile.c_str());
            tabComplete->ReloadRootFile();
            tabComplete->UpdateGraphicsDetectors();
        }
    }

    TakeANap(5000);
    tabLog->AppendTextNL("Starting " + TabLog::ColorB("GoAT", "BlueViolet") + " with " + TabLog::Color(this->curFile, "DarkOliveGreen"));

    if (this->GoATProcess->state() == QProcess::NotRunning && this->PhysicsProcess->state() == QProcess::NotRunning)
    {
        std::cout << "starting GoAT" << std::endl;
        //this->FinishedACQUFiles.push_back(this->curFile);
        GoATProcess->start(configGUI.getGoATExe().c_str(), *GoATarguments);
        TransientState = false;
    }



}

void MainWindow::TakeANap(int ms)
{
    QTime dieTime = QTime::currentTime().addMSecs( ms );
    while( QTime::currentTime() < dieTime )
    {
        waitCondition.wait(&mutex, 100);
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}


void MainWindow::ForceRunGoAT()
{
    if (GoATProcess->state() != QProcess::NotRunning)
    {
        tabLog->AppendText1L("<b>Error:</b> ", "DarkMagenta", TabLog::ColorB("GoAT", "BlueViolet") + " process is already running.");
        return;
    }

    if (PhysicsProcess->state() != QProcess::NotRunning)
    {
        tabLog->AppendText1L("<b>Error:</b> ", "DarkMagenta", TabLog::ColorB("Physics Analysis", "RoyalBlue ") + " process is already running.");
        return;
    }

    this->curFile = tabLog->getLabelACQU();
    this->RunGoat(false);
}

void MainWindow::ForceRunPhysics()
{
    if (GoATProcess->state() != QProcess::NotRunning)
    {
        tabLog->AppendText1L("<b>Error:</b> ", "DarkMagenta", TabLog::ColorB("GoAT", "BlueViolet") + " process is already running.");
        return;
    }

    if (PhysicsProcess->state() != QProcess::NotRunning)
    {
        tabLog->AppendText1L("<b>Error:</b> ", "DarkMagenta", TabLog::ColorB("Physics Analysis", "RoyalBlue ") + " process is already running.");
        return;
    }

    /* newGoatFile() will transform curFile into an appropriate directory for corresponding GoAT file */
    this->curFile = this->configGUI.getLastFile();
    this->newGoatFile();
}

void MainWindow::newGoatFile()
{
    TransientState = false;
    QString p_stdout = GoATProcess->readAllStandardOutput();
    QString p_stderr = GoATProcess->readAllStandardError();
    QString pa = GoATProcess->readAll();


    //this->continueScanning = true;
    this->OpeningAtempt = 0;

    if (!p_stdout.isEmpty())
    {
        tabLog->AppendTextNL("GoAT Output Stream", p_stdout.toStdString());
        tabLog->AppendTextNL("GoAT Error Stream", p_stderr.toStdString());
    }

    // Only storing changes of files that were used in computation
    configGUI.writeGUIConfigFile();

    /* Obtaining absolute GoAT file path */
    QFileInfo fileInfo(this->curFile.c_str());

    QString GoATFileName = fileInfo.fileName().replace(configGUI.getACQUPrefix().c_str(), configGUI.getGoATPrefix().c_str());
    QString GoATFilePath = QString(configGUI.getGoATDir().c_str()) + GoATFileName;

    QFile GoATFile(GoATFilePath);

    if (GoATFile.exists())
    {
        //tabLog->AppendTextNL("New " + TabLog::ColorB("GoAT", "BlueViolet") + " file: " + TabLog::Color(GoATFilePath.toStdString(), "DarkOliveGreen"));
        tabLog->setLabelLastGoAT(GoATFilePath.toStdString());
        configGUI.setLastGoATFile(GoATFilePath.toStdString());

        *PhysicsArguments << "-f" << GoATFilePath.toStdString().c_str()
                          << "-D" << configGUI.getPhysicsDir().c_str()
                          << "-p" << configGUI.getGoATPrefix().c_str()
                          << "-P" << configGUI.getPhysPrefix().c_str()
                          << (QCoreApplication::applicationDirPath().toStdString() + std::string("/config/GoAT-config.dat")).c_str();

        //TakeANap(1000);

        /*
         * Arguments for Physics analysis
         */
        std::cout << "Physics Arguments: " << std::endl
                  << "-f" << " " << GoATFilePath.toStdString().c_str() << " "
                  << "-D" << " " << configGUI.getPhysicsDir().c_str() << " "
                  << "-p" << " " << configGUI.getGoATPrefix().c_str() << " "
                  << "-P" << " " << configGUI.getPhysPrefix().c_str() << " "
                  << (QCoreApplication::applicationDirPath().toStdString() + std::string("/config/GoAT-config.dat")).c_str() << std::endl;



        tabLog->AppendTextNL("Starting " + TabLog::ColorB("Physics Analysis", "RoyalBlue ") + " with: " + TabLog::Color(GoATFilePath.toStdString(), "DarkOliveGreen"));
        std::cout << "Starting Physics analysis" << std::endl;

        if (this->PhysicsProcess->state() == QProcess::NotRunning)
             PhysicsProcess->start(configGUI.getPhysExe().c_str(), *PhysicsArguments);

    } else {
        tabLog->AppendText1L("File Error: ", "DarkMagenta", "GoAT file " + TabLog::Color(GoATFilePath.toStdString(), "DarkOliveGreen") + " was not found, skipping " + TabLog::ColorB("Physics Analysis", "RoyalBlue "));

        /* No GoAT found, continue work with ACQU */
        if (!this->ACQUFilesQueue.empty())
        {
            this->curFile = this->ACQUFilesQueue[0];
            this->ACQUFilesQueue.erase(ACQUFilesQueue.begin());
            tabLog->setLabelLastACQU(this->curFile);
            configGUI.setLastACQUFile(this->curFile);
            RunGoat();
        }
    }
}

void MainWindow::newPhysicsFile()
{

    QString p_stdout = PhysicsProcess->readAllStandardOutput();
    QString p_stderr = PhysicsProcess->readAllStandardError();
    QString pa = PhysicsProcess->readAll();

    tabLog->AppendTextNL("Physics Analysis Output Stream", p_stdout.toStdString());
    tabLog->AppendTextNL("Physics Analysis Error Stream", p_stderr.toStdString());

    /* Obtaining Physics file path */
    QFileInfo fileInfo(this->curFile.c_str());
    QString PhysicsFileName = fileInfo.fileName().replace(configGUI.getACQUPrefix().c_str(), configGUI.getPhysPrefix().c_str());
    QString PhysicsFilePath = QString(configGUI.getPhysicsDir().c_str()) + PhysicsFileName;

    std::cout << PhysicsFilePath.toStdString() << std::endl;

    /* Update Physics plots from new file */
    tabRunByRun->updateRootPhysicsFile(PhysicsFilePath.toStdString().c_str());
    tabRunByRun->UpdateGraphicsPhysics();

    /* Update Complete experiment Physics file & update plots */
    UpdateCompletePhysics(PhysicsFilePath.toStdString().c_str());
    tabComplete->ReloadRootPhysicsFile();
    tabComplete->UpdateGraphicsPhysics();

    /* Updating with GoAT and Physics file info */
    tabLog->setLabelLastPhys(PhysicsFilePath.toStdString());
    configGUI.setLastPhysFile(PhysicsFilePath.toStdString());
    configGUI.writeGUIConfigFile();

    /* Check the queue for ACQU files */
    if (!this->ACQUFilesQueue.empty())
    {
        this->curFile = this->ACQUFilesQueue[0];
        this->ACQUFilesQueue.erase(ACQUFilesQueue.begin());
        tabLog->setLabelLastACQU(this->curFile);
        configGUI.setLastACQUFile(this->curFile);
        RunGoat();
    }


}

void MainWindow::on_actionEdig_config_file_triggered()
{
    configurationDialog = new ConfigurationDialog(this);
    configurationDialog->updateUIText();
    configurationDialog->setModal(true);
    configurationDialog->exec();
}

void MainWindow::killGoatProcess()
{
    std::cout << "GoATProcess kill" << std::endl;
    this->GoATProcess->kill();
}

void MainWindow::killPhysicsProcess()
{
    std::cout << "PhysicsProcess kill" << std::endl;
    this->PhysicsProcess->kill();
}

void MainWindow::ExitingGoat()
{
    std::cout << "exiting GoAT" << std::endl;
    this->GoATProcess->kill();
    this->PhysicsProcess->kill();
}

void MainWindow::StreamMonitor()
{

}

void MainWindow::on_actionToggle_Plot_lock_triggered()
{
    this->GoATProcess->kill();
    this->PhysicsProcess->kill();
}

void MainWindow::CompleteExperimentDataUpdate(const char *inputFile, const char *outputFile)
{

    TFile *out = new TFile(outputFile, "UPDATE");
    TFile *in = new TFile(inputFile);
    TKey *key;
    TIter nextkey(in->GetListOfKeys());
    TDirectory *currentDir;

    while((key = (TKey*)nextkey()))
    {
        const char *classname = key->GetClassName();
        TClass *cl = gROOT->GetClass(classname);
        TObject *obj = key->ReadObj();

        if (!cl) continue;
        //std::cout << obj->GetName() << std::endl;

        in->cd();
        currentDir = in;
        if (cl->InheritsFrom("TDirectory"))
        {
            //std::cout << "\t(directory above)" << std::endl;

            currentDir->cd(obj->GetName());
            out->mkdir(obj->GetName());

            TKey* dKey;
            TIter dnextkey(gDirectory->GetListOfKeys()); // !@#
            while((dKey = (TKey*)dnextkey()))
            {
                //const char *dclassname = key->GetClassName();
                //TClass *dcl = gROOT->GetClass(dclassname);
                TObject *dobj = dKey->ReadObj();

                // TH2 *InHist = (TH2*)dobj;

                TH2 *InHist = (TH2*)dobj;


                std::string Folder(obj->GetName());
                std::string HistName(dobj->GetName());
                std::string FullPath = Folder + "/" + HistName;

                std::cout << "Looking for: " << FullPath.c_str() << std::endl;

                out->cd();
                TH2 *OutHist = (TH2*)out->Get( FullPath.c_str() );

                if (OutHist != NULL)
                {
                    std::cout << "Histogram found updating." << dobj->GetName() << std::endl;
                    OutHist->Add(InHist);
                    out->cd(obj->GetName());
                    OutHist->Write();
                } else
                {
                    std::cout << "Could not find histogram, adding. " << dobj->GetName() << std::endl;
                    out->cd(obj->GetName());
                    dobj->Write();
                }
            }
        }

        if (cl->InheritsFrom("TTree"))
        {
            // Do not copy trees
        } else {
            // Individual files
            TH2 *InHist = (TH2*)obj;
            TH2 *OutHist = (TH2*)out->Get( obj->GetName() );
            if (OutHist != NULL)
            {
                std::cout << "Doing individual updating " << obj->GetName() <<  std::endl;
                OutHist->Add(InHist);
                out->cd();
                OutHist->Write();
            } else {
                std::cout << "Doing individual updating " <<   obj->GetName() << std::endl;
                out->cd();
                obj->Write();
            }
        }
    }
    in->Close();
    out->Close();
}

void MainWindow::CompleteExperimentDataCreate(const char *inputFile, const char *outputFile)
{
    TFile *out = new TFile(outputFile, "RECREATE");
    TFile *in = new TFile(inputFile);
    TKey *key;
    TIter nextkey(in->GetListOfKeys());
    TDirectory *currentDir;

    while((key = (TKey*)nextkey()))
    {
        const char *classname = key->GetClassName();
        TClass *cl = gROOT->GetClass(classname);
        TObject *obj = key->ReadObj();

        if (!cl) continue;
        //std::cout << obj->GetName() << std::endl;

        in->cd();
        currentDir = in;
        if (cl->InheritsFrom("TDirectory"))
        {
            std::cout << "\t(directory above)" << std::endl;

            currentDir->cd(obj->GetName());
            out->mkdir(obj->GetName());

            TKey* dKey;
            TIter dnextkey(gDirectory->GetListOfKeys()); // !@#
            while((dKey = (TKey*)dnextkey()))
            {
                //const char *dclassname = key->GetClassName();
                //TClass *dcl = gROOT->GetClass(dclassname);
                TObject *dobj = dKey->ReadObj();

                std::cout << "Copying: " << dobj->GetName() <<std::endl;
                out->cd(obj->GetName());
                dobj->Write();

            }
        }

        if (cl->InheritsFrom("TTree"))
        {
            // Do not copy trees
        } else {
            // Individual objects not in folder
            std::cout << "Doing individual creating" << std::endl;
            out->cd();
            obj->Write();
        }
    }
    in->Close();
    out->Close();
}

void MainWindow::UpdateCompleteACQU(const char* inputFile)
{
    tabLog->AppendTextNL(TabLog::ColorB("Accumulated ", "DarkOrange") + TabLog::ColorB("ACQU", "CadetBlue") + "  histograms are being updated: " + TabLog::Color(inputFile, "DarkOliveGreen"));

    TakeANap(100);

    QFile CompleteDataFile(configGUI.getCompleteACQUFile().c_str());
    if (!CompleteDataFile.exists())
    {
        std::cout << "Creating Complete experiment data file." << std::endl;
        CompleteExperimentDataCreate(inputFile, configGUI.getCompleteACQUFile().c_str());
    } else {
        std::cout << "Updating Complete experiment data file." << std::endl;
        CompleteExperimentDataUpdate(inputFile, configGUI.getCompleteACQUFile().c_str());
    }
}

void MainWindow::UpdateCompletePhysics(const char* inputFile)
{
    tabLog->AppendTextNL(TabLog::ColorB("Accumulated ", "DarkOrange") + TabLog::ColorB("Physics", "RoyalBlue") + "  histograms are being updated: " + TabLog::Color(inputFile, "DarkOliveGreen"));
    TakeANap(100);

    QFile CompleteDataFile(configGUI.getCompletePhysicsFile().c_str());
    if (!CompleteDataFile.exists())
    {
        std::cout << "Creating Accumulated physics data file." << std::endl;
        CompleteExperimentDataCreate(inputFile, configGUI.getCompletePhysicsFile().c_str());
    } else {
        std::cout << "Updating Accumulated physics data file." << std::endl;
        CompleteExperimentDataUpdate(inputFile, configGUI.getCompletePhysicsFile().c_str());
    }
}

std::string MainWindow::getNewestFile(std::string iDirIn, char* extension)
{
    std::cout << "getNewestfile called MM" << std::endl;
    QString iDir = QString(iDirIn.c_str());
    QDir dir;
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::Time);
    dir.setCurrent(iDir);

    QStringList filters;
    filters << extension;
    dir.setNameFilters(filters);

    QFileInfoList list = dir.entryInfoList();

    if (list.empty())
        return "";

    foreach(QFileInfo f, list)
    {
        if(!(std::find(FinishedACQUFiles.begin(), FinishedACQUFiles.end(), f.absoluteFilePath().toStdString()) != FinishedACQUFiles.end()))
        {
            FinishedACQUFiles.push_back(f.absoluteFilePath().toStdString());
            std::cout << "returning: " << f.absoluteFilePath().toStdString() << std::endl;
            return f.absoluteFilePath().toStdString();
        }

    }

    return "";
}

void MainWindow::ExcludeFiles(std::string iDirIn, char* extension, bool leaveNewest)
{
    QString iDir = QString(iDirIn.c_str());
    QDir dir;
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::Time);
    dir.setCurrent(iDir);

    QStringList filters;
    filters << extension;
    dir.setNameFilters(filters);

    QFileInfoList list = dir.entryInfoList();

    if (list.empty())
        return;

    foreach(QFileInfo f, list)
    {
        if (leaveNewest)
            if (f == list.first())
                continue;

        FinishedACQUFiles.push_back(f.absoluteFilePath().toStdString());
        std::cout << "Excluded: " << f.absoluteFilePath().toStdString() << std::endl;
    }
}

void MainWindow::ListQueue()
{
    tabLog->AppendTextNL(TabLog::ColorB("Files Queue:", "DarkOliveGreen ") );
    foreach(std::string s, ACQUFilesQueue)
    {
        tabLog->AppendTextNL(s.c_str());
    }
}

void MainWindow::EditQueueDialog()
{
    queueEditDialog->setModal(true);
    queueEditDialog->UpdateTextWidget(ACQUFilesQueue);
    queueEditDialog->exec();
}

void MainWindow::QueueDialogConfirm()
{
    ACQUFilesQueue.clear();
    ACQUFilesQueue = queueEditDialog->getNewQueueList();
}

void MainWindow::StartQueue()
{
    this->ACQUdirChanged("force");
}

void MainWindow::CompleteButtonSave()
{
    std::cout << "Resaving accumulated files" << std::endl;
    std::string MasterPrefix = "AccumulatedACQU ";
    std::string MasterPhysicsPrefix = "AccumulatedPhysics ";

    QDate date = QDate::currentDate();
    std::string datePrefix = date.toString("yyyy MM dd - ").toStdString() + QTime::currentTime().toString().toStdString();

    /* Copying Accumulated ACQU */
    QFileInfo AccumulatedAcquFile(configGUI.getCompleteACQUFile().c_str());
    std::string AccumulatedAcquDirectory = AccumulatedAcquFile.absoluteDir().absolutePath().toStdString();

    std::string newAccumulatedAcquPath = AccumulatedAcquDirectory  + std::string("/") + MasterPrefix + datePrefix + std::string(".root");
    if (QFile::copy(AccumulatedAcquFile.absoluteFilePath(), newAccumulatedAcquPath.c_str()))
        tabLog->AppendTextNL(TabLog::ColorB("Accumulated ", "DarkOrange") + TabLog::ColorB("ACQU", "CadetBlue") + "  resaved as : " + TabLog::Color(newAccumulatedAcquPath, "DarkOliveGreen"));


    /* Copying Accumulated ACQU */
    QFileInfo AccumulatedPhysicsFile(configGUI.getCompletePhysicsFile().c_str());
    std::string AccumulatedPhysicsDirectory = AccumulatedPhysicsFile.absoluteDir().absolutePath().toStdString();

    std::string newAccumulatedPhysicsPath = AccumulatedPhysicsDirectory  + std::string("/") + MasterPhysicsPrefix + datePrefix + std::string(".root");
    if (QFile::copy(AccumulatedPhysicsFile.absoluteFilePath(), newAccumulatedPhysicsPath.c_str()))
        tabLog->AppendTextNL(TabLog::ColorB("Accumulated ", "DarkOrange") + TabLog::ColorB("Physics", "RoyalBlue") + "  resaved as : " + TabLog::Color(newAccumulatedPhysicsPath, "DarkOliveGreen"));

    QFile::remove(configGUI.getCompletePhysicsFile().c_str());
    QFile::remove(configGUI.getCompleteACQUFile().c_str());
}
