#ifndef GENERAL_LEDGER_H
#define GENERAL_LEDGER_H

#include <qapplication.h>
#include <qsqldatabase.h>
#include <qtabwidget.h>

#include "mainStack.h"
#include "accountStack.h"
#include "journalStack.h"
#include "reportStack.h"

class GeneralLedger : public QApplication
{
    Q_OBJECT
    public:
        GeneralLedger(int argc, char *argv[]);
        
    signals:
        void dbOpened();
        
    private slots:
        void openDefault();
        void openNew(QString name);
        void createNew(QString name);
        
    private:
        QSqlDatabase *db;
        QTabWidget *tabs;
        MainStack *mainStack;
        AccountStack *accountStack;
        JournalStack *journalStack;
        ReportStack *reportStack;
};

#endif