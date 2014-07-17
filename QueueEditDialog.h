#ifndef QUEUEEDITDIALOG_H
#define QUEUEEDITDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QWidget>
#include <string>
#include <vector>
#include <iostream>
#include "ui_queueeditdialog.h"

namespace Ui {
class QueueEditDialog;
}

class QueueEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QueueEditDialog(QWidget *parent = 0);
    ~QueueEditDialog();
    QPushButton* getButtonOK();
    QPushButton* getButtonSave();

    void UpdateTextWidget(std::vector<std::string> v);
    std::vector<std::string> getNewQueueList();

private slots:
    void on_ButtonCancel_clicked();

    void on_ButtonSave_clicked();

private:
    Ui::QueueEditDialog *ui;

    std::vector<std::string> NewQueueList;
};

#endif // QUEUEEDITDIALOG_H
