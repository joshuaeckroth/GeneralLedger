#ifndef REPORT_STACK_H
#define REPORT_STACK_H

#include <qwidgetstack.h>

#include "goBackLabel.h"

class QEvent;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class GoBackLabel;
class QVGroupBox;
class QVBox;
class QFrame;
class QComboBox;
class QCheckBox;
class QPushButton;
class QLabel;
class QLineEdit;
class QRadioButton;
class QBoxLayout;
class QTextEdit;
class QStringList;

class BalanceSheetEditor;
class Database;
class AccountEditList;

class ReportStack : public QWidgetStack
{
    Q_OBJECT
    public:
        ReportStack( QWidget *parent = 0, const char *name = 0 );
        ~ReportStack();
        
    signals:
        void goToMain();
        
    private slots:
        void detailAccountsAllChanged(bool);
        void trialAccountsAllChanged(bool);
        void dbOpened();
        void updateAccounts();
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
        QStringList accounts;
        
        struct {
            QWidget *widget;
            QVBoxLayout *vBoxLabel;
            QHBoxLayout *hBoxLabel;
            GoBackLabel *topLabel;
            QHBoxLayout *hBoxLayout;
            QVBoxLayout *vBoxLayout;
            
            QVGroupBox *reportsGroup;
            QWidget *reportsWidget;
            QGridLayout *reportsGridLayout;
            
            QVGroupBox *generalDetailVGroupBox;
            QWidget *generalDetailDateRangeWidget;
            QHBoxLayout *generalDetailDateRangeHBoxLayout;
            QLabel *generalDetailDateRangeLabelBegin;
            QComboBox *generalDetailDateRangeBegin;
            QLabel *generalDetailDateRangeLabelBetween;
            QComboBox *generalDetailDateRangeEnd;
            QWidget *generalDetailAccountRangeWidget;
            QHBoxLayout *generalDetailAccountRangeHBoxLayout;
            QLabel *generalDetailAccountRangeLabel;
            AccountEditList *generalDetailAccountRangeBegin;
            QLabel *generalDetailAccountRangeLabelBetween;
            AccountEditList *generalDetailAccountRangeEnd;
            QCheckBox *generalDetailAccountRangeAll;
            QVBoxLayout *generalDetailButtonVBoxLayout;
            QPushButton *generalDetailButton;
            
            QVGroupBox *generalTrialVGroupBox;
            QWidget *generalTrialMonthEndWidget;
            QHBoxLayout *generalTrialMonthEndHBoxLayout;
            QLabel *generalTrialMonthEndLabel;
            QComboBox *generalTrialMonthEndList;
            QWidget *generalTrialAccountRangeWidget;
            QHBoxLayout *generalTrialAccountRangeHBoxLayout;
            QLabel *generalTrialAccountRangeLabel;
            AccountEditList *generalTrialAccountRangeBegin;
            QLabel *generalTrialAccountRangeLabelBetween;
            AccountEditList *generalTrialAccountRangeEnd;
            QCheckBox *generalTrialAccountRangeAll;
            QVBoxLayout *generalTrialButtonVBoxLayout;
            QPushButton *generalTrialButton;
            
            QVGroupBox *balanceVGroupBox;
            QWidget *balanceMonthEndWidget;
            QHBoxLayout *balanceMonthEndHBoxLayout;
            QLabel *balanceMonthEndLabel;
            QComboBox *balanceMonthEndList;
            QWidget *balanceModifyWidget;
            QHBoxLayout *balanceModifyHBoxLayout;
            QPushButton *balanceModifyButton;
            QVBoxLayout *balanceReportButtonVBoxLayout;
            QPushButton *balanceReportButton;
            
            QVGroupBox *incomeVGroupBox;
            QWidget *incomeMonthEndWidget;
            QHBoxLayout *incomeMonthEndHBoxLayout;
            QLabel *incomeMonthEndLabel;
            QComboBox *incomeMonthEndList;
            QWidget *incomeModifyWidget;
            QHBoxLayout *incomeModifyHBoxLayout;
            QPushButton *incomeModifyButton;
            QVBoxLayout *incomeButtonVBoxLayout;
            QPushButton *incomeButton;
            
            QVBoxLayout *chartAccountsButtonVBoxLayout;
            QPushButton *chartAccountsButton;
            
            QVGroupBox *adminGroup;
            QWidget *adminWidget;
            QHBoxLayout *adminHBoxLayout;
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
