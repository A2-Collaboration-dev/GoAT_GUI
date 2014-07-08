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

    std::string Bold(std::string str);
    std::string Color(std::string, char *color);
private:
    Ui::TabLog *ui;
};

#endif // TABLOG_H
