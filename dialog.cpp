#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    QString algoritam = ui->algoritam->currentText();
    qDebug()<<algoritam;
    QString brojProcesa = ui->brojProcesa->currentText();
    qDebug()<<brojProcesa;
}
