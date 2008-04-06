#ifndef MAIN_STACK_H
#define MAIN_STACK_H

#include <vector>
#include <qwidgetstack.h>

class QKeyEvent;
class QSqlDatabase;
class QHBoxLayout;
class QVBoxLayout;
class QVGroupBox;
class QPushButton;

class MainStack : public QWidgetStack
{
    Q_OBJECT
    public:
        MainStack( QString curClient, QWidget *parent = 0, const char *name = 0 );
        bool focusNextPrevChild(bool next);
        void dbClosed();
        
    signals:
        void openDefault();
        void openNew(QString);
        void createNew(QString);
        void prepareQuit();
        void switchToAccounts();
        void switchToJournal();
        void switchToReports();
        void closeDb();
        void copyDb(QString);
        
    private slots:
        void dbOpened();
        void openNewDialog();
        void createNewDialog();
        void editName();
        void copyClient();
         
    private:
        void switchWidget();
        void keyPressEvent(QKeyEvent *event);
        
        int active;
        
        QSqlDatabase *db;
        
        struct {
            std::vector<QWidget*> widgetVect;
            std::vector<QWidget*>::iterator widgetIter;
            QWidget *widget;
            QHBoxLayout *hBoxLayout;
            QVBoxLayout *vBoxLayout;
            QVGroupBox *mainGroup;
            QPushButton *openDefaultButton;
            QPushButton *openNewButton;
            QPushButton *createNewButton;
            QVGroupBox *otherGroup;
            QPushButton *importButton;
            QPushButton *quitButton;
        } main;
            
        struct {
            std::vector<QWidget*> widgetVect;
            std::vector<QWidget*>::iterator widgetIter;
            QWidget *widget;
            QVBoxLayout *vBoxLabel;
            QHBoxLayout *hBoxLabel;
            QPushButton *closeButton;
            QHBoxLayout *hBoxLayout;
            QVBoxLayout *vBoxLayout;
            QVGroupBox *clientGroup;
            QPushButton *accountsButton;
            QPushButton *journalButton;
            QPushButton *reportButton;
            QVGroupBox *adminGroup;
            QPushButton *editNameButton;
            QPushButton *copyDatabaseButton;
            QPushButton *cryptButton;
        } tasks;
};


#endif
