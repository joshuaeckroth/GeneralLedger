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

class BalanceSheetEditor;
class Database;

class ReportStack : public QWidgetStack
{
    Q_OBJECT
    public:
        ReportStack( QWidget *parent = 0, const char *name = 0 );
        ~ReportStack();
        
    signals:
        void goBack();
        
    private slots:
        void rangeToggled();
        void allToggled();
        void dbOpened();
        void viewReport(QString &text);
        void exportHTML();
        void exportCSV();
        void switchWidget();
        void switchToEditBalance();
        void generalReport();
        void chartAccountsReport();
        void balanceReport();
        void printerOptions();
        
    private:
        void switchToGeneralReport();
        bool eventFilter(QObject *target, QEvent *event);
        
        int active;
        Database *db;
        
        struct {
            QWidget *widget;
            QVBoxLayout *vBoxLabel;
            QHBoxLayout *hBoxLabel;
            GoBackLabel *topLabel;
            QHBoxLayout *hBoxLayout;
            QVBoxLayout *vBoxLayout;
            QVGroupBox *generalGroup;
            QPushButton *generalButton;
            QPushButton *chartAccountsButton;
            QVGroupBox *balanceGroup;
            QPushButton *balanceModifyButton;
            QPushButton *balanceReportButton;
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
            QWidget *bottomWidget;
            QHBoxLayout *bottomHBoxLayout;
            QLabel *bottomLabel;
            QFrame *bottomRightFrame;
            QBoxLayout *bottomRightBoxLayout;
            QPushButton *exportHTMLButton;
            QPushButton *exportCSVButton;
        } report;
        
        struct {
            QWidget *widget;
            QVBoxLayout *vBoxLayout;
            QBoxLayout *labelLayout;
            GoBackLabel *topLabel;
            BalanceSheetEditor *editor;
        } balance;
        
};


#endif
