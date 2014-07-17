#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QFile>
#include "ConfigurationDialog.h"
#include "TabRunByRun.h"
#include "TabComplete.h"
#include "TabLog.h"

#include "ConfigFile.h"
#include "ConfigGui.h"
#include "Functions.h"


#include "TGraph.h"
#include "TQtWidget.h"
#include "TCanvas.h"
#include "TLine.h"


#include <QFileSystemWatcher>
#include <QProcess>
#include <TROOT.h>
#include <QFuture>

#include <QWaitCondition>
#include <QMutex>
#include <thread>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setAbsolutePath(std::string);
    void StreamMonitor();


private:
    bool continueScanning;
    std::vector<std::string> ACQUFilesQueue;
    std::vector<std::string> FinishedACQUFiles;

    Ui::MainWindow *ui;

    QFuture<void> future;
    QThread *thread;

    void FileChecker();
    void TakeANap(int ms);

    void UpdateCompleteACQU(const char *inputFile);
    void UpdateCompletePhysics(const char *inputFile);
    void CompleteExperimentDataUpdate(const char *inputFile, const char *outputFile);
    void CompleteExperimentDataCreate(const char *inputFile, const char *outputFile);

    ConfigurationDialog *configurationDialog;

    TabRunByRun* tabRunByRun;
    TabComplete* tabComplete;
    TabLog* tabLog;

    std::string absolutePath;
    ConfigGUI configGUI;
    QFileSystemWatcher * watcher;

    std::string curFile;
    std::string ACQUFile;

    QProcess* GoATProcess;
    QStringList* GoATarguments;

    QProcess* PhysicsProcess;
    QStringList* PhysicsArguments;

    int OpeningAtempt;

    /* Test */
    QWaitCondition waitCondition;
    QMutex mutex;

    bool TransientState;


private slots:

  void ACQUdirChanged(QString path);
    void ACQUdirChanged2(QString path);
  void ListQueue();
  void newGoatFile();
  void newPhysicsFile();
  void ForceRunGoAT();
  void ForceRunPhysics();
  void killGoatProcess();
  void killPhysicsProcess();
  void on_actionEdig_config_file_triggered();
  void RunGoat();
  void ExitingGoat();
  void on_actionToggle_Plot_lock_triggered();

  std::string getNewestFile(std::string iDirIn, char* extension);
  void ExcludeFiles(std::string iDirIn, char* extension);
};

#endif // MAINWINDOW_H
