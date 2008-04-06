#ifndef TABS_H
#define TABS_H

#include <qtabwidget.h>

class QApplication;
class QKeyEvent;
class QCloseEvent;

class Database;
class MainStack;
class AccountStack;
class JournalStack;
class ReportStack;
class HelpStack;

class Tabs : public QTabWidget
{
    Q_OBJECT
    public:
        Tabs(QApplication *parent, const char *name = 0);
        ~Tabs();
        
    private slots:
        void dbOpened();
        void dbClosed();
        void nameChanged();
        void switchToMain();
        void switchToAccounts();
        void switchToJournal();
        void switchToReports();
        void switchToHelp();
        void quit();
        
    private:
        void keyPressEvent(QKeyEvent *event);
        void closeEvent(QCloseEvent *event);
        
        QApplication *app;
        Database *db;
        MainStack *mainStack;
        AccountStack *accountStack;        
        JournalStack *journalStack;
        ReportStack *reportStack;
        HelpStack *helpStack;
    
};

#endif
