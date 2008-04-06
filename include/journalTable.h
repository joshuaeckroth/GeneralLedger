#ifndef JOURNAL_TABLE_H
#define JOURNAL_TABLE_H

#include <qtable.h>

class QStringList;
class QKeyEvent;

class Database;

class JournalTable : public QTable
{
    Q_OBJECT
    public:
        JournalTable(QWidget *parent = 0, const char *name = 0);
        ~JournalTable();
        int debitColWidth();
        int creditColWidth();
        void populate();
        
    signals:
        void goToMain();
        void goToAccounts();
        void goToReports();
        void goToHelp();
    
    private slots:
        void updateDb(int row, int);
        void updateAccounts();
        
    private:
        void insert();
        void remove(int row);
        void keyPressEvent(QKeyEvent *event);
        
        Database *db;
        QStringList accounts;
};

#endif
