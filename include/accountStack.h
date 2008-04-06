#ifndef ACCOUNT_STACK_H
#define ACCOUNT_STACK_H

#include <vector>
#include <qwidgetstack.h>

class QEvent;
class QVBoxLayout;
class QHBoxLayout;
class QVGroupBox;
class QPushButton;

class GoBackLabel;
class AccountTable;

class AccountStack : public QWidgetStack
{
    Q_OBJECT
    public:
        AccountStack( QWidget *parent = 0, const char *name = 0 );
        bool focusNextPrevChild(bool next);
        
    signals:
        void goBack();
        
    private slots:
        void dbOpened();
        void importCSV();
        void exportCSV();
        void switchWidget();
        
    private:
        bool eventFilter(QObject *target, QEvent *event);
        
        int active;
        
        struct {
            std::vector<QWidget*> widgetVect;
            std::vector<QWidget*>::iterator widgetIter;
            QWidget *widget;
            QVBoxLayout *vBoxLabel;
            QHBoxLayout *hBoxLabel;
            GoBackLabel *topLabel;
            QHBoxLayout *hBoxLayout;
            QVBoxLayout *vBoxLayout;
            QVGroupBox *taskGroup;
            QPushButton *editButton;
            QVGroupBox *adminGroup;
            QPushButton *importButton;
            QPushButton *exportButton;
        } main;
        
        struct {
            QWidget *widget;
            QVBoxLayout *vBoxLayout;
            QHBoxLayout *labelLayout;
            GoBackLabel *topLabel;
            AccountTable *dataTable;
        } table;
        
};


#endif
