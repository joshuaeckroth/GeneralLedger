#ifndef JOURNAL_STACK_H
#define JOURNAL_STACK_H

#include <qwidget.h>

class QEvent;
class QVBoxLayout;
class QHBoxLayout;
class QBoxLayout;
class QVGroupBox;
class QPushButton;
class QFrame;
class QLabel;

class GoBackLabel;
class JournalTable;
class JournalSummary;
class Database;
class Settings;
class Printer;

class JournalStack : public QWidget
{
    Q_OBJECT
    public:
        JournalStack( QWidget *parent = 0, const char *name = 0 );
        ~JournalStack();
        
    signals:
        void goToMain();
        void goToAccounts();
        void goToReports();
        void goToHelp();

    public slots:
        void saveAndClear();
        
    private slots:
        void dbOpened();
        void importCSV();
        void exportCSV();
        void printJournal();
        
    private:
        bool eventFilter(QObject *target, QEvent *event);
        void showEvent(QShowEvent*);
        
        Database *db;
        Settings *settings;
        Printer *printer;

        QString exportPath;
        QString importPath;
        
        bool active;

        bool printed;
        
        struct {
            QVBoxLayout *vBoxLayout;
            QBoxLayout *labelLayout;
            GoBackLabel *topLabel;
            QPushButton *saveAndClear;
            QPushButton *print;
            JournalTable *dataTable;
            JournalSummary *summary;
            QLabel *hr;
            QWidget *bottomWidget;
            QHBoxLayout *bottomHBoxLayout;
            QLabel *bottomLabel;
            QFrame *bottomRightFrame;
            QBoxLayout *bottomRightBoxLayout;
            QPushButton *importButton;
            QPushButton *exportButton;
        } main;
        
};


#endif

