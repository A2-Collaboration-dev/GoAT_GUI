#ifndef TABCOMPLETE_H
#define TABCOMPLETE_H

#include <QWidget>

#include "TH1.h"
#include "TH2F.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TFolder.h"

#include "TQtWidget.h"

#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "ConfigGui.h"
#include "Functions.h"

namespace Ui {
class TabComplete;
}

class TabComplete : public QWidget
{
    Q_OBJECT

public:
    explicit TabComplete(QWidget *parent = 0);
    ~TabComplete();
    void updateRootFile(const char *file);
    void updateRootPhysicsFile(const char *file);
    void ReloadRootFile();
    void ReloadRootPhysicsFile();

public slots:
        void UpdateAllGraphics();
        void UpdateGraphicsPhysics();
        void UpdateGraphicsDetectors();

private:
    Ui::TabComplete *ui;

    ConfigGUI configGUI;
    int horizontalSize;
    int verticalSize;
    TFile*  file;
    TFile* PhysicsFile;

    TH2F * h2;
    void FillWidget(TQtWidget *Twidget, TFile* tfile, std::string detector, std::string filename);
};

#endif // TABCOMPLETE_H
