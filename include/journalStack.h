#ifndef JOURNAL_STACK_H
#define JOURNAL_STACK_H

#include <qwidgetstack.h>

class QEvent;
class QVBoxLayout;
class QHBoxLayout;
class QBoxLayout;
class QVGroupBox;
class QPushButton;

class GoBackLabel;
class JournalTable;
class JournalSummary;

class JournalStack : public QWidgetStack
{
    Q_OBJECT
    public:
        JournalStack( QWidget *parent = 0, const char *name = 0 );
        
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
            QBoxLayout *labelLayout;
            GoBackLabel *topLabel;
            QPushButton *closePeriod;
            JournalTable *dataTable;
            JournalSummary *summary;
        } table;
        
};


#endif

