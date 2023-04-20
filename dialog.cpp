#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 760, 410);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    this->putSpinBoxesIntoArrays();
    this->drawAxis();

    QGraphicsRectItem * rect = new QGraphicsRectItem(0,0,40,300);
    rect->setPos(5,50);
    scene->addItem(rect);

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::disableSpinBoxesBasedOnSelectedAmount(int numOfProcesses)
{

    for(int i = 0 ; i < numOfProcesses; i++ ){
        brojCiklusaSpinBoxes[i]->setEnabled(true);
        dolazakSpinBoxes[i]->setEnabled(true);
        if(ui->algoritam->currentText() == "Prioritet"){
            prioritetSpinBoxes[i]->setEnabled(true);
        }
    }

    for(int i = numOfProcesses ; i < 7; i++ ){
        brojCiklusaSpinBoxes[i]->setEnabled(false);
        dolazakSpinBoxes[i]->setEnabled(false);
        if(ui->algoritam->currentText() == "Prioritet"){
            prioritetSpinBoxes[i]->setEnabled(false);
        }
    }

}

void Dialog::on_pushButton_clicked()
{
    scene->clear();
//    Helping rect
//    QGraphicsRectItem * rect = new QGraphicsRectItem(0,0,40,300);
//    rect->setPos(5,50);
//    scene->addItem(rect);

    this->drawAxis();
    QString algoritam = ui->algoritam->currentText();
    qDebug()<<algoritam;
    QString brojProcesa = ui->brojProcesa->currentText();
    qDebug()<<brojProcesa;
    QFont font("Helvetica", 13);
    for(int i = 1; i <= brojProcesa.toInt(); i++ ){
    QGraphicsTextItem * procesAxisLabel = new QGraphicsTextItem("P" + QString::number(i));
    procesAxisLabel->setPos(10, (300/(brojProcesa.toInt()+1))*i + 35);
    procesAxisLabel->setFont(font);
    procesAxisLabel->setDefaultTextColor(Qt::blue);
    scene->addItem(procesAxisLabel);
    }
}

void Dialog::on_brojProcesa_activated(const QString &brojProcesa)
{
    disableSpinBoxesBasedOnSelectedAmount(brojProcesa.toInt());
}


void Dialog::on_algoritam_activated(const QString &arg1)
{
    if(arg1 == "Prioritet"){
        disableSpinBoxesBasedOnSelectedAmount(ui->brojProcesa->currentText().toInt());
    }else if(arg1 != "Prioritet"){
        for(int i = 0; i < 7 ; i++){
            prioritetSpinBoxes[i]->setEnabled(false);
        }
    }
}

void Dialog::putSpinBoxesIntoArrays()
{
    prioritetSpinBoxes[0] = ui->prioritetP1;
    prioritetSpinBoxes[1] = ui->prioritetP2;
    prioritetSpinBoxes[2] = ui->prioritetP3;
    prioritetSpinBoxes[3] = ui->prioritetP4;
    prioritetSpinBoxes[4] = ui->prioritetP5;
    prioritetSpinBoxes[5] = ui->prioritetP6;
    prioritetSpinBoxes[6] = ui->prioritetP7;

    brojCiklusaSpinBoxes[0] = ui->brojCiklusaP1;
    brojCiklusaSpinBoxes[1] = ui->brojCiklusaP2;
    brojCiklusaSpinBoxes[2] = ui->brojCiklusaP3;
    brojCiklusaSpinBoxes[3] = ui->brojCiklusaP4;
    brojCiklusaSpinBoxes[4] = ui->brojCiklusaP5;
    brojCiklusaSpinBoxes[5] = ui->brojCiklusaP6;
    brojCiklusaSpinBoxes[6] = ui->brojCiklusaP7;

    dolazakSpinBoxes[0] = ui->dolazakP1;
    dolazakSpinBoxes[1] = ui->dolazakP2;
    dolazakSpinBoxes[2] = ui->dolazakP3;
    dolazakSpinBoxes[3] = ui->dolazakP4;
    dolazakSpinBoxes[4] = ui->dolazakP5;
    dolazakSpinBoxes[5] = ui->dolazakP6;
    dolazakSpinBoxes[6] = ui->dolazakP7;
}

void Dialog::drawAxis()
{
    QPen pen(Qt::blue);
    QFont font("Helvetica", 15);
    pen.setWidth(2);

    QGraphicsLineItem* brojCiklusaLine = new QGraphicsLineItem(0,370, 750, 370);
    QGraphicsLineItem* brojCiklusaLineArrowLeft = new QGraphicsLineItem(750,370, 720, 350);
    QGraphicsLineItem* brojCiklusaLineArrowRight = new QGraphicsLineItem(750,370, 720, 390);
    QGraphicsTextItem* brojCiklusaText = new QGraphicsTextItem("Broj Ciklusa");

    brojCiklusaLine->setPen(pen);
    brojCiklusaLineArrowLeft->setPen(pen);
    brojCiklusaLineArrowRight->setPen(pen);
    brojCiklusaText->setDefaultTextColor(Qt::blue);
    brojCiklusaText->setFont(font);
    brojCiklusaText->setPos(600,370);

    scene->addItem(brojCiklusaLine);
    scene->addItem(brojCiklusaText);
    scene->addItem(brojCiklusaLineArrowLeft);
    scene->addItem(brojCiklusaLineArrowRight);

    QGraphicsLineItem* procesiLine = new QGraphicsLineItem(50,10, 50, 410);
    QGraphicsLineItem* procesiLineArrowLeft = new QGraphicsLineItem(50,10, 30, 40);
    QGraphicsLineItem* procesiLineArrowRight = new QGraphicsLineItem(50,10, 70, 40);
    QGraphicsTextItem* procesiText = new QGraphicsTextItem("Procesi");

    procesiLine->setPen(pen);
    procesiLineArrowLeft->setPen(pen);
    procesiLineArrowRight->setPen(pen);
    procesiText->setFont(font);
    procesiText->setDefaultTextColor(Qt::blue);
    procesiText->setPos(80,10);


    scene->addItem(procesiLine);
    scene->addItem(procesiLineArrowLeft);
    scene->addItem(procesiLineArrowRight);
    scene->addItem(procesiText);
}
