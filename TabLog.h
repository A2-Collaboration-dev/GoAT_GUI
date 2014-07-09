#ifndef TABLOG_H
#define TABLOG_H

#include <QWidget>
#include <QTime>
#include <iostream>

namespace Ui {
class TabLog;
}

class TabLog : public QWidget
{
    Q_OBJECT

public:
    explicit TabLog(QWidget *parent = 0);
    ~TabLog();

    void AppendTextNL(std::string str);
    void AppendTextNL(std::string title, std::string str);
    void AppendTextNL(std::string title, char* color, std::string str);
    void AppendText1L(std::string title, char* color, std::string str);

    static std::string Bold(std::string str);
    static std::string Color(std::string, char *color);
    static std::string ColorB(std::string, char *color);

    void setLabelLastACQU(std::string);
    void setCurrentGoAT(std::string);
private:
    Ui::TabLog *ui;
};

#endif // TABLOG_H
