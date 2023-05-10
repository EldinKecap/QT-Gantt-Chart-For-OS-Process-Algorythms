#ifndef PROCES_H
#define PROCES_H
#include <QString>

class Proces
{
public:
    Proces();
    Proces(QString naziv,int brojCiklusa,int dolazakUCiklus);
    Proces(QString naziv,int brojCiklusa,int dolazakUCiklus, int prioritet);
    QString naziv;

    float brojCiklusa;
    float procenatBrojaCiklusa;
    int dolazakUCiklus;
    int prioritet;

    float rectWidth;
    int rectHeight;
    int rectSpacing;
    int rectSpacingHeight;
};

#endif // PROCES_H
