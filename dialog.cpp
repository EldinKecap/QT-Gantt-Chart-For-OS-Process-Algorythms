#include "dialog.h"
#include "ui_dialog.h"
#include "proces.h"

bool compareDolazak(const Proces* p1, const Proces* p2) {
    return p1->dolazakUCiklus < p2->dolazakUCiklus;
}

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

//    QGraphicsRectItem * rect = new QGraphicsRectItem(0,0,40,300);
//    rect->setPos(5,50);
//    scene->addItem(rect);

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
        brojCiklusaSpinBoxes[i]->setValue(0);
        dolazakSpinBoxes[i]->setValue(0);
        if(ui->algoritam->currentText() == "Prioritet"){
            prioritetSpinBoxes[i]->setEnabled(false);
            prioritetSpinBoxes[i]->setValue(0);
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
//    int size = sizeof(prioritetSpinBoxes) / sizeof(QSpinBox*);
//    qDebug()<<size;


    if(algoritam == "FCFS"){
        QFont font("Helvetica", 13);
        float lengthOfAllProcesses = 0;
        //CREATE ARRAY OF PROCESSES FOR SORTING AND CALCULATE THE EXECUTION CYCLE LENGTH
        for(int i = 0; i < brojProcesa.toInt(); i++){
            lengthOfAllProcesses += brojCiklusaSpinBoxes[i]->value();
            QString naziv = "P" + QString::number(i + 1);
            Proces* proces = new Proces(naziv ,brojCiklusaSpinBoxes[i]->value() ,dolazakSpinBoxes[i]->value());
            this->procesArray[i] = proces;
        }
        // SORTING THE ARRAY BASED ON DOLAZAK
        std::sort(procesArray, procesArray + brojProcesa.toInt(), compareDolazak);

        if(lengthOfAllProcesses == 0){
            qDebug()<<"Unesite Broj ciklusa trajanja";
            return;
        }

        for (int i = 0; i < brojProcesa.toInt(); i++) {
            procesArray[i]->procenatBrojaCiklusa = (procesArray[i]->brojCiklusa / lengthOfAllProcesses)*100;
        }

    // DRAWING PROCESSES
        for(int i = 1; i <= brojProcesa.toInt(); i++ ){
            //drawing processes
// OVDJE SAM STAO
            qDebug()<<procesArray[i-1]->procenatBrojaCiklusa;
            float rectWidth = (procesArray[i-1]->procenatBrojaCiklusa/100) * 650;
            qDebug()<<(int)rectWidth;
   //         /////////////////////////////////

            int rectHeight = 300/brojProcesa.toInt();
            QGraphicsRectItem * rect = new QGraphicsRectItem(0,0,rectWidth,rectHeight);
            rect->setPos(50, 50 + rectHeight*(i-1));
            scene->addItem(rect);

            QGraphicsTextItem * procesAxisLabel = new QGraphicsTextItem("P" + QString::number(i));
            procesAxisLabel->setPos(10, (330/(brojProcesa.toInt()+1))*i + 20);
            procesAxisLabel->setFont(font);
            procesAxisLabel->setDefaultTextColor(Qt::blue);
            scene->addItem(procesAxisLabel);
            qDebug()<<"yo";
        }
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
            prioritetSpinBoxes[i]->setValue(0);
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
