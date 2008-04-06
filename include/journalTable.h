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
        bool isInserting() const;
        int debitColWidth();
        int creditColWidth();
        void populate();
        void clearTable();
        
    signals:
        void goToMain();
        void goToAccounts();
        void goToReports();
        void goToHelp();
    
    private slots:
        void updateDb(int, int);
        void editingChanged(int, int);
        void updateAccounts();
        
    private:
        void insert();
        void remove(int row);
        void keyPressEvent(QKeyEvent *event);
        
        Database *db;

        bool editing;
        int editingRow;
        int editingCol;

        bool inserting;
        
        QStringList accounts;
};

#endif
