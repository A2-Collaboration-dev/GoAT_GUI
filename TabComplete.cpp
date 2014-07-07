#include "TabComplete.h"
#include "ui_tabcomplete.h"

TabComplete::TabComplete(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabComplete)
{
    ui->setupUi(this);
}

TabComplete::~TabComplete()
{
    delete ui;
}
