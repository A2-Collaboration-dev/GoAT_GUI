#include "MainWindow.h"
#include "ui_mainwindow.h"

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
     * Using QFileSystemWatcher to check if ACQU directory was updated
     */
    this->curFile = this->configGUI.getLastFile();
    this->continueScanning = true;
    this->watcher = new QFileSystemWatcher(this);
    this->watcher->addPath(this->configGUI.getACQUDir().c_str());
    connect(watcher, SIGNAL(directoryChanged(const QString &)), this, SLOT(ACQUdirChanged(const QString &)));

    /*
     * Signal of when GoAT is finished
     */
    this->process = new QProcess(this);
    this->arguments = new QStringList;
    connect(process, SIGNAL(finished(int)), this, SLOT(newGoatFile()));


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

void MainWindow::ACQUdirChanged(const QString & path)
{
  std::cout << "ACQUdirChanged method called." << std::endl;

  if (this->continueScanning)
  {
      std::string newFile = getNewestFile(configGUI.getACQUDir(), "*.root");

      if (newFile != curFile && newFile != "")
      {
          this->curFile = newFile;
          tabLog->AppendTextNL("New file detected: " + this->curFile);

          /*
           * exe goat make changes here, goat is incompatible with custom configFile path
           */
          *arguments << "-f" << "Raw_CB_41901.root" << "configfiles/GoAT-example.dat";

          /*
           * Experimental
           */
          std::ifstream ifs(this->curFile);
          if (!ifs)
          {
              tabLog->AppendTextNL("Input stream seems to be associated with: " + this->curFile);
              std::cout << "bla bla error " << std::endl;
              return;
          }

              tabLog->AppendTextNL("Starting <b>GoAT</b> with " + this->curFile);
              process->setWorkingDirectory("/home/august/a2GoAT/");
              process->start("build/bin/goat", *arguments);

              this->continueScanning = false;

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
