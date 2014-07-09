#include "TabLog.h"
#include "ui_tablog.h"

TabLog::TabLog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabLog)
{
    ui->setupUi(this);
    ui->textBrowser->setAcceptRichText(true);
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

void TabLog::setCurrentGoAT(std::string s)
{
    ui->labelCurrentGoat->setText(s.c_str());
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

QPushButton* TabLog::getButtonRunGoAT()
{
    return ui->buttonForceRunGoAT;
}
