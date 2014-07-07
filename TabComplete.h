#ifndef TABCOMPLETE_H
#define TABCOMPLETE_H

#include <QWidget>

namespace Ui {
class TabComplete;
}

class TabComplete : public QWidget
{
    Q_OBJECT

public:
    explicit TabComplete(QWidget *parent = 0);
    ~TabComplete();

private:
    Ui::TabComplete *ui;
};

#endif // TABCOMPLETE_H
