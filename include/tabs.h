#ifndef TABS_H
#define TABS_H

#include <qtabwidget.h>

class QApplication;
class QKeyEvent;

class MainStack;
class AccountStack;
class JournalStack;
class ReportStack;

class Tabs : public QTabWidget
{
    Q_OBJECT
    public:
        Tabs(QApplication *parent = 0, const char *name = 0);
        
    signals:
        void writeSettings();
        void closeDb();
        void copyDb(QString);
        
    private slots:
        void dbOpened();
        void dbClosed();
        void switchToMain();
        void switchToAccounts();
        void switchToJournal();
        void switchToReports();
        
    private:
        void keyPressEvent(QKeyEvent *event);
        void closeEvent(QCloseEvent *event);
        
        MainStack *mainStack;
        AccountStack *accountStack;        
        JournalStack *journalStack;
        ReportStack *reportStack;
    
};

#endif
