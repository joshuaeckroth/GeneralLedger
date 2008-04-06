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
class IncomeSvcEditor;
class IncomeRetailEditor;
class AccountList;
class Database;
class Settings;
class Printer;

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
        void exportPDF();
        void exportCSV();
        void switchWidget();
        void generalDetailReport();
        void generalTrialReport();
        void chartAccountsReport();
        void balanceReport();
        void incomeSvcReport();
        void incomeRetailReport();
        void switchToEditBalance();
        void switchToEditIncomeSvc();
        void switchToEditIncomeRetail();
        void print();
        
    private:
        bool eventFilter(QObject *target, QEvent *event);
        
        Database *db;
        Settings *settings;
        Printer *printer;

        QString clientPath;
        QString exportPath;
        QString importPath;
        
        int active;
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
            AccountList *generalDetailAccountRangeBegin;
            QLabel *generalDetailAccountRangeLabelBetween;
            AccountList *generalDetailAccountRangeEnd;
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
            AccountList *generalTrialAccountRangeBegin;
            QLabel *generalTrialAccountRangeLabelBetween;
            AccountList *generalTrialAccountRangeEnd;
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
            
            QVGroupBox *incomeSvcVGroupBox;
            QWidget *incomeSvcMonthEndWidget;
            QHBoxLayout *incomeSvcMonthEndHBoxLayout;
            QLabel *incomeSvcMonthEndLabel;
            QComboBox *incomeSvcMonthEndList;
            QWidget *incomeSvcModifyWidget;
            QHBoxLayout *incomeSvcModifyHBoxLayout;
            QPushButton *incomeSvcModifyButton;
            QVBoxLayout *incomeSvcButtonVBoxLayout;
            QPushButton *incomeSvcButton;

            QVGroupBox *incomeRetailVGroupBox;
            QWidget *incomeRetailMonthEndWidget;
            QHBoxLayout *incomeRetailMonthEndHBoxLayout;
            QLabel *incomeRetailMonthEndLabel;
            QComboBox *incomeRetailMonthEndList;
            QWidget *incomeRetailModifyWidget;
            QHBoxLayout *incomeRetailModifyHBoxLayout;
            QPushButton *incomeRetailModifyButton;
            QVBoxLayout *incomeRetailButtonVBoxLayout;
            QPushButton *incomeRetailButton;
            
            QVBoxLayout *chartAccountsButtonVBoxLayout;
            QPushButton *chartAccountsButton;
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
            QPushButton *exportPDFButton;
            QPushButton *exportCSVButton;
        } report;
        
        struct {
            QWidget *widget;
            QVBoxLayout *vBoxLayout;
            QBoxLayout *labelLayout;
            GoBackLabel *topLabel;
            BalanceSheetEditor *editor;
        } balance;

        struct {
            QWidget *widget;
            QVBoxLayout *vBoxLayout;
            QBoxLayout *labelLayout;
            GoBackLabel *topLabel;
            IncomeSvcEditor *editor;
        } incomeSvc;

        struct {
            QWidget *widget;
            QVBoxLayout *vBoxLayout;
            QBoxLayout *labelLayout;
            GoBackLabel *topLabel;
            IncomeRetailEditor *editor;
        } incomeRetail;
        
};


#endif
