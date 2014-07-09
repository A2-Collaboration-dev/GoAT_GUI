#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <TROOT.h>
#include <TSystemFile.h>
#include <TSystemDirectory.h>

#include <chrono>
#include <thread>

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

    /*
     * Setting labels
     */
    tabLog->setLabelLastACQU(configGUI.getLastFile());
    tabLog->setCurrentGoAT(configGUI.getLastGoATFile());


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
    this->process = new QProcess(this);
    this->arguments = new QStringList;
    connect(process, SIGNAL(finished(int)), this, SLOT(newGoatFile()));


    //connect(tabLog->)
    this->tabLog->AppendTextNL("GUI Initialized.");
}

MainWindow::~MainWindow()
{
    continueScanning = false;
    process->kill();
    process->close();
    delete process;
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
  std::cout << "ACQUdirChanged method called." << std::endl;
  int MaxContinueAttempts = 5;

  std::cout << continueScanning << std::endl;
  std::cout << OpeningAtempt << std::endl;
  if (this->continueScanning)
  {
      if (this->OpeningAtempt >= MaxContinueAttempts)
      {
          tabLog->AppendText1L("File Error: ", "DarkMagenta", "Could not open " + this->curFile + ". Possible causes: file is still being copied or is corrupted.");
          this->OpeningAtempt = 0;
          this->continueScanning = true;
          return;
      }

      std::string newFile = getNewestFile(configGUI.getACQUDir(), "*.root");

      if ((newFile != curFile && newFile != "") || OpeningAtempt > 0)
      {
          this->curFile = newFile;
          tabLog->setLabelLastACQU(this->curFile);
          configGUI.setLastACQUFile(this->curFile);

          if (this->OpeningAtempt == 0)
            tabLog->AppendTextNL("New file detected: " + this->curFile);

          /*
           * exe goat make changes here, goat is incompatible with custom configFile path
           */
          *arguments << "-f" << this->curFile.c_str()
                     << "-D" << configGUI.getGoATDir().c_str()
                     << (QCoreApplication::applicationDirPath().toStdString() + std::string("/config/GoAT-config.dat")).c_str();

          /*
           * Experimental
           */

          TFile *file_in = TFile::Open(this->curFile.c_str());
          if(!file_in)
          {
              tabLog->AppendTextNL("File is being accessed by another program, trying again in 5 seconds. (" + std::to_string(MaxContinueAttempts - this->OpeningAtempt) + ")");
              std::cout << "Could not open file, maybe being written. " << this->OpeningAtempt << std::endl;

              QTime dieTime = QTime::currentTime().addMSecs( 10000 );
              while( QTime::currentTime() < dieTime )
              {
                  QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
              }
              //std::this_thread::sleep_for(std::chrono::milliseconds(5000));

              this->OpeningAtempt++;
              this->ACQUdirChanged(path);
              return;
          }
          file_in->Close();

          //std::this_thread::sleep_for(std::chrono::milliseconds(1000));

          this->continueScanning = false;
          this->OpeningAtempt = 0;
          std::cout << "stop scanning" << std::endl;

          QTime dieTime = QTime::currentTime().addMSecs( 5000 );
          while( QTime::currentTime() < dieTime )
          {
              QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
          }

          tabLog->AppendTextNL("Starting " + TabLog::ColorB("GoAT", "BlueViolet") + " with " + TabLog::Color(this->curFile, "DarkOliveGreen"));
          //process->setWorkingDirectory("/home/august/a2GoAT/");
          process->start(configGUI.getGoATExe().c_str(), *arguments);



      }
  }
}


void MainWindow::newGoatFile()
{
    QString p_stdout = process->readAllStandardOutput();
    QString p_stderr = process->readAllStandardError();
    QString pa = process->readAll();

    std::cout << "Output: \n" << p_stdout.toStdString() << std::endl;
    std::cout << "Error: \n" << p_stderr.toStdString() << std::endl;
    std::cout << pa.toStdString() << std::endl;

    std::cout << "finished" << std::endl;
    this->continueScanning = true;
    this->OpeningAtempt = 0;

    tabLog->AppendTextNL("GoAT Output Stream", p_stdout.toStdString());
    tabLog->AppendTextNL("GoAT Error Stream", p_stderr.toStdString());


    // NEED to get GOAT file name to display it.
    //this->tabRunByRun->updateRootFile(curFile.c_str());
    //this->tabRunByRun->updateAllGraphics();
}

void MainWindow::on_actionEdig_config_file_triggered()
{
    configurationDialog = new ConfigurationDialog(this);
    configurationDialog->updateUIText();
    configurationDialog->setModal(true);
    configurationDialog->exec();
}
