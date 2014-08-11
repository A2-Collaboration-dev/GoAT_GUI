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
#include "QPushButton"

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
    void CloseFile();


    QPushButton* getButtonSave();
    QPushButton* getButtonSave2();

public slots:
        void UpdateAllGraphics();
        void UpdateGraphicsPhysics();
        void UpdateGraphicsDetectors();

private slots:
        void on_pushButton_clicked();

        void on_pushButtonZoomIn_clicked();

        void on_pushButtonZoomOut_clicked();

        void on_buttonTaps1R_clicked();

        void on_buttonTaps1Plus_clicked();

        void on_buttonTaps1Minus_clicked();

        void on_buttonTaps2R_clicked();

        void on_buttonTaps2Plus_clicked();

        void on_buttonTaps2Minus_clicked();

        void on_buttonTaps3R_clicked();

        void on_buttonTaps3Plus_clicked();

        void on_buttonTaps3Minus_clicked();

        void on_buttonPAR_clicked();

        void on_buttonPAPlus_clicked();

        void on_buttonTapsPAM_clicked();


private:
    Ui::TabComplete *ui;

    ConfigGUI configGUI;
    int horizontalSize;
    int verticalSize;
    TFile*  file;
    TFile* PhysicsFile;

    TH2F * h2;
    void FillWidget(TQtWidget *Twidget, TFile* tfile, std::string detector, std::string filename);
    std::vector<float> VerticalSizeExt;
};

#endif // TABCOMPLETE_H
