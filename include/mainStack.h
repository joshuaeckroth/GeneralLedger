#ifndef MAIN_STACK_H
#define MAIN_STACK_H

#include <qwidgetstack.h>

class QEvent;
class QHBoxLayout;
class QVBoxLayout;
class QVGroupBox;
class QPushButton;

class Database;
class Settings;

class MainStack : public QWidgetStack
{
    Q_OBJECT
    public:
        MainStack(QWidget *parent = 0, const char *name = 0);
        ~MainStack();
        bool closed() const;
        
    signals:
        void switchToAccounts();
        void switchToJournal();
        void switchToReports();
        void dbOpened();
        void dbClosed();
        void nameChanged();
        void saveAndClearJournalTmp();
        void quit();

    public slots:
        void closeClient();
        
    private slots:
        void openDefault();
        void openNewDialog();
        void createNewDialog();
        void editName();
        void copyClient();
        void changePath();
        void import();
        void encrypt();
        void decrypt();
        void prepareQuit();
         
    private:
        void clientOpened();
        void switchWidget();
        bool eventFilter(QObject *target, QEvent *event);
        
        Database *db;
        Settings *settings;

        QString clientPath;
        QString exportPath;
        QString importPath;
        
        int active;
        
        struct {
            QWidget *widget;
            QHBoxLayout *hBoxLayout;
            QVBoxLayout *vBoxLayout;
            QVGroupBox *mainGroup;
            QPushButton *openDefaultButton;
            QPushButton *openNewButton;
            QPushButton *createNewButton;
            QVGroupBox *otherGroup;
            QPushButton *pathButton;
            QPushButton *importButton;
            QPushButton *quitButton;
        } main;
            
        struct {
            QPushButton *focusedButton;
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
