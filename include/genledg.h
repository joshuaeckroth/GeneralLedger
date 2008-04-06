#ifndef GENLEDG_H
#define GENLEDG_H

#include <qapplication.h>

class GeneralLedger : public QApplication
{
    Q_OBJECT
    public:
        GeneralLedger(int argc, char *argv[]);
};

#endif
