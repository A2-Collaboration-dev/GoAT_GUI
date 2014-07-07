#include "TabRunByRun.h"
#include "ui_tabrunbyrun.h"



#include "Functions.cpp"

TabRunByRun::TabRunByRun(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabRunByRun)
{
    ui->setupUi(this);

    file    = TFile::Open("/home/august/a2GoAT/Acqu_Compton_355.root");
    if(!file)
        return;


    ui->scrollArea->setMinimumWidth(800);
    ui->scrollArea->setMinimumHeight(600);

    // Only to initially resize Widget to fill the window
    ui->scrollArea->setWidgetResizable(true);

    ui->widget->GetCanvas()->Clear();  //MyWidget is an instance of TQtWidget
    ui->widget->GetCanvas()->Modified();
    ui->widget->GetCanvas()->Update();

    ui->widget->EnableSignalEvents(kMousePressEvent);
    ui->widget->EnableSignalEvents(kMouseMoveEvent);
    ui->widget->EnableSignalEvents(kMouseReleaseEvent);
    ui->widget->EnableSignalEvents(kMouseDoubleClickEvent);
    ui->widget->EnableSignalEvents(kKeyPressEvent);
    ui->widget->EnableSignalEvents(kEnterEvent);
    ui->widget->EnableSignalEvents(kLeaveEvent);

    /*
     * Filling TQtWidgets
     */


    /* Detectors */
    FillWidget(ui->widget, std::string("CB"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
    FillWidget(ui->widgetTaps1, std::string("TAPS1"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
    FillWidget(ui->widgetTaps2, std::string("TAPS2"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
    FillWidget(ui->widgetTaps3, std::string("TAPS3"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));

    /* Physics Analysis */
    FillWidget(ui->widgetPA, std::string("PA"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
    //updateAllGraphics();
}

TabRunByRun::~TabRunByRun()
{
    file->Close();
    delete ui;
}

void TabRunByRun::updateRootFile(const char* file)
{
    this->file->Close();
    this->file = TFile::Open("/home/august/a2GoAT/Acqu_Compton_355.root");
    std::cout << "root file updated "<< file << std::endl;
    if(!this->file)
        std::cout << "not okay" << std::endl;

}

void TabRunByRun::updateAllGraphics()
{
    /* Detectors */
    FillWidget(ui->widget, std::string("CB"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
    FillWidget(ui->widgetTaps1, std::string("TAPS1"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
    FillWidget(ui->widgetTaps2, std::string("TAPS2"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
    FillWidget(ui->widgetTaps3, std::string("TAPS3"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));

    /* Physics Analysis */
    FillWidget(ui->widgetPA, std::string("PA"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
}

void TabRunByRun::on_pushButtonZoomIn_clicked()
{
    ui->scrollArea->setWidgetResizable(false);
    ui->widget->resize(ui->widget->size() + QSize(150, 150));
    ui->scrollAreaWidgetContents->resize(ui->widget->size() + QSize(150, 150));
}

void TabRunByRun::on_pushButtonZoomOut_clicked()
{
    ui->scrollArea->setWidgetResizable(false);
    ui->widget->resize(ui->widget->size() - QSize(150, 150));
    ui->scrollAreaWidgetContents->resize(ui->widget->size() - QSize(150, 150));
}

void TabRunByRun::on_pushButton_clicked()
{
    ui->widget->GetCanvas()->Clear();
    ui->scrollArea->setWidgetResizable(true);
    FillWidget(ui->widget, std::string("CB"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
}

void TabRunByRun::FillWidget(TQtWidget *Twidget, std::string detector, std::string filename)
{
    /*
     * Reading gui config file to specify the histograms
     */
    std::ifstream ifs(QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
    if (!ifs)
    {
        std::cout << "file not found or failed to access, qt!" << std::endl;
        return;
    }


    std::string ConfigFileContent( (std::istreambuf_iterator<char>(ifs) ),
                         (std::istreambuf_iterator<char>()    ) );

    std::vector<std::string> histList;

    if(!this->file)
        return;


    Twidget->GetCanvas()->Clear();

    Twidget->GetCanvas()->SetFillStyle(4100);
    Twidget->GetCanvas()->SetFillColor(10);

    getNamesList(detector, histList, ConfigFileContent);

    horizontalSize = histList.size() < 3 ? histList.size() : 3;
    verticalSize = histList.size() % 3 == 0 ? histList.size() / 3 : histList.size() / 3 + 1;


    Twidget->GetCanvas()->Divide(horizontalSize, verticalSize);
    for(int i = 0; i < static_cast<int>(histList.size()); i++)
    {
        Twidget->cd(1 + i);
        h2 = (TH2F*)file->Get(histList[i].c_str());
        if (h2 == nullptr)
            continue;
        h2->Draw();
    }

    Twidget->Refresh();
    Twidget->update();
}

void TabRunByRun::on_buttonTaps1R_clicked()
{
    ui->widgetTaps1->GetCanvas()->Clear();
    ui->scrollArea_2->setWidgetResizable(true);
    FillWidget(ui->widgetTaps1, std::string("TAPS1"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));
}

void TabRunByRun::on_buttonTaps2R_clicked()
{
    ui->widgetTaps2->GetCanvas()->Clear();
    ui->scrollArea_3->setWidgetResizable(true);
    FillWidget(ui->widgetTaps2, std::string("TAPS2"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));

}

void TabRunByRun::on_buttonTaps3R_clicked()
{
    ui->widgetTaps3->GetCanvas()->Clear();
    ui->scrollArea_4->setWidgetResizable(true);
    FillWidget(ui->widgetTaps3, std::string("TAPS3"), QCoreApplication::applicationDirPath().toStdString() + std::string("/config/plotdata.gui"));

}

void TabRunByRun::on_buttonTaps1Plus_clicked()
{
    ui->scrollArea_2->setWidgetResizable(false);
    ui->widgetTaps1->resize(ui->widgetTaps1->size() + QSize(150, 150));
    ui->scrollAreaWidgetContents_2->resize(ui->widgetTaps1->size() + QSize(150, 150));
}

void TabRunByRun::on_buttonTaps2Plus_clicked()
{
    ui->scrollArea_3->setWidgetResizable(false);
    ui->widgetTaps2->resize(ui->widgetTaps2->size() + QSize(150, 150));
    ui->scrollAreaWidgetContents_3->resize(ui->widgetTaps2->size() + QSize(150, 150));
}

void TabRunByRun::on_buttonTaps3Plus_clicked()
{
    ui->scrollArea_4->setWidgetResizable(false);
    ui->widgetTaps3->resize(ui->widgetTaps3->size() + QSize(150, 150));
    ui->scrollAreaWidgetContents_4->resize(ui->widgetTaps3->size() + QSize(150, 150));
}

void TabRunByRun::on_buttonTaps1Minus_clicked()
{
    ui->scrollArea_2->setWidgetResizable(false);
    ui->widgetTaps1->resize(ui->widgetTaps1->size() - QSize(150, 150));
    ui->scrollAreaWidgetContents_2->resize(ui->widgetTaps1->size() - QSize(150, 150));
}

void TabRunByRun::on_buttonTaps2Minus_clicked()
{
    ui->scrollArea_3->setWidgetResizable(false);
    ui->widgetTaps2->resize(ui->widgetTaps2->size() - QSize(150, 150));
    ui->scrollAreaWidgetContents_3->resize(ui->widgetTaps2->size() - QSize(150, 150));
}

void TabRunByRun::on_buttonTaps3Minus_clicked()
{
    ui->scrollArea_4->setWidgetResizable(false);
    ui->widgetTaps3->resize(ui->widgetTaps3->size() - QSize(150, 150));
    ui->scrollAreaWidgetContents_4->resize(ui->widgetTaps3->size() - QSize(150, 150));
}
