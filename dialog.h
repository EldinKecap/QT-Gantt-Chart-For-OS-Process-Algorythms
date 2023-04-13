#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QString>
#include <QDebug>
#include <QVector>
#include <QSpinBox>
#include <QGraphicsScene>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    void disableSpinBoxesBasedOnSelectedAmount(int numOfProcesses);

private slots:
    void on_pushButton_clicked();

    void on_brojProcesa_activated(const QString &arg1);

    void on_algoritam_activated(const QString &arg1);

private:
    Ui::Dialog *ui;
    QGraphicsScene *scene;
    QSpinBox* prioritetSpinBoxes[7];
    QSpinBox* brojCiklusaSpinBoxes[7];
    QSpinBox* dolazakSpinBoxes[7];
};

#endif // DIALOG_H
