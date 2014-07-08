#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include <QDialog>
#include <QFileDialog>

#include "ConfigFile.h"
#include "ConfigGui.h"

#include "TGraph.h"
#include "TQtWidget.h"
#include "TCanvas.h"
#include "TLine.h"

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>


#include <functional>
#include <cstring>
#include <vector>
#include <sstream>
#include <string>

struct cmp_str
{
   bool operator()(char const *a, char const *b)
   {
      return std::strcmp(a, b) < 0;
   }
};

namespace Ui {
class ConfigurationDialog;
}

class ConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigurationDialog(QWidget *parent = 0);
    ~ConfigurationDialog();

    void updateUIText();

private slots:

    void on_checkBoxCPreSort_stateChanged(int arg1);

    void on_checkBoxCPR_stateChanged(int arg1);

    void on_checkBoxCMR_stateChanged(int arg1);

    void on_checkBoxCPS_stateChanged(int arg1);

    void on_buttonInputBrowse_clicked();

    void on_buttonInputBrowseDir_clicked();

    void on_buttonOutputBrowse_clicked();

    void on_pushButton_clicked();

    void on_pushButtonPRCBProtons_clicked();

    void on_pushButtonPRCBElectrons_clicked();

    void on_pushButtonPRCBPions_clicked();

    void on_pushButtonPRTapsProtons_clicked();

    void on_pushButtonPRTapsElectrons_clicked();

    void on_pushButtonPRTapsPions_clicked();

    void on_lineEditConfigPreSortParticleTotal_editingFinished();

    void on_lineEditConfigPreSortParticleCB_editingFinished();

    void on_lineEditConfigPreSortParticleTaps_editingFinished();

    void on_comboBox_currentIndexChanged(int index);

    void on_comboBox_3_currentIndexChanged(int index);

    void on_comboBox_4_currentIndexChanged(int index);

    void on_lineEditConfigPreSortEnergySum_editingFinished();

private:
    bool importTreeFromFile(std::string treename, std::string filename);
    bool importTreeFromFile(char* treename, std::string filename);
    void updateTQtWidget();

    Ui::ConfigurationDialog *ui;
    ConfigFile configFile;
    ConfigGUI configGUI;
    TTree*  tree;

    unsigned int HorizontalPads = 1;
    unsigned int VerticalPads = 2;

    std::map<const char*, TLine*, cmp_str> MaxValMap;
    std::map<const char*, TLine*, cmp_str> MinValMap;

    TLine*  GlobalMax;
    TLine*  GlobalMin;

    TLine*  CutMin1;
    TLine*  CutMax1;

    TLine*  CutMin2;
    TLine*  CutMax2;

    std::string lastCutPath;

    static std::string resolvingComboBoxIndex(int index)
    {
        if (index == 0) return std::string("+");
        if (index == 1) return std::string("-");
        if (index == 2) return std::string("=");
        else return std::string("=");
    }

    static int InverseResolvingBoxIndex(std::string s)
    {
        if (s == std::string("+")) return 0;
        if (s == std::string("-")) return 1;
        if (s == std::string("=")) return 2;
        else return 2;
    }

private slots:
    void RootSpinBoxEvent(TObject* object, uint event, TCanvas* canvas);
    void RootCanvasEvent(TObject* object, uint event, TCanvas* canvas);
    void on_spinBoxMin_valueChanged(double arg1);
    void on_spinBoxMax_valueChanged(double arg1);
    void on_pushButtonSelectCuts_clicked();
    void on_lineEditConfigMRWidthPi0_editingFinished();
    void on_lineEditConfigMRWidthEta_editingFinished();
    void on_lineEditConfigMRWidthEtaprime_editingFinished();
};



#endif // CONFIGURATIONDIALOG_H
