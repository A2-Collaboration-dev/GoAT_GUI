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
//#include "Functions.cpp"


#include "TGraph.h"
#include "TQtWidget.h"
#include "TCanvas.h"
#include "TLine.h"

#include "Functions.cpp"
#include <QFileSystemWatcher>
#include <QProcess>
#include <TROOT.h>

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

private:
    bool continueScanning;

    Ui::MainWindow *ui;

    QFuture<void> future;
    void FileChecker();

    ConfigurationDialog *configurationDialog;

    TabRunByRun* tabRunByRun;
    TabComplete* tabComplete;
    TabLog* tabLog;

    std::string absolutePath;
    ConfigGUI configGUI;
    QFileSystemWatcher * watcher;

    std::string curFile;

    QProcess* process;
    QStringList* arguments;

private slots:

  void ACQUdirChanged(const QString & path);
  void newGoatFile();

  void on_actionEdig_config_file_triggered();
};

#endif // MAINWINDOW_H
