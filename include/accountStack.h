#ifndef ACCOUNT_STACK_H
#define ACCOUNT_STACK_H

#include <qwidget.h>

class QEvent;
class QVBoxLayout;
class QHBoxLayout;
class QVGroupBox;
class QPushButton;
class QLabel;
class QFrame;
class QBoxLayout;

class GoBackLabel;
class AccountTable;
class Database;

class AccountStack : public QWidget
{
    Q_OBJECT
    public:
        AccountStack( QWidget *parent = 0, const char *name = 0 );
        ~AccountStack();
        
    signals:
        void goToMain();
        void goToJournal();
        void goToReports();
        void goToHelp();
        
    private slots:
        void dbOpened();
        void importCSV();
        void exportCSV();
        
    private:
        bool eventFilter(QObject *target, QEvent *event);
        void showEvent(QShowEvent*);
        
        bool active;
        Database *db;
        
        struct {
            QVBoxLayout *vBoxLayout;
            QHBoxLayout *labelLayout;
            GoBackLabel *topLabel;
            AccountTable *dataTable;
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
