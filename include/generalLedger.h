#ifndef GENERAL_LEDGER_H
#define GENERAL_LEDGER_H

#include <qapplication.h>
#include <qstring.h>

class QSqlDatabase;
class Tabs;

class GeneralLedger : public QApplication
{
    Q_OBJECT
    public:
        GeneralLedger(int argc, char *argv[]);
        QString* getCurClient();
        QString* getCurDb();
        
    signals:
        void dbOpened();
        void dbClosed();
        
    public slots:
        void openDefault();
        void openNew(QString name);
        void createNew(QString name);
        void writeSettings();
        void closeDb();
        void copyDb(QString file);
        void prepareQuit();
        
    private:
        void readDefaults();
        
        QString *curDb;
        QString *curClient;
        QSqlDatabase *db;
        Tabs *tabs;
};

#endif
