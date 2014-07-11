#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <TROOT.h>
#include <TSystemFile.h>
#include <TSystemDirectory.h>

#include <chrono>
#include <thread>

#include <QFileInfo>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


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
     * Constructing control tab widget
     */
    ui->tabWidget->addTab(tabRunByRun, QString("Run By Run"));
    ui->tabWidget->addTab(tabComplete, QString("Complete"));
    ui->tabWidget->addTab(tabLog, QString("Events Log"));

    /*
     * Calling GUI file and checking if it was read
     */

    if (!configGUI.loadGUIConfigFile(QCoreApplication::applicationDirPath().toStdString() + std::string("/config/settings.gui")))
    {
        std::cout << "GUI Config file not found at: " <<
                     QCoreApplication::applicationDirPath().toStdString() + std::string("/config/settings.gui") << std::endl;
        return;
    }
    configGUI.setConfigFilePath(QCoreApplication::applicationDirPath().toStdString() + std::string("/config/settings.gui"));

    /*
     * Setting labels
     */
    tabLog->setLabelLastACQU(configGUI.getLastFile());
    tabLog->setLabelLastGoAT(configGUI.getLastGoATFile());


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

    this->tabLog->AppendTextNL("GUI Initialized.");

}

MainWindow::~MainWindow()
{
    continueScanning = false;
    GoATProcess->kill();
    GoATProcess->close();
    delete GoATProcess;
    delete ui;
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

    std::string newFile = getNewestFile(configGUI.getACQUDir(), "*.root");

    /* Checking if this file was already used before */
    if((std::find(FinishedACQUFiles.begin(), FinishedACQUFiles.end(), newFile) != FinishedACQUFiles.end()))
    {
        tabLog->AppendTextNL("File was already used in GoAT: " + TabLog::Color(newFile, "DarkOliveGreen"));
        return;
    }


    if ((newFile != this->curFile && newFile != "") || this->OpeningAtempt > 0)
    {
        if ((this->GoATProcess->state() == QProcess::NotRunning && this->PhysicsProcess->state() == QProcess::NotRunning)
          || this->OpeningAtempt > 0) // (not needed?)
        {
            /* Attempt to open the file that was being used by another program */
            if (this->OpeningAtempt > 0)
            {
                //RunGoat() is already dealing recursively with this case;
                return;
            }

            tabLog->AppendTextNL("New file detected: " + TabLog::Color(newFile, "DarkOliveGreen"));


            /* Get the first file from the queue */
            if (!this->ACQUFilesQueue.empty())
            {
                this->curFile = this->ACQUFilesQueue[0];
                this->ACQUFilesQueue.erase(ACQUFilesQueue.begin());
                tabLog->setLabelLastACQU(this->curFile);
                configGUI.setLastACQUFile(this->curFile);

                tabRunByRun->updateRootFile(this->curFile.c_str());
                tabRunByRun->UpdateGraphicsDetectors();

                RunGoat();
                return;
            }

            /* Standard GoAT call */
            this->curFile = newFile;
            tabLog->setLabelLastACQU(this->curFile);
            configGUI.setLastACQUFile(this->curFile);

            tabRunByRun->updateRootFile(this->curFile.c_str());
            tabRunByRun->UpdateGraphicsDetectors();

            RunGoat();
            return;

        } else {
            // if the file is really new never been in the list
            if(!(std::find(ACQUFilesQueue.begin(), ACQUFilesQueue.end(), newFile) != ACQUFilesQueue.end())) {
                ACQUFilesQueue.push_back(newFile);
                tabLog->AppendTextNL("New file detected: " + TabLog::Color(newFile, "DarkOliveGreen"));
                tabLog->AppendTextNL(TabLog::ColorB("GoAT", "BlueViolet") + " is already running. File added to queue: " + TabLog::Color(newFile, "DarkOliveGreen")  + " [" + std::to_string(ACQUFilesQueue.size()) + "]");
                std::cout << "file in queue" + newFile << std::endl;
            }
        }
    }

}


void MainWindow::RunGoat()
{
    if ((this->GoATProcess->state() != QProcess::NotRunning) &&
         this->PhysicsProcess->state() != QProcess::NotRunning)
        return;

    int MaxContinueAttempts = 5;

    // are we attempting to check file?
    if (this->OpeningAtempt >= MaxContinueAttempts)
    {
        tabLog->AppendText1L("File Error: ", "DarkMagenta", "Could not open " + this->curFile + ". Possible causes: file is still being copied or is corrupted.");
        this->OpeningAtempt = 0;
        return;
    }

    *GoATarguments << "-f" << this->curFile.c_str()
                   << "-D" << configGUI.getGoATDir().c_str()
                   << "-p" << configGUI.getACQUPrefix().c_str()
                   << "-P" << configGUI.getGoATPrefix().c_str()
                   << (QCoreApplication::applicationDirPath().toStdString() + std::string("/config/GoAT-config.dat")).c_str();

    /*
     * Checking if file is in use (usually working)
     */

    QFileInfo qfile(this->curFile.c_str());

    /* Check if files was modified in 5 seconds, else use it */
    if (qfile.lastModified() > QDateTime::currentDateTime().addSecs(-20))
    {
        tabLog->AppendTextNL("Could not open " + TabLog::Color(this->curFile, "DarkOliveGreen") + ", trying again in 5 seconds. (" + std::to_string(MaxContinueAttempts - this->OpeningAtempt) + ")");
        std::cout << "Could not open file, maybe being written. " << this->OpeningAtempt << std::endl;

        TakeANap(5000);
        this->OpeningAtempt++;
        this->RunGoat();
        return;
    }

    /* File seems to be okay, taking a nap and starting GoAT */
    this->OpeningAtempt = 0;

    TakeANap(5000);
    tabLog->AppendTextNL("Starting " + TabLog::ColorB("GoAT", "BlueViolet") + " with " + TabLog::Color(this->curFile, "DarkOliveGreen"));

    if (this->GoATProcess->state() == QProcess::NotRunning)
        GoATProcess->start(configGUI.getGoATExe().c_str(), *GoATarguments);

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
    this->RunGoat();
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
    //QString tempTrick(tabLog->getLabelGoAT().c_str());
   // tempTrick = tempTrick.replace(configGUI.getGoATConfig(), configGUI.getACQUPrefix());

   // this->curFile = tempTrick.toStdString();
    //may cause bugs if
   // this->RunGoat();
}

void MainWindow::newGoatFile()
{
    QString p_stdout = GoATProcess->readAllStandardOutput();
    QString p_stderr = GoATProcess->readAllStandardError();
    QString pa = GoATProcess->readAll();

    //std::cout << "Output: \n" << p_stdout.toStdString() << std::endl;
    //std::cout << "Error: \n" << p_stderr.toStdString() << std::endl;
    //std::cout << pa.toStdString() << std::endl;
    std::cout << "finished" << std::endl;

    //this->continueScanning = true;
    this->OpeningAtempt = 0;
    tabLog->AppendTextNL("GoAT Output Stream", p_stdout.toStdString());
    tabLog->AppendTextNL("GoAT Error Stream", p_stderr.toStdString());

    // Only storing changes of files that were used in computation
    configGUI.writeGUIConfigFile();

    /* Obtaining absolute GoAT file path */
    QFileInfo fileInfo(this->curFile.c_str());

    QString GoATFileName = fileInfo.fileName().replace(configGUI.getACQUPrefix().c_str(), configGUI.getGoATPrefix().c_str());
    QString GoATFilePath = QString(configGUI.getGoATDir().c_str()) + GoATFileName;

    QFile GoATFile(GoATFilePath);

    if (GoATFile.exists())
    {
        tabLog->AppendTextNL("New " + TabLog::ColorB("GoAT", "BlueViolet") + " file: " + TabLog::Color(GoATFilePath.toStdString(), "DarkOliveGreen"));
        tabLog->setLabelLastGoAT(GoATFilePath.toStdString());
        configGUI.setLastGoATFile(GoATFilePath.toStdString());

        *PhysicsArguments << "-f" << GoATFilePath.toStdString().c_str()
                          << "-D" << configGUI.getPhysicsDir().c_str()
                          << "-p" << configGUI.getGoATPrefix().c_str()
                          << "-P" << configGUI.getPhysPrefix().c_str()
                          << (QCoreApplication::applicationDirPath().toStdString() + std::string("/config/GoAT-config.dat")).c_str();

        TakeANap(1000);

        tabLog->AppendTextNL("Starting " + TabLog::ColorB("Physics Analysis", "RoyalBlue ") + " with: " + TabLog::Color(GoATFilePath.toStdString(), "DarkOliveGreen"));

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
    GoATProcess->kill();
}

void MainWindow::killPhysicsProcess()
{
    std::cout << "PhysicsProcess kill" << std::endl;
    PhysicsProcess->kill();
}
