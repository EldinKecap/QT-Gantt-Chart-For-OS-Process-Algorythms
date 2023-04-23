#ifndef PROCES_H
#define PROCES_H
#include <QString>

class Proces
{
public:
    Proces();
    Proces(QString naziv,int brojCiklusa,int dolazakUCiklus);
    QString naziv;
    int brojCiklusa;
    int dolazakUCiklus;
    int prioritet;
};

#endif // PROCES_H
