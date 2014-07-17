#include "QueueEditDialog.h"
#include "ui_queueeditdialog.h"

QueueEditDialog::QueueEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QueueEditDialog)
{
    ui->setupUi(this);
}

QueueEditDialog::~QueueEditDialog()
{
    delete ui;
}

QPushButton* QueueEditDialog::getButtonOK()
{
    return ui->ButtonSave;
}

QPushButton* QueueEditDialog::getButtonSave()
{
    return ui->ButtonSave;
}

void QueueEditDialog::UpdateTextWidget(std::vector<std::string> vect)
{
    ui->textEdit->clear();

    foreach(std::string s, vect)
    {
        ui->textEdit->append(s.c_str());
    }
}

void QueueEditDialog::on_ButtonCancel_clicked()
{
    this->close();
}

void QueueEditDialog::on_ButtonSave_clicked()
{
    QString plainTextEditContents = ui->textEdit->toPlainText();
    QStringList lines = plainTextEditContents.split("\n");

    foreach(QString s, lines)
    {
        if (s.isEmpty()) continue;
        this->NewQueueList.push_back(s.toStdString());
        std::cout << s.toStdString() << std::endl;
    }

    this->close();
}

std::vector<std::string> QueueEditDialog::getNewQueueList()
{
    return this->NewQueueList;
}
