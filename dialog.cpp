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
//    qDebug()<<algoritam;
    QString brojProcesa = ui->brojProcesa->currentText();
    int brojProcesaInt = brojProcesa.toInt();
//    qDebug()<<brojProcesa;
    bool isPreemptive =  ui->radioButton->isChecked();

    float lengthOfAllProcesses = 0;
    for(int i = 0; i < brojProcesa.toInt(); i++){
        lengthOfAllProcesses += brojCiklusaSpinBoxes[i]->value();
    }

    if(lengthOfAllProcesses == 0){
        qDebug() << "Unesite Broj ciklusa trajanja";
        return;
    }

    if( algoritam == "FCFS" && !isPreemptive ){
        QFont font("Helvetica", 13);
        QPen pen(Qt::blue);
        QBrush brush(Qt::green);

        //CREATE ARRAY OF PROCESSES FOR SORTING AND CALCULATE THE EXECUTION CYCLE LENGTH
        for(int i = 0; i < brojProcesa.toInt(); i++){
            QString naziv = "P" + QString::number(i + 1);
            Proces* proces = new Proces(naziv ,brojCiklusaSpinBoxes[i]->value() ,dolazakSpinBoxes[i]->value());
            this->procesArray[i] = proces;
        }
        // SORTING THE ARRAY BASED ON DOLAZAK
        std::sort(procesArray, procesArray + brojProcesa.toInt(), compareDolazak);

        for (int i = 0; i < brojProcesa.toInt(); i++) {
            procesArray[i]->procenatBrojaCiklusa = (procesArray[i]->brojCiklusa / lengthOfAllProcesses)*100;
        }

    // DRAWING PROCESSES
        QVector <int> arrOfRectWidths;
        for(int i = 1; i <= brojProcesa.toInt(); i++ ){
            float rectWidth = (procesArray[i-1]->procenatBrojaCiklusa/100) * 650;
            arrOfRectWidths.push_back((int)rectWidth);
            int rectSpacing = 0;
            if(i>1){
            for(int j = 0; j < i-1 ; j++){
                rectSpacing += arrOfRectWidths[j];
                }
            }

            QPen dashedLine = QPen(Qt::DashLine);
            dashedLine.setColor(Qt::blue);
            QGraphicsLineItem* endLine = new QGraphicsLineItem(50 + rectSpacing,10, 50 + rectSpacing, 370);
            endLine->setPen(dashedLine);
            scene->addItem(endLine);

            int rectHeight = 300/brojProcesa.toInt();
            QGraphicsRectItem * rect = new QGraphicsRectItem(0,0,rectWidth,rectHeight);
            rect->setPen(pen);
            rect->setBrush(brush);
            rect->setPos(50 + rectSpacing , 350 - rectHeight*(i));
            scene->addItem(rect);

            QGraphicsTextItem * procesAxisLabel = new QGraphicsTextItem(procesArray[brojProcesa.toInt() - i]->naziv);
            procesAxisLabel->setPos(10, (330/(brojProcesa.toInt()+1))*i + 20 );
            procesAxisLabel->setFont(font);
            procesAxisLabel->setDefaultTextColor(Qt::blue);
            scene->addItem(procesAxisLabel);


        }
    }else if ( algoritam == "FCFS" && isPreemptive ){

        this->fillProcesVector();

        std::sort(procesVector.begin(), procesVector.end(), compareDolazak);
//        printProcesVector();
        // splitting processes based on time of arrival
        for( int i = 0; i < brojProcesaInt ; i++ ){
            if(i+1 < brojProcesaInt){
                if(procesVector[i]->brojCiklusa > procesVector[i+1]->dolazakUCiklus ){
                    int remainingBrojCiklusa = procesVector[i]->brojCiklusa + procesVector[i]->dolazakUCiklus - procesVector[i+1]->dolazakUCiklus ;
                    if(remainingBrojCiklusa != 0){
                        procesVector[i]->brojCiklusa -= remainingBrojCiklusa;
                        Proces* proces = new Proces(procesVector[i]->naziv,remainingBrojCiklusa, 99 );
                        procesVector.push_back(proces);
                    }
                }
            }
        }

//        qDebug() << "_______________ Sliced up array ___________";
//        printProcesVector();
//        int checkAllCiklusi = 0;
        for(Proces* value : procesVector){
//           checkAllCiklusi += value->brojCiklusa;
           value->procenatBrojaCiklusa = (value->brojCiklusa / lengthOfAllProcesses)*100;
        }
//        qDebug() << "Length all:" << lengthOfAllProcesses;
//        qDebug() << "Check Length all:" << checkAllCiklusi;
        // Drawing

        for(int i = 0; i < procesVector.size(); i++ ){
            float rectWidth = (procesVector[i]->procenatBrojaCiklusa/100) * 650;
            int rectHeight = 300/brojProcesaInt;
            procesVector[i]->rectWidth = rectWidth;
            procesVector[i]->rectHeight = rectHeight;
            procesVector[i]->rectSpacingHeight = rectHeight * (i+1);
            if( i > 0 ){
                procesVector[i]->rectSpacing = procesVector[i-1]->rectSpacing + procesVector[i-1]->rectWidth;
            }
}
        for(int i = 0; i < brojProcesaInt; i++ ){
            QString nazivProcesa = procesVector[i]->naziv;
            for(Proces* proces: procesVector){
                if(proces->naziv == nazivProcesa){
                    proces->rectSpacingHeight = procesVector[i]->rectSpacingHeight;
                }
            }
        }


        for(Proces* proces: procesVector){
            QPen pen(Qt::blue);
            QBrush brush(Qt::green);
            QGraphicsRectItem * rect = new QGraphicsRectItem(0,0,proces->rectWidth,proces->rectHeight);
            rect->setPos(50 + proces->rectSpacing , 350 - proces->rectSpacingHeight);
            rect->setPen(pen);
            rect->setBrush(brush);
//            qDebug() << proces->rectSpacingHeight;
            scene->addItem(rect);

            QPen dashedLine = QPen(Qt::DashLine);
            dashedLine.setColor(Qt::blue);
            QGraphicsLineItem* endLine = new QGraphicsLineItem(50 + proces->rectSpacing,10, 50 + proces->rectSpacing, 370);
            endLine->setPen(dashedLine);
            scene->addItem(endLine);
        }

        for(int i = 0; i < brojProcesaInt; i++ ){
                QFont font("Helvetica", 13);
                QGraphicsTextItem * procesAxisLabel = new QGraphicsTextItem(procesVector[brojProcesaInt - (i + 1)]->naziv);
                procesAxisLabel->setPos(10, (330/(brojProcesaInt+1))*(i + 1) + 20 );
                procesAxisLabel->setFont(font);
                procesAxisLabel->setDefaultTextColor(Qt::blue);
                scene->addItem(procesAxisLabel);
        }

        procesVector.clear();
    }else if( algoritam == "SJF" && !isPreemptive ){

        this->fillProcesVector();

        std::sort(procesVector.begin(), procesVector.end(), compareDolazak);

        printProcesVector();

        int executionTime = procesVector[0]->dolazakUCiklus;
        for(int i = 0; i < procesVector.size(); i++){
            qDebug()<<executionTime;
            qDebug()<<procesVector[i]->dolazakUCiklus;
           if( (i+1) < procesVector.size() && procesVector[i]->dolazakUCiklus == procesVector[i+1]->dolazakUCiklus ){
               for(int j = i; j < procesVector.size() ; j++){
                   if(procesVector[i]->brojCiklusa > procesVector[j]->brojCiklusa){
                       qDebug() <<" Switched: " << procesVector[i]->naziv << " with " << procesVector[j]->naziv;
                       Proces * tempProces = procesVector[i];
                       procesVector[i] = procesVector[j];
                       procesVector[j] = tempProces;
                   }
               }

           }else if( executionTime > procesVector[i]->dolazakUCiklus ){
               for(int j = i + 1; j < procesVector.size() ; j++){
                   if(procesVector[i]->brojCiklusa > procesVector[j]->brojCiklusa && executionTime >= procesVector[j]->dolazakUCiklus){
                       qDebug() <<" Switched: " << procesVector[i]->naziv << " with " << procesVector[j]->naziv;
                       Proces * tempProces = procesVector[i];
                       procesVector[i] = procesVector[j];
                       procesVector[j] = tempProces;
                   }
               }
           }
           executionTime += procesVector[i]->brojCiklusa;
//           qDebug()<<executionTime;
        }

        printProcesVector();

        // //////////////////////////////////////////////////////////
        this->drawVectorNonPreemptive();
        // //////////////////////////////////////////////////////////
        procesVector.clear();
    }else if( algoritam == "SJF" && isPreemptive){

        this ->fillProcesVector();
        std::sort(procesVector.begin(), procesVector.end(), compareDolazak);
        printProcesVector();

        int executionTime = 0;
        QVector <Proces*> executionQueue;
        QVector <Proces*> waitingQueue;
//        bool procesIsExecuting = false;
//        int procesStartedExecutingTime;
        bool firstProcesAdded = false;
        qDebug()<<"SJF Preemptive";
        while(true){

            if(procesVector.size() != 0){
                for(int i = 0; i < procesVector.size(); i++){
                    if(procesVector[i]->dolazakUCiklus <= executionTime){
                      waitingQueue.push_back(procesVector[i]);
                      procesVector.remove(i);
                      i--;
                    }
                }
            }

            if(waitingQueue.size() > 0){
                firstProcesAdded = true;
                std::sort(waitingQueue.begin(),waitingQueue.end(),[](Proces* a, Proces* b){
                                    return a->brojCiklusa < b->brojCiklusa;
                                });
                if(executionQueue.size() == 0){
                    executionQueue.push_back(new Proces(waitingQueue[0]->naziv, 0, waitingQueue[0]->dolazakUCiklus));
                    executionQueue[executionQueue.size() - 1]->procesStartedExecuting = executionTime;
                    executionQueue[executionQueue.size() - 1]->dolazakUCiklus = executionTime;
                }
//                else if(executionQueue.size() > 0){
//                    if(executionQueue[executionQueue.size() - 1]->procesStartedExecuting)
//                }
            }
//            qDebug()<<"WaitingQueue";
//            printVector(waitingQueue);

            if(waitingQueue.size() > 0){
            if(executionQueue.size() > 0){



                if(waitingQueue[0]->naziv ==  executionQueue[executionQueue.size() - 1]->naziv ){
                    if(waitingQueue[0]->brojCiklusa == 0){
                        waitingQueue.remove(0);
//                        qDebug()<<"BAM";
                        if(waitingQueue.size() > 0){
//                            qDebug()<<"BAM";
                            executionQueue.push_back(new Proces(waitingQueue[0]->naziv, 0, waitingQueue[0]->dolazakUCiklus));
                            executionQueue[executionQueue.size() - 1]->procesStartedExecuting = executionTime;
                            executionQueue[executionQueue.size() - 1]->dolazakUCiklus = executionTime;
                            executionQueue[executionQueue.size() - 1]->brojCiklusa++;
                            waitingQueue[0]->brojCiklusa--;
                        }

                    }else{
//                        qDebug()<<"BAM else";
                        executionQueue[executionQueue.size() - 1]->brojCiklusa++;
                        waitingQueue[0]->brojCiklusa--;
                    }
                }else{
                    qDebug()<<"not Redundant";
                    executionQueue.push_back(new Proces(waitingQueue[0]->naziv, 0, waitingQueue[0]->dolazakUCiklus));
                    executionQueue[executionQueue.size() - 1]->procesStartedExecuting = executionTime;
                    executionQueue[executionQueue.size() - 1]->dolazakUCiklus = executionTime;
                    executionQueue[executionQueue.size() - 1]->brojCiklusa++;
                    waitingQueue[0]->brojCiklusa--;
                } }
            }
//            qDebug()<< "executionQueue";


            if(waitingQueue.size() == 0 && firstProcesAdded && procesVector.size() == 0){
//                qDebug()<<"BAM end";
                           procesVector = executionQueue;
                           executionQueue.clear();
                           waitingQueue.clear();
                           firstProcesAdded = false;
                           break;
                       }

            executionTime++;
            qDebug()<< "Vrijeme Izvrsavanja: "<<executionTime;
        }


//        executionQueue.clear();
//        drawVectorPreemptive();
        printProcesVector();
        int checkProcesBrojCiklusaLength = 0;
        for(Proces * proces : procesVector){
             checkProcesBrojCiklusaLength += proces->brojCiklusa;
        }

        qDebug()<<"original length all broj Ciklusa: " << lengthOfAllProcesses;
        qDebug()<<"check length all broj Ciklusa: " << checkProcesBrojCiklusaLength;

        // /////////////////////////////////////////////////////////
        // Drawing processes
        for (int i = 0; i < procesVector.size(); i++) {
            procesVector[i]->procenatBrojaCiklusa = (procesVector[i]->brojCiklusa / lengthOfAllProcesses)*100;
        }
qDebug()<< "yo";
        QVector <QString> listOfProcesNaziva;
        for(int i = 0; i < procesVector.size(); i++ ){
            float rectWidth = (procesVector[i]->procenatBrojaCiklusa/100) * 650;
            int rectHeight = 300/brojProcesaInt;
            procesVector[i]->rectWidth = rectWidth;
            procesVector[i]->rectHeight = rectHeight;
            if(listOfProcesNaziva.indexOf(procesVector[i]->naziv) == -1){
                listOfProcesNaziva.push_back(procesVector[i]->naziv);
                procesVector[i]->rectSpacingHeight = rectHeight * (listOfProcesNaziva.indexOf(procesVector[i]->naziv)+1);
            }
            if( i > 0 ){
                procesVector[i]->rectSpacing = procesVector[i-1]->rectSpacing + procesVector[i-1]->rectWidth;
                }
            }
qDebug()<< procesVector.size();
        listOfProcesNaziva.clear();
        for(int i = 0; i < procesVector.size(); i++ ){
            QString nazivProcesa = procesVector[i]->naziv;
            for(Proces* proces: procesVector){
                if(proces->naziv == nazivProcesa){
                    proces->rectSpacingHeight = procesVector[i]->rectSpacingHeight;
                }
            }
            qDebug()<<procesVector[i]->naziv << procesVector[i]->rectSpacingHeight;
        }

qDebug()<< "yo";
        for(Proces* proces: procesVector){
            QPen pen(Qt::blue);
            QBrush brush(Qt::green);
            QGraphicsRectItem * rect = new QGraphicsRectItem(0,0,proces->rectWidth,proces->rectHeight);
            rect->setPos(50 + proces->rectSpacing , 350 - proces->rectSpacingHeight);
            rect->setPen(pen);
            rect->setBrush(brush);
    //            qDebug() << proces->rectSpacingHeight;
            scene->addItem(rect);

            QPen dashedLine = QPen(Qt::DashLine);
            dashedLine.setColor(Qt::blue);
            QGraphicsLineItem* endLine = new QGraphicsLineItem(50 + proces->rectSpacing,10, 50 + proces->rectSpacing, 370);
            endLine->setPen(dashedLine);
            scene->addItem(endLine);
        }
        qDebug()<< "yo";

        for(int i = 0; i < procesVector.size(); i++ ){
                if(listOfProcesNaziva.indexOf(procesVector[i]->naziv) == -1){
                    QFont font("Helvetica", 13);
                    QGraphicsTextItem * procesAxisLabel = new QGraphicsTextItem(procesVector[i]->naziv);
                    listOfProcesNaziva.push_back(procesVector[i]->naziv);
                    procesAxisLabel->setPos(10, 330 - ((330/(brojProcesaInt+1))*(listOfProcesNaziva.indexOf(procesVector[i]->naziv) + 1) - 20) );
                    procesAxisLabel->setFont(font);
                    procesAxisLabel->setDefaultTextColor(Qt::blue);
                    scene->addItem(procesAxisLabel);
                }

        }

// IMPORTANT DONT FORGET
procesVector.clear();
// /////////////////////////
    }else if( algoritam == "RR" ) {
        this->fillProcesVector();

        std::sort(procesVector.begin(), procesVector.end(), compareDolazak);

        printProcesVector();

        int quantum = 2;
        for(int i = 0; i < procesVector.size(); i++){
            if(procesVector[i]->brojCiklusa > quantum){
                procesVector[i]->brojCiklusa -= quantum;
                procesVector.push_back(new Proces (procesVector[i]->naziv,procesVector[i]->brojCiklusa,99));
                procesVector[i]->brojCiklusa = quantum;
            }
        }
        this->drawVectorPreemptive();
        procesVector.clear();
    } else if ( algoritam == "Prioritet" && !isPreemptive ){
        this->fillProcesVector();

        std::sort(procesVector.begin(), procesVector.end(), compareDolazak);

//        this->printProcesVector();

        bool executingProcess = false;
        int counter = 0;
        int startOfProcesExecutionTime = 0;

        QVector <Proces *> executionQueue;
        while(true){
            if(!executingProcess){
                QVector <Proces *> contendersForQueue;

                for(int i = 0; i < procesVector.size(); i++){
                    if(procesVector[i]->dolazakUCiklus <= counter){
                        contendersForQueue.push_back(procesVector[i]);
                    }
                }

                std::sort( contendersForQueue.begin(), contendersForQueue.end(),[](Proces * a, Proces * b){
                    return a->prioritet > b->prioritet;
                });



                if(contendersForQueue.size() > 0){
                    //
                    QVector<Proces*> contendersWithHighestPriority;
                    int highestPriority = 0;
                    for(Proces* proces: contendersForQueue){
                        if(proces->prioritet > highestPriority){
                            highestPriority = proces->prioritet;
                        }
                    }
                    qDebug()<<highestPriority<<"/highest Priority";
                    for(Proces* proces: contendersForQueue){
                        if(proces->prioritet == highestPriority){
                            contendersWithHighestPriority.push_back(proces);
                        }
                    }

                    printVector(contendersWithHighestPriority);
                    if(contendersWithHighestPriority.size() > 0){
                        executionQueue.push_back(contendersWithHighestPriority[0]);
                        qDebug()<<procesVector.indexOf(contendersWithHighestPriority[0]) << "index high";
                        procesVector.remove(procesVector.indexOf(contendersWithHighestPriority[0]));
                    }else {
                        executionQueue.push_back(contendersForQueue[contendersForQueue.size() - 1]);
                        qDebug()<<procesVector.indexOf(contendersForQueue[contendersForQueue.size() - 1]) << "index not high";
                        procesVector.remove(procesVector.indexOf(contendersForQueue[contendersForQueue.size() - 1]));
                    }

                    //
                    executingProcess = true;
                    startOfProcesExecutionTime = counter;
                    contendersWithHighestPriority.clear();
                    contendersForQueue.clear();
                }

            }else{
                if(counter - (startOfProcesExecutionTime + executionQueue[executionQueue.size() - 1]->brojCiklusa) == 0){
                    executingProcess = false;
                }
            }
            if(executionQueue.size() == brojProcesaInt){
                break;
            }
            counter++;
        }

//        printVector(executionQueue);
        procesVector = executionQueue;
        executionQueue.clear();

        printProcesVector();
        drawVectorNonPreemptive();
        procesVector.clear();

    }else if(algoritam == "Prioritet" && isPreemptive){
        this->fillProcesVector();
        std::sort(procesVector.begin(), procesVector.end(), compareDolazak);

        QVector <Proces *> readyQueue;
        QVector <Proces *> waitQueue;
        QVector <Proces *> executionQueue;
        bool processesAreExecuting = true;
        int executionTime = 0;
        while(processesAreExecuting){

            for(int i = 0; i < procesVector.size(); i++){
                if(executionTime == procesVector[i]->dolazakUCiklus){
                    readyQueue.push_back(procesVector[i]);
                    procesVector.remove(i);
                    i--;
                }
            }

            if(waitQueue.size() > 0){
               for(int i = 0; i < waitQueue.size(); i++){
                    if(executionTime >= waitQueue[i]->dolazakUCiklus){
                        readyQueue.push_back(waitQueue[i]);
                        waitQueue.remove(i);
                        i--;
                    }
               }
            }

            if(readyQueue.size() > 0){
                std::sort( readyQueue.begin(), readyQueue.end(),[](Proces * a, Proces * b){
                    return a->prioritet > b->prioritet;
                });

                if(executionQueue.size() == 0){
                    executionQueue.push_back(readyQueue[0]);
                    executionQueue[executionQueue.size()-1]->procesStartedExecuting = executionTime;
                    readyQueue.remove(0);

                }else if(executionQueue.size() > 0){
                    Proces * lastProcessInExecutionQueue = executionQueue[executionQueue.size()-1];
                    int remainingBrojCiklusa = lastProcessInExecutionQueue->brojCiklusa  + lastProcessInExecutionQueue->procesStartedExecuting - executionTime;
                    if(lastProcessInExecutionQueue->prioritet < readyQueue[0]->prioritet){
                                if(remainingBrojCiklusa > 0){
                                    waitQueue.push_back(new Proces(lastProcessInExecutionQueue->naziv,
                                                                   remainingBrojCiklusa,
                                                                   executionTime + 1,
                                                                   lastProcessInExecutionQueue->prioritet));
                                    lastProcessInExecutionQueue->brojCiklusa -= remainingBrojCiklusa;
                                    executionQueue.push_back(readyQueue[0]);
                                    executionQueue[executionQueue.size()-1]->procesStartedExecuting = executionTime;
                                    readyQueue.remove(0);
                                }else{
                                    executionQueue.push_back(readyQueue[0]);
                                    executionQueue[executionQueue.size()-1]->procesStartedExecuting = executionTime;
                                    readyQueue.remove(0);
                                }

                    }else if(remainingBrojCiklusa == 0){
                        executionQueue.push_back(readyQueue[0]);
                        executionQueue[executionQueue.size()-1]->procesStartedExecuting = executionTime;
                        readyQueue.remove(0);
                    }
                    qDebug() << "Remaining broj ciklusa: " << remainingBrojCiklusa;
                }


            }

            if(procesVector.size() == 0 && waitQueue.size() == 0 && readyQueue.size() == 0){
                processesAreExecuting = false;
                procesVector = executionQueue;
            }

            executionTime++;
            qDebug() << "Proces Vector size: " << procesVector.size();
            qDebug() << "Wait Vector size: " << waitQueue.size();
            qDebug() << "Ready Vector size: " << readyQueue.size();
            qDebug() << "Vrijeme izvrsavanja: " << executionTime;
        }
        printVector(executionQueue);
        int checkProcesLength = 0;
        for(Proces * proces : executionQueue){
            checkProcesLength += proces->brojCiklusa ;
        }
        qDebug() << "Original: " << lengthOfAllProcesses;
        qDebug() << "Check: " << checkProcesLength;
        executionQueue.clear();
        //Drawing
        for (int i = 0; i < procesVector.size(); i++) {
            procesVector[i]->procenatBrojaCiklusa = (procesVector[i]->brojCiklusa / lengthOfAllProcesses)*100;
        }
qDebug()<< "yo";
        QVector <QString> listOfProcesNaziva;
        for(int i = 0; i < procesVector.size(); i++ ){
            float rectWidth = (procesVector[i]->procenatBrojaCiklusa/100) * 650;
            int rectHeight = 300/brojProcesaInt;
            procesVector[i]->rectWidth = rectWidth;
            procesVector[i]->rectHeight = rectHeight;
            if(listOfProcesNaziva.indexOf(procesVector[i]->naziv) == -1){
                listOfProcesNaziva.push_back(procesVector[i]->naziv);
                procesVector[i]->rectSpacingHeight = rectHeight * (listOfProcesNaziva.indexOf(procesVector[i]->naziv)+1);
            }
            if( i > 0 ){
                procesVector[i]->rectSpacing = procesVector[i-1]->rectSpacing + procesVector[i-1]->rectWidth;
                }
            }
qDebug()<< procesVector.size();
        listOfProcesNaziva.clear();
        for(int i = 0; i < procesVector.size(); i++ ){
            QString nazivProcesa = procesVector[i]->naziv;
            for(Proces* proces: procesVector){
                if(proces->naziv == nazivProcesa){
                    proces->rectSpacingHeight = procesVector[i]->rectSpacingHeight;
                }
            }
            qDebug()<<procesVector[i]->naziv << procesVector[i]->rectSpacingHeight;
        }

qDebug()<< "yo";
        for(Proces* proces: procesVector){
            QPen pen(Qt::blue);
            QBrush brush(Qt::green);
            QGraphicsRectItem * rect = new QGraphicsRectItem(0,0,proces->rectWidth,proces->rectHeight);
            rect->setPos(50 + proces->rectSpacing , 350 - proces->rectSpacingHeight);
            rect->setPen(pen);
            rect->setBrush(brush);
    //            qDebug() << proces->rectSpacingHeight;
            scene->addItem(rect);

            QPen dashedLine = QPen(Qt::DashLine);
            dashedLine.setColor(Qt::blue);
            QGraphicsLineItem* endLine = new QGraphicsLineItem(50 + proces->rectSpacing,10, 50 + proces->rectSpacing, 370);
            endLine->setPen(dashedLine);
            scene->addItem(endLine);
        }
        qDebug()<< "yo";

        for(int i = 0; i < procesVector.size(); i++ ){
                if(listOfProcesNaziva.indexOf(procesVector[i]->naziv) == -1){
                    QFont font("Helvetica", 13);
                    QGraphicsTextItem * procesAxisLabel = new QGraphicsTextItem(procesVector[i]->naziv);
                    listOfProcesNaziva.push_back(procesVector[i]->naziv);
                    procesAxisLabel->setPos(10, 330 - ((330/(brojProcesaInt+1))*(listOfProcesNaziva.indexOf(procesVector[i]->naziv) + 1) - 20) );
                    procesAxisLabel->setFont(font);
                    procesAxisLabel->setDefaultTextColor(Qt::blue);
                    scene->addItem(procesAxisLabel);
                }

        }
        procesVector.clear();
    }



}

void Dialog :: printVector(QVector<Proces*> vector){

        qDebug()<< "______________New Print_________";

        for(int i = 0; i < vector.size(); i++){
            qDebug() << "Naziv: " << vector[i]->naziv;
            qDebug() << "Broj Ciklusa: " << vector[i]->brojCiklusa;
            qDebug() << "Dolazak u Ciklus: " << vector[i]->dolazakUCiklus;
            if( ui->algoritam->currentText() == "Prioritet"){
                qDebug() << "Prioritet: " << vector[i]->prioritet;
            }
            qDebug() << "_________________________";
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
    if( arg1 == "RR"){
        ui->radioButton->setChecked(true);
        ui->radioButton->setEnabled(false);
    } else if( arg1 != "RR"){
        ui->radioButton->setChecked(false);
        ui->radioButton->setEnabled(true);
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

void Dialog::printProcesVector()
{
    qDebug()<< "______________New Print_________";

    for(int i = 0; i < procesVector.size(); i++){
        qDebug() << "Naziv: " << procesVector[i]->naziv;
        qDebug() << "Broj Ciklusa: " << procesVector[i]->brojCiklusa;
        qDebug() << "Dolazak u Ciklus: " << procesVector[i]->dolazakUCiklus;
        if( ui->algoritam->currentText() == "Prioritet"){
            qDebug() << "Prioritet: " << procesVector[i]->prioritet;
        }
        qDebug() << "_________________________";
    }
}

void Dialog::fillProcesVector()
{
    QString brojProcesa = ui->brojProcesa->currentText();
    int brojProcesaInt = brojProcesa.toInt();
    for( int i = 0; i < brojProcesaInt ; i++ ){
        QString naziv = "P" + QString::number(i+1);
        Proces* proces = new Proces(naziv, brojCiklusaSpinBoxes[i]->value(), dolazakSpinBoxes[i]->value(), prioritetSpinBoxes[i]->value());
        procesVector.push_back(proces);
    }
}

void Dialog::drawVectorNonPreemptive()
{   QString brojProcesa = ui->brojProcesa->currentText();
    int brojProcesaInt = brojProcesa.toInt();

    float lengthOfAllProcesses = 0;
    for(int i = 0; i < brojProcesaInt; i++){
        lengthOfAllProcesses += brojCiklusaSpinBoxes[i]->value();
    }


    QFont font("Helvetica", 13);
    QPen pen(Qt::blue);
    QBrush brush(Qt::green);
    for (int i = 0; i < brojProcesaInt; i++) {
        procesVector[i]->procenatBrojaCiklusa = (procesVector[i]->brojCiklusa / lengthOfAllProcesses)*100;
    }

// DRAWING PROCESSES
    QVector <int> arrOfRectWidths;
    for(int i = 1; i <= brojProcesaInt; i++ ){
        float rectWidth = (procesVector[i-1]->procenatBrojaCiklusa/100) * 650;
        arrOfRectWidths.push_back((int)rectWidth);
        int rectSpacing = 0;
        if(i>1){
        for(int j = 0; j < i-1 ; j++){
            rectSpacing += arrOfRectWidths[j];
            }
        }

        QPen dashedLine = QPen(Qt::DashLine);
        dashedLine.setColor(Qt::blue);
        QGraphicsLineItem* endLine = new QGraphicsLineItem(50 + rectSpacing,10, 50 + rectSpacing, 370);
        endLine->setPen(dashedLine);
        scene->addItem(endLine);

        int rectHeight = 300/brojProcesa.toInt();
        QGraphicsRectItem * rect = new QGraphicsRectItem(0,0,rectWidth,rectHeight);
        rect->setPen(pen);
        rect->setBrush(brush);
        rect->setPos(50 + rectSpacing , 350 - rectHeight*(i));
        scene->addItem(rect);

        QGraphicsTextItem * procesAxisLabel = new QGraphicsTextItem(procesVector[brojProcesa.toInt() - i]->naziv);
        procesAxisLabel->setPos(10, (330/(brojProcesa.toInt()+1))*i + 20 );
        procesAxisLabel->setFont(font);
        procesAxisLabel->setDefaultTextColor(Qt::blue);
        scene->addItem(procesAxisLabel);


    }
}

void Dialog::drawVectorPreemptive()
{
    QString brojProcesa = ui->brojProcesa->currentText();
    int brojProcesaInt = brojProcesa.toInt();

    float lengthOfAllProcesses = 0;
    for(int i = 0; i < brojProcesaInt; i++){
        lengthOfAllProcesses += brojCiklusaSpinBoxes[i]->value();
    }


    for (int i = 0; i < procesVector.size(); i++) {
        procesVector[i]->procenatBrojaCiklusa = (procesVector[i]->brojCiklusa / lengthOfAllProcesses)*100;
    }

    for(int i = 0; i < procesVector.size(); i++ ){
        float rectWidth = (procesVector[i]->procenatBrojaCiklusa/100) * 650;
        int rectHeight = 300/brojProcesaInt;
        procesVector[i]->rectWidth = rectWidth;
        procesVector[i]->rectHeight = rectHeight;
        procesVector[i]->rectSpacingHeight = rectHeight * (i+1);
        if( i > 0 ){
            procesVector[i]->rectSpacing = procesVector[i-1]->rectSpacing + procesVector[i-1]->rectWidth;
        }
}
    for(int i = 0; i < brojProcesaInt; i++ ){
        QString nazivProcesa = procesVector[i]->naziv;
        for(Proces* proces: procesVector){
            if(proces->naziv == nazivProcesa){
                proces->rectSpacingHeight = procesVector[i]->rectSpacingHeight;
            }
        }
    }


    for(Proces* proces: procesVector){
        QPen pen(Qt::blue);
        QBrush brush(Qt::green);
        QGraphicsRectItem * rect = new QGraphicsRectItem(0,0,proces->rectWidth,proces->rectHeight);
        rect->setPos(50 + proces->rectSpacing , 350 - proces->rectSpacingHeight);
        rect->setPen(pen);
        rect->setBrush(brush);
//            qDebug() << proces->rectSpacingHeight;
        scene->addItem(rect);

        QPen dashedLine = QPen(Qt::DashLine);
        dashedLine.setColor(Qt::blue);
        QGraphicsLineItem* endLine = new QGraphicsLineItem(50 + proces->rectSpacing,10, 50 + proces->rectSpacing, 370);
        endLine->setPen(dashedLine);
        scene->addItem(endLine);
    }

    for(int i = 0; i < brojProcesaInt; i++ ){
            QFont font("Helvetica", 13);
            QGraphicsTextItem * procesAxisLabel = new QGraphicsTextItem(procesVector[brojProcesaInt - (i + 1)]->naziv);
            procesAxisLabel->setPos(10, (330/(brojProcesaInt+1))*(i + 1) + 20 );
            procesAxisLabel->setFont(font);
            procesAxisLabel->setDefaultTextColor(Qt::blue);
            scene->addItem(procesAxisLabel);
    }
}
