#include "TabComplete.h"
#include "ui_tabcomplete.h"

TabComplete::TabComplete(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabComplete)
{
    ui->setupUi(this);

    if (!configGUI.loadGUIConfigFile(QCoreApplication::applicationDirPath().toStdString() + std::string("/config/settings.gui")))
    {
        std::cout << "GUI Config file not found at: " <<
                     QCoreApplication::applicationDirPath().toStdString() + std::string("/config/settings.gui") << std::endl;
        return;
    }

    this->file = new TFile;
    this->PhysicsFile = new TFile;

    this->updateRootFile(configGUI.getCompleteACQUFile().c_str());
    this->updateRootPhysicsFile(configGUI.getCompletePhysicsFile().c_str());

    ui->scrollArea->setMinimumWidth(800);
    ui->scrollArea->setMinimumHeight(600);

    // Only to initially resize Widget to fill the window
    ui->scrollArea->setWidgetResizable(true);

    VerticalSizeExt = {1, 1, 1, 1, 1};

    ui->tabWidget_2->removeTab(3);
    //ui->tabWidget_2->setTabEnabled(3, false);

    std::cout << "init error" << std::endl;
    this->UpdateGraphicsDetectors();
    this->UpdateGraphicsPhysics();
    std::cout << "init error" << std::endl;
}

TabComplete::~TabComplete()
{
    delete ui;
}

void TabComplete::UpdateAllGraphics()
{
    /* Detectors */
    this->UpdateGraphicsDetectors();

    /* Physics Analysis */
    this->UpdateGraphicsPhysics();
}

void TabComplete::UpdateGraphicsPhysics()
{
    std::cout << "Complete updating graphics physics" << std::endl;
    FillWidget(ui->widgetPA, PhysicsFile, std::string("PA"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
}

void TabComplete::UpdateGraphicsDetectors()
{
    std::cout << "Complete updating graphics detectors" << std::endl;
    FillWidget(ui->widget, file, std::string("CB"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
    FillWidget(ui->widgetTaps1, file, std::string("TAPS1"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
    FillWidget(ui->widgetTaps2, file, std::string("TAPS2"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
    FillWidget(ui->widgetTaps3, file, std::string("TAPS3"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
}


void TabComplete::FillWidget(TQtWidget *Twidget, TFile* tfile, std::string detector, std::string filename)
{
    /*
     * Reading gui config file to specify the histograms
     */

    if (!tfile)
    {
        std::cout << "!tfile" << std::endl;
        return;
    }

    if (!tfile->IsOpen())
    {
        std::cout << "!IsOpen()" << std::endl;
        return;
    }

    std::ifstream ifs(QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
    if (!ifs)
    {
        std::cout << "file not found or failed to access, qt!" << std::endl;
        return;
    }

    std::string ConfigFileContent( (std::istreambuf_iterator<char>(ifs) ),
                         (std::istreambuf_iterator<char>()    ) );

    std::vector<std::string> histList;


    Twidget->GetCanvas()->Clear();

    Twidget->GetCanvas()->SetFillStyle(4100);
    Twidget->GetCanvas()->SetFillColor(10);

    getNamesList(detector, histList, ConfigFileContent);

    horizontalSize = histList.size() < 3 ? histList.size() : 3;
    verticalSize = histList.size() % 3 == 0 ? histList.size() / 3 : histList.size() / 3 + 1;

    if (ui->widget == Twidget)
         this->VerticalSizeExt[0] = static_cast<float>(verticalSize)/3 > 1 ? static_cast<float>(verticalSize)/3 : 1;
    if (Twidget == ui->widgetTaps1)
         this->VerticalSizeExt[1] = static_cast<float>(verticalSize)/3 > 1 ? static_cast<float>(verticalSize)/3 : 1;
    if (Twidget == ui->widgetTaps2)
         this->VerticalSizeExt[2] = static_cast<float>(verticalSize)/3 > 1 ? static_cast<float>(verticalSize)/3 : 1;
    if (Twidget == ui->widgetTaps3)
         this->VerticalSizeExt[3] = static_cast<float>(verticalSize)/3 > 1 ? static_cast<float>(verticalSize)/3 : 1;
    if (Twidget == ui->widgetPA)
         this->VerticalSizeExt[4] = static_cast<float>(verticalSize)/3 > 1 ? static_cast<float>(verticalSize)/3 : 1;

    Twidget->GetCanvas()->Divide(horizontalSize, verticalSize, 0.00001, 0.00001, 0);
    for(int i = 0; i < static_cast<int>(histList.size()); i++)
    {
        Twidget->cd(1 + i);
            // ERROR MAY BE HERE
        h2 = (TH2F*)tfile->Get(histList[i].c_str());
        if (h2 == nullptr)
            continue;
        h2->Draw("colz");
    }

    Twidget->Refresh();
    Twidget->update();
}

void TabComplete::updateRootPhysicsFile(const char *ifile)
{
    std::cout << "physics root file updated" << std::endl;

    // Does not work for some magical reason
    //if (this->PhysicsFile->IsOpen())
    //    this->PhysicsFile->Close();
    //if (ifile != nullptr)

    this->PhysicsFile = TFile::Open(ifile);

    if (!this->PhysicsFile)
        std::cout << "could not open physics file" << std::endl;
}

void TabComplete::updateRootFile(const char* ifile)
{
    std::cout << "root file updated "<< ifile << std::endl;

   // Does not work for some magical reason
   // if (this->file->IsOpen())
   //     this->file->Close();
   //if (file != nullptr)

    this->file = TFile::Open(ifile);

    if(!this->file)
        std::cout << "not okay" << std::endl;

    std::cout << "end of update root file" << std::endl;
}

void TabComplete::ReloadRootFile()
{
    std::cout << "Reloading Root File" << std::endl;
    this->file = TFile::Open(configGUI.getCompleteACQUFile().c_str());
}

void TabComplete::ReloadRootPhysicsFile()
{
    std::cout << "Reloading Physics File" << std::endl;
    updateRootPhysicsFile(configGUI.getCompletePhysicsFile().c_str());
}
void TabComplete::on_buttonTaps1R_clicked()
{
    ui->widgetTaps1->GetCanvas()->Clear();
    ui->scrollArea_2->setWidgetResizable(true);
    FillWidget(ui->widgetTaps1,file, std::string("TAPS1"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
    ui->scrollArea_2->setWidgetResizable(false);
    ui->widgetTaps1->resize(QSize(ui->widgetTaps1->size().width(), ui->widgetTaps1->size().height() * VerticalSizeExt[1]));
    ui->scrollAreaWidgetContents_2->resize(QSize(ui->scrollAreaWidgetContents_2->size().width(), ui->scrollAreaWidgetContents_2->size().height() * VerticalSizeExt[1]));
}

void TabComplete::on_buttonTaps2R_clicked()
{
    ui->widgetTaps2->GetCanvas()->Clear();
    ui->scrollArea_3->setWidgetResizable(true);
    FillWidget(ui->widgetTaps2, file, std::string("TAPS2"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
    ui->scrollArea_3->setWidgetResizable(false);
    ui->widgetTaps2->resize(QSize(ui->widgetTaps2->size().width(), ui->widgetTaps2->size().height() * VerticalSizeExt[2]));
    ui->scrollAreaWidgetContents_3->resize(QSize(ui->scrollAreaWidgetContents_3->size().width(), ui->scrollAreaWidgetContents_3->size().height() * VerticalSizeExt[2]));

}

void TabComplete::on_buttonTaps3R_clicked()
{
    ui->widgetTaps3->GetCanvas()->Clear();
    ui->scrollArea_4->setWidgetResizable(true);
    FillWidget(ui->widgetTaps3, file, std::string("TAPS3"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
    ui->scrollArea_4->setWidgetResizable(false);
    ui->widgetTaps3->resize(QSize(ui->widgetTaps3->size().width(), ui->widgetTaps3->size().height() * VerticalSizeExt[3]));
    ui->scrollAreaWidgetContents_4->resize(QSize(ui->scrollAreaWidgetContents_4->size().width(), ui->scrollAreaWidgetContents_4->size().height() * VerticalSizeExt[3]));

}

void TabComplete::on_buttonTaps1Plus_clicked()
{
    ui->scrollArea_2->setWidgetResizable(false);
    ui->widgetTaps1->resize(ui->widgetTaps1->size() + QSize(150, 0));
    ui->scrollAreaWidgetContents_2->resize(ui->scrollAreaWidgetContents_2->size() + QSize(150, 0));
}

void TabComplete::on_buttonTaps2Plus_clicked()
{
    ui->scrollArea_3->setWidgetResizable(false);
    ui->widgetTaps2->resize(ui->widgetTaps2->size() + QSize(150, 0));
    ui->scrollAreaWidgetContents_3->resize(ui->scrollAreaWidgetContents_3->size() + QSize(150, 0));
}

void TabComplete::on_buttonTaps3Plus_clicked()
{
    ui->scrollArea_4->setWidgetResizable(false);
    ui->widgetTaps3->resize(ui->widgetTaps3->size() + QSize(150, 0));
    ui->scrollAreaWidgetContents_4->resize(ui->scrollAreaWidgetContents_4->size() + QSize(150, 0));
}

void TabComplete::on_buttonTaps1Minus_clicked()
{
    ui->scrollArea_2->setWidgetResizable(false);
    ui->widgetTaps1->resize(ui->widgetTaps1->size() + QSize(0, 150));
    ui->scrollAreaWidgetContents_2->resize(ui->scrollAreaWidgetContents_2->size() + QSize(0, 150));
}

void TabComplete::on_buttonTaps2Minus_clicked()
{
    ui->scrollArea_3->setWidgetResizable(false);
    ui->widgetTaps2->resize(ui->widgetTaps2->size() + QSize(0, 150));
    ui->scrollAreaWidgetContents_3->resize(ui->scrollAreaWidgetContents_3->size() + QSize(0, 150));
}

void TabComplete::on_buttonTaps3Minus_clicked()
{
    ui->scrollArea_4->setWidgetResizable(false);
    ui->widgetTaps3->resize(ui->widgetTaps3->size() + QSize(0, 150));
    ui->scrollAreaWidgetContents_4->resize(ui->scrollAreaWidgetContents_4->size() + QSize(0, 150));
}

void TabComplete::on_buttonPAR_clicked()
{
    ui->widgetPA->GetCanvas()->Clear();
    ui->scrollArea_5->setWidgetResizable(true);
    FillWidget(ui->widgetPA, PhysicsFile, std::string("PA"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
    ui->scrollArea_5->setWidgetResizable(false);
    ui->widgetPA->resize(QSize(ui->widgetPA->size().width(), ui->widgetPA->size().height() * VerticalSizeExt[4]));
    ui->scrollAreaWidgetContents_5->resize(QSize(ui->scrollAreaWidgetContents_5->size().width(), ui->scrollAreaWidgetContents_5->size().height() * VerticalSizeExt[4]));
}

void TabComplete::on_buttonPAPlus_clicked()
{
    ui->scrollArea_5->setWidgetResizable(false);
    ui->widgetPA->resize(ui->widgetPA->size() + QSize(150, 0));
    ui->scrollAreaWidgetContents_5->resize(ui->scrollAreaWidgetContents_5->size() + QSize(150, 0));
}

void TabComplete::on_buttonTapsPAM_clicked()
{
    ui->scrollArea_5->setWidgetResizable(false);
    ui->widgetPA->resize(ui->widgetPA->size() + QSize(0, 150));
    ui->scrollAreaWidgetContents_5->resize(ui->scrollAreaWidgetContents_5->size() + QSize(0, 150));
}

void TabComplete::on_pushButtonZoomIn_clicked()
{
    ui->scrollArea->setWidgetResizable(false);
    ui->widget->resize(ui->widget->size() + QSize(150, 0));
    ui->scrollAreaWidgetContents->resize(ui->scrollAreaWidgetContents->size() + QSize(150, 0));
}

void TabComplete::on_pushButtonZoomOut_clicked()
{
    ui->scrollArea->setWidgetResizable(false);
    ui->widget->resize(ui->widget->size() + QSize(0, 150));
    ui->scrollAreaWidgetContents->resize(ui->scrollAreaWidgetContents->size() + QSize(0, 150));
}

void TabComplete::on_pushButton_clicked()
{
    ui->scrollArea->setWidgetResizable(true);
    FillWidget(ui->widget, file, std::string("CB"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
    ui->scrollArea->setWidgetResizable(false);
    ui->widget->resize(QSize(ui->widget->size().width(), ui->widget->size().height() * VerticalSizeExt[0]));
    ui->scrollAreaWidgetContents->resize(QSize(ui->scrollAreaWidgetContents->size().width(), ui->scrollAreaWidgetContents->size().height() * VerticalSizeExt[0]));
    ui->widget->update();
}

QPushButton* TabComplete::getButtonSave()
{
    return ui->buttonSave;
}

QPushButton* TabComplete::getButtonSave2()
{
    return ui->buttonSave2;
}

void TabComplete::CloseFile()
{
    this->file->Close();
}
