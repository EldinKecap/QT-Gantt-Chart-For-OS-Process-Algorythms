#include "proces.h"

Proces::Proces(){

   }

Proces::Proces(QString naziv, int brojCiklusa, int dolazakUCiklus)
{
    this->naziv = naziv;
    this->brojCiklusa = brojCiklusa;
    this->dolazakUCiklus = dolazakUCiklus;
    this->rectSpacing = 0;
}
