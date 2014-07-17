#include "TabLog.h"
#include "ui_tablog.h"

TabLog::TabLog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabLog)
{
    ui->setupUi(this);
    ui->textBrowser->setAcceptRichText(true);
    date = QDate::currentDate();

    LogFileName = date.toString("yyyy MM dd - ").toStdString() + QTime::currentTime().toString().toStdString();


    connect(ui->textBrowser, SIGNAL(textChanged()), this, SLOT(WriteLogToFile()));

}

TabLog::~TabLog()
{
    delete ui;
}

void TabLog::AppendText1L(std::string title, char* color, std::string str)
{
    std::string Time =  QTime::currentTime().toString().toStdString();
    std::string newText = Bold(Time + " : ") + Color(title, color) + str;
    ui->textBrowser->append(QString(newText.c_str()).replace('\n', "<br>"));
}

void TabLog::AppendText1L(char* t, char* color, std::string str)
{
    std::string title(t);
    std::string Time =  QTime::currentTime().toString().toStdString();
    std::string newText = Bold(Time + " : ") + Color(title, color) + str;
    ui->textBrowser->append(QString(newText.c_str()).replace('\n', "<br>"));
}

void TabLog::AppendTextNL(std::string str)
{
    std::string Time =  QTime::currentTime().toString().toStdString();
    std::string newText = Bold(Time + ": ") + str;
    ui->textBrowser->append(QString(newText.c_str()).replace('\n', "<br>"));
}

void TabLog::AppendTextNL(std::string title, std::string str)
{
    std::string Time =  QTime::currentTime().toString().toStdString();
    std::string newText = Bold(Time + " : ") + title + "\n" + str;
    ui->textBrowser->append(QString(newText.c_str()).replace('\n', "<br>"));
}

void TabLog::AppendTextNL(std::string title, char* color, std::string str)
{
    std::string Time =  QTime::currentTime().toString().toStdString();
    std::string newText = Bold(Time + " : ") + Color(title, color) + "\n" + str;
    ui->textBrowser->append(QString(newText.c_str()).replace('\n', "<br>"));
}

void TabLog::setLabelLastACQU(std::string s)
{
    ui->labelLastACQU->setText(s.c_str());
}

void TabLog::setLabelLastGoAT(std::string s)
{
    ui->labelCurrentGoat->setText(s.c_str());
}

void TabLog::setLabelLastPhys(std::string s)
{
    ui->labelLastPhysics->setText(s.c_str());
}

std::string TabLog::Bold(std::string str)
{
    return "<b>" + str + "</b>";
}

std::string TabLog::Color(std::string str, char* color)
{
    return std::string("<font color=\"") + color + std::string("\">") + str + std::string("</font>");
}

std::string TabLog::ColorB(std::string str, char* color)
{
    return Bold(Color(str, color));
}

std::string TabLog::getLabelACQU()
{
    return ui->labelLastACQU->text().toStdString();
}

std::string TabLog::getLabelGoAT()
{
    return ui->labelCurrentGoat->text().toStdString();
}

std::string TabLog::getLabelPhys()
{
    return ui->labelLastPhysics->text().toStdString();
}

QPushButton* TabLog::getButtonRunGoAT()
{
    return ui->buttonForceRunGoAT;
}

QPushButton* TabLog::getButtonKillGoAT()
{
    return ui->buttonKillGoAT;
}

QPushButton* TabLog::getButtonRunPhys()
{
    return ui->buttonForceRunPhys;
}

QPushButton* TabLog::getButtonKillPhys()
{
    return ui->buttonKillPhys;
}

QPushButton* TabLog::getButtonList()
{
    return ui->buttonList;
}

QPushButton* TabLog::getButtonEditQueue()
{
    return ui->buttonEditQueue;
}

QPushButton* TabLog::getButtonStartQueue()
{
    return ui->buttonStartQueue;
}

void TabLog::WriteLogToFile()
{
    std::ofstream outfile (QCoreApplication::applicationDirPath().toStdString() + std::string("/logs/" + LogFileName + ".txt"), std::ofstream::binary);
    std::string outputData(ui->textBrowser->document()->toPlainText().toStdString());

    outfile.write (outputData.c_str(),outputData.size());
    outfile.close();
}

void TabLog::on_pushButton_clicked()
{
    ui->textBrowser->setText("");
    LogFileName = QDate::currentDate().toString().toStdString() + QTime::currentTime().toString().toStdString();
}
