#ifndef JOURNAL_TABLE_H
#define JOURNAL_TABLE_H

#include <qtable.h>

class QStringList;
class QKeyEvent;

class Database;
class Settings;

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
        void edittingChanged(int, int);
        void updateAccounts();
        
    private:
        void insert();
        void remove(int row);
        void keyPressEvent(QKeyEvent *event);
        
        Database *db;
        Settings *settings;

        QString iconPath;

        bool editting;
        int edittingRow;
        int edittingCol;

        bool inserting;
        
        QStringList accounts;
};

#endif
