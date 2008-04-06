#ifndef REPORT_STACK_H
#define REPORT_STACK_H

#include <qwidgetstack.h>

#include "goBackLabel.h"

class QEvent;
class QVBoxLayout;
class QHBoxLayout;
class GoBackLabel;
class QVGroupBox;
class QVBox;
class QFrame;
class QComboBox;
class QPushButton;
class QLabel;
class QLineEdit;
class QRadioButton;
class QBoxLayout;
class QTextEdit;

class ReportStack : public QWidgetStack
{
    Q_OBJECT
    public:
        ReportStack( QWidget *parent = 0, const char *name = 0 );
        
    signals:
        void goBack();
        
    private slots:
        void rangeToggled();
        void allToggled();
        void dbOpened();
        void viewReport();
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
            QVGroupBox *generalGroup;
            QLabel *monthRangeLabel;
            QVBox *monthRangeVBox;
            QFrame *monthRangeFrame;
            QHBoxLayout *monthRangeLayout;
            QComboBox *monthRangeFirst;
            QLabel *monthRangeMiddleLabel;
            QComboBox *monthRangeLast;
            QLabel *accountRangeLabel;
            QVBox *accountRangeVBox;
            QFrame *accountRangeFrame;
            QHBoxLayout *accountRangeLayout;
            QRadioButton *accountRangeRadio;
            QLineEdit *accountRangeFirst;
            QLabel *accountRangeMiddleLabel;
            QLineEdit *accountRangeLast;
            QFrame *accountRangeAllFrame;
            QHBoxLayout *accountRangeAllLayout;
            QRadioButton *accountRangeAllRadio;
            QLabel *accountRangeAllLabel;
            QPushButton *generalDetailButton;
            QPushButton *generalTrialButton;
            QVGroupBox *chartGroup;
            QPushButton *chartAccountsButton;
            QVGroupBox *adminGroup;
            QPushButton *printerOptionsButton;
        } main;
        
        struct {
            QWidget *widget;
            QVBoxLayout *vBoxLayout;
            QBoxLayout *labelLayout;
            GoBackLabel *topLabel;
            QPushButton *print;
            QTextEdit *view;
        } report;
        
};


#endif
