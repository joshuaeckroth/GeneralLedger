#include <qwidgetstack.h>
#include <qvgroupbox.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qframe.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qtextedit.h>
#include <qevent.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qfiledialog.h>
#include <qstringlist.h>

#include "reportStack.h"
#include "goBackLabel.h"
#include "balanceSheetEditor.h"
#include "database.h"
#include "accountEditList.h"

ReportStack::ReportStack(QWidget *parent, const char *name)
    : QWidgetStack(parent,name)
{
    db = new Database();
    connect(db, SIGNAL(accountsChanged()), this, SLOT(updateAccounts()));
    accounts = QStringList();

    QStringList periodTmp("Period X (00/00/00)");  // placeholder for combobox sizes
    
    main.widget = new QWidget(this);
    
    main.vBoxLabel = new QVBoxLayout(main.widget);
    
    main.hBoxLabel = new QHBoxLayout(main.vBoxLabel);
    main.hBoxLabel->setMargin(5);
    
    main.topLabel = new GoBackLabel(main.widget);
    connect(main.topLabel, SIGNAL(goBack()), this, SIGNAL(goToMain()));
    
    main.hBoxLabel->addWidget(main.topLabel);
    main.hBoxLabel->addStretch();
    
    main.hBoxLayout = new QHBoxLayout(main.vBoxLabel);
    
    main.hBoxLayout->addStretch();
    
    main.vBoxLayout = new QVBoxLayout(main.hBoxLayout);
    main.vBoxLayout->setSpacing(30);
    
    main.vBoxLayout->addStretch();
    
    
    main.reportsGroup = new QVGroupBox("Reports", main.widget);
    main.reportsGroup->setInsideMargin(30);
    main.reportsGroup->setInsideSpacing(20);
    
    main.vBoxLayout->add(main.reportsGroup);
    
    main.reportsWidget = new QWidget(main.reportsGroup);
    
    main.reportsGridLayout = new QGridLayout(main.reportsWidget, 5, 2, 0, 15);  // rows, cols, margin, spacing
    
    
    main.generalDetailVGroupBox = new QVGroupBox(main.reportsWidget);
    main.generalDetailVGroupBox->setInsideSpacing(5);
    
    main.generalDetailDateRangeWidget = new QWidget(main.generalDetailVGroupBox);
    main.generalDetailDateRangeHBoxLayout = new QHBoxLayout(main.generalDetailDateRangeWidget);
    main.generalDetailDateRangeHBoxLayout->setSpacing(5);
    
    main.generalDetailDateRangeLabelBegin = new QLabel("Date Range:", main.generalDetailDateRangeWidget);
    main.generalDetailDateRangeBegin = new QComboBox(main.generalDetailDateRangeWidget);
    main.generalDetailDateRangeBegin->insertStringList(periodTmp);
    main.generalDetailDateRangeLabelBetween = new QLabel("to", main.generalDetailDateRangeWidget);
    main.generalDetailDateRangeEnd = new QComboBox(main.generalDetailDateRangeWidget);
    main.generalDetailDateRangeEnd->insertStringList(periodTmp);
    
    main.generalDetailDateRangeHBoxLayout->addWidget(main.generalDetailDateRangeLabelBegin);
    main.generalDetailDateRangeHBoxLayout->addWidget(main.generalDetailDateRangeBegin);
    main.generalDetailDateRangeHBoxLayout->addWidget(main.generalDetailDateRangeLabelBetween);
    main.generalDetailDateRangeHBoxLayout->addWidget(main.generalDetailDateRangeEnd);
    main.generalDetailDateRangeHBoxLayout->addStretch();
    
    main.generalDetailAccountRangeWidget = new QWidget(main.generalDetailVGroupBox);
    main.generalDetailAccountRangeHBoxLayout = new QHBoxLayout(main.generalDetailAccountRangeWidget);
    main.generalDetailAccountRangeHBoxLayout->setSpacing(5);
    
    main.generalDetailAccountRangeLabel = new QLabel("Account Range:", main.generalDetailAccountRangeWidget);
    main.generalDetailAccountRangeAll = new QCheckBox("All", main.generalDetailAccountRangeWidget);
    main.generalDetailAccountRangeAll->setChecked(true);
    connect(main.generalDetailAccountRangeAll, SIGNAL(toggled(bool)), this, SLOT(detailAccountsAllChanged(bool)));
    main.generalDetailAccountRangeBegin = new AccountEditList(main.generalDetailAccountRangeWidget, 0, accounts, "");
    main.generalDetailAccountRangeBegin->setEnabled(false);
    main.generalDetailAccountRangeLabelBetween = new QLabel("to", main.generalDetailAccountRangeWidget);
    main.generalDetailAccountRangeLabelBetween->setEnabled(false);
    main.generalDetailAccountRangeEnd = new AccountEditList(main.generalDetailAccountRangeWidget, 0, accounts, "");
    main.generalDetailAccountRangeEnd->setEnabled(false);

    main.generalDetailAccountRangeHBoxLayout->addWidget(main.generalDetailAccountRangeLabel);    
    main.generalDetailAccountRangeHBoxLayout->addWidget(main.generalDetailAccountRangeAll);
    main.generalDetailAccountRangeHBoxLayout->addWidget(main.generalDetailAccountRangeBegin);
    main.generalDetailAccountRangeHBoxLayout->addWidget(main.generalDetailAccountRangeLabelBetween);
    main.generalDetailAccountRangeHBoxLayout->addWidget(main.generalDetailAccountRangeEnd);
    main.generalDetailAccountRangeHBoxLayout->addStretch();
    
    main.generalDetailButtonVBoxLayout = new QVBoxLayout();
    main.generalDetailButton = new QPushButton(QIconSet(QPixmap::fromMimeSource("icons/generalDetailReport.png")),
                                               "General Ledger Detail", main.reportsWidget);
    main.generalDetailButton->installEventFilter(this);
    main.generalDetailButtonVBoxLayout->addWidget(main.generalDetailButton);
    
    main.reportsGridLayout->addWidget(main.generalDetailVGroupBox, 0, 0);
    main.reportsGridLayout->addLayout(main.generalDetailButtonVBoxLayout, 0, 1);
    
    
    main.generalTrialVGroupBox = new QVGroupBox(main.reportsWidget);
    main.generalTrialVGroupBox->setInsideSpacing(5);
    
    main.generalTrialMonthEndWidget = new QWidget(main.generalTrialVGroupBox);
    main.generalTrialMonthEndHBoxLayout = new QHBoxLayout(main.generalTrialMonthEndWidget);
    main.generalTrialMonthEndHBoxLayout->setSpacing(5);
    
    main.generalTrialMonthEndLabel = new QLabel("Month-End:", main.generalTrialMonthEndWidget);
    main.generalTrialMonthEndList = new QComboBox(main.generalTrialMonthEndWidget);
    main.generalTrialMonthEndList->insertStringList(periodTmp);
    
    main.generalTrialMonthEndHBoxLayout->addWidget(main.generalTrialMonthEndLabel);
    main.generalTrialMonthEndHBoxLayout->addWidget(main.generalTrialMonthEndList);
    main.generalTrialMonthEndHBoxLayout->addStretch();
    
    main.generalTrialAccountRangeWidget = new QWidget(main.generalTrialVGroupBox);
    main.generalTrialAccountRangeHBoxLayout = new QHBoxLayout(main.generalTrialAccountRangeWidget);
    main.generalTrialAccountRangeHBoxLayout->setSpacing(5);
    
    main.generalTrialAccountRangeLabel = new QLabel("Account Range:", main.generalTrialAccountRangeWidget);
    main.generalTrialAccountRangeAll = new QCheckBox("All", main.generalTrialAccountRangeWidget);
    main.generalTrialAccountRangeAll->setChecked(true);
    connect(main.generalTrialAccountRangeAll, SIGNAL(toggled(bool)), this, SLOT(trialAccountsAllChanged(bool)));
    main.generalTrialAccountRangeBegin = new AccountEditList(main.generalTrialAccountRangeWidget, 0, accounts, "");
    main.generalTrialAccountRangeBegin->setEnabled(false);
    main.generalTrialAccountRangeLabelBetween = new QLabel("to", main.generalTrialAccountRangeWidget);
    main.generalTrialAccountRangeLabelBetween->setEnabled(false);
    main.generalTrialAccountRangeEnd = new AccountEditList(main.generalTrialAccountRangeWidget, 0, accounts, "");
    main.generalTrialAccountRangeEnd->setEnabled(false);
    
    main.generalTrialAccountRangeHBoxLayout->addWidget(main.generalTrialAccountRangeLabel);
    main.generalTrialAccountRangeHBoxLayout->addWidget(main.generalTrialAccountRangeAll);
    main.generalTrialAccountRangeHBoxLayout->addWidget(main.generalTrialAccountRangeBegin);
    main.generalTrialAccountRangeHBoxLayout->addWidget(main.generalTrialAccountRangeLabelBetween);
    main.generalTrialAccountRangeHBoxLayout->addWidget(main.generalTrialAccountRangeEnd);
    main.generalTrialAccountRangeHBoxLayout->addStretch();
    
    main.generalTrialButtonVBoxLayout = new QVBoxLayout();
    main.generalTrialButton = new QPushButton(QIconSet(QPixmap::fromMimeSource("icons/generalTrialReport.png")),
                                              "General Trial Balance", main.reportsWidget);
    main.generalTrialButton->installEventFilter(this);
    main.generalTrialButtonVBoxLayout->addWidget(main.generalTrialButton);
    
    main.reportsGridLayout->addWidget(main.generalTrialVGroupBox, 1, 0);
    main.reportsGridLayout->addLayout(main.generalTrialButtonVBoxLayout, 1, 1);
    
    
    main.balanceVGroupBox = new QVGroupBox(main.reportsWidget);
    main.balanceVGroupBox->setInsideSpacing(5);
    
    main.balanceMonthEndWidget = new QWidget(main.balanceVGroupBox);
    main.balanceMonthEndHBoxLayout = new QHBoxLayout(main.balanceMonthEndWidget);
    main.balanceMonthEndHBoxLayout->setSpacing(5);
    
    main.balanceMonthEndLabel = new QLabel("Month-End:", main.balanceMonthEndWidget);
    main.balanceMonthEndList = new QComboBox(main.balanceMonthEndWidget);
    main.balanceMonthEndList->insertStringList(periodTmp);
    
    main.balanceMonthEndHBoxLayout->addWidget(main.balanceMonthEndLabel);
    main.balanceMonthEndHBoxLayout->addWidget(main.balanceMonthEndList);
    main.balanceMonthEndHBoxLayout->addStretch();
    
    main.balanceModifyWidget = new QWidget(main.balanceVGroupBox);
    main.balanceModifyHBoxLayout = new QHBoxLayout(main.balanceModifyWidget);
    
    main.balanceModifyButton = new QPushButton(QIconSet( QPixmap::fromMimeSource("icons/balanceModify.png") ),
                                               "Modify Balance Sheet", main.balanceModifyWidget);
    connect(main.balanceModifyButton, SIGNAL(clicked()), this, SLOT(switchToEditBalance()));
    main.balanceModifyButton->installEventFilter(this);
    
    main.balanceModifyHBoxLayout->addWidget(main.balanceModifyButton);
    main.balanceModifyHBoxLayout->addStretch();
    
    main.balanceReportButtonVBoxLayout = new QVBoxLayout();
    main.balanceReportButton = new QPushButton(QIconSet( QPixmap::fromMimeSource("icons/balanceReport.png") ),
                                               "Balance Sheet Report", main.reportsWidget);
    main.balanceReportButton->installEventFilter(this);
    main.balanceReportButtonVBoxLayout->addWidget(main.balanceReportButton);
        
    
    main.reportsGridLayout->addWidget(main.balanceVGroupBox, 2, 0);
    main.reportsGridLayout->addLayout(main.balanceReportButtonVBoxLayout, 2, 1);
    
    
    main.incomeVGroupBox = new QVGroupBox(main.reportsWidget);
    main.incomeVGroupBox->setInsideSpacing(5);
    
    main.incomeMonthEndWidget = new QWidget(main.incomeVGroupBox);
    main.incomeMonthEndHBoxLayout = new QHBoxLayout(main.incomeMonthEndWidget);
    main.incomeMonthEndHBoxLayout->setSpacing(5);
    
    main.incomeMonthEndLabel = new QLabel("Month-End:", main.incomeMonthEndWidget);
    main.incomeMonthEndList = new QComboBox(main.incomeMonthEndWidget);
    main.incomeMonthEndList->insertStringList(periodTmp);
    
    main.incomeMonthEndHBoxLayout->addWidget(main.incomeMonthEndLabel);
    main.incomeMonthEndHBoxLayout->addWidget(main.incomeMonthEndList);
    main.incomeMonthEndHBoxLayout->addStretch();
    
    main.incomeModifyWidget = new QWidget(main.incomeVGroupBox);
    main.incomeModifyHBoxLayout = new QHBoxLayout(main.incomeModifyWidget);
    
    main.incomeModifyButton = new QPushButton(QIconSet( QPixmap::fromMimeSource("icons/incomeModify.png") ),
                                               "Modify Income Statement", main.incomeModifyWidget);
    connect(main.incomeModifyButton, SIGNAL(clicked()), this, SLOT(switchToEditIncome()));
    main.incomeModifyButton->installEventFilter(this);
    
    main.incomeModifyHBoxLayout->addWidget(main.incomeModifyButton);
    main.incomeModifyHBoxLayout->addStretch();
    
    main.incomeButtonVBoxLayout = new QVBoxLayout();
    main.incomeButton = new QPushButton(QIconSet( QPixmap::fromMimeSource("icons/incomeReport.png") ),
                                        "Income Statement", main.reportsWidget);
    main.incomeButton->installEventFilter(this);
    main.incomeButtonVBoxLayout->addWidget(main.incomeButton);
    
    
    main.reportsGridLayout->addWidget(main.incomeVGroupBox, 3, 0);
    main.reportsGridLayout->addLayout(main.incomeButtonVBoxLayout, 3, 1);
    
    
    main.chartAccountsButtonVBoxLayout = new QVBoxLayout();
    main.chartAccountsButton = new QPushButton(QIconSet( QPixmap::fromMimeSource("icons/chartOfAccounts.png") ),
                                               "Chart of Accounts", main.reportsWidget);
    connect(main.chartAccountsButton, SIGNAL(clicked()), this, SLOT(chartAccountsReport()));
    main.chartAccountsButton->installEventFilter(this);
    main.chartAccountsButtonVBoxLayout->addWidget(main.chartAccountsButton);
    
    
    main.reportsGridLayout->addLayout(main.chartAccountsButtonVBoxLayout, 4, 1);
    
    
    
    main.adminGroup = new QVGroupBox("Administrative Options", main.widget);
    main.adminGroup->setInsideMargin(30);
    
    main.vBoxLayout->add(main.adminGroup);
    
    main.adminWidget = new QWidget(main.adminGroup);
    
    main.adminHBoxLayout = new QHBoxLayout(main.adminWidget);
    
    main.printerOptionsButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/printerOptionsButton.png") ),
            "Printer Options", main.adminWidget);
    connect(main.printerOptionsButton, SIGNAL(clicked()), this, SLOT(printerOptions()));
    main.printerOptionsButton->installEventFilter(this);
    
    main.adminHBoxLayout->addStretch(1);
    main.adminHBoxLayout->addWidget(main.printerOptionsButton);
    main.adminHBoxLayout->setStretchFactor(main.printerOptionsButton, 2);
    main.adminHBoxLayout->addStretch(1);
    
    main.vBoxLayout->addStretch();
    
    main.hBoxLayout->addStretch();
    
    
    addWidget(main.widget, 0);
    
    active = 0;
    
    
    report.widget = new QWidget(this);
    report.vBoxLayout = new QVBoxLayout(report.widget);
    report.vBoxLayout->setMargin(5);
    report.vBoxLayout->setSpacing(5);
    
    report.labelLayout = new QBoxLayout(report.vBoxLayout, QBoxLayout::LeftToRight);
    report.topLabel = new GoBackLabel(report.widget);
    connect(report.topLabel, SIGNAL(goBack()), this, SLOT(switchWidget()));
    
    report.print = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/print.png") ),
    "Print Report (F8)", report.widget);
    report.print->setFocusPolicy(QWidget::NoFocus);
    
    report.labelLayout->addWidget(report.topLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    report.labelLayout->addStretch();
    report.labelLayout->addWidget(report.print);
    
    report.view = new QTextEdit(report.widget);
    report.view->setTextFormat(QTextEdit::RichText);
    report.view->setReadOnly(true);
    report.view->setVScrollBarMode(QScrollView::AlwaysOn);
    
    report.vBoxLayout->addWidget(report.view);
    
    report.bottomWidget = new QWidget(report.widget);
    report.bottomHBoxLayout = new QHBoxLayout(report.bottomWidget);
    
    report.bottomLabel = new QLabel(
            "<nobr><b>Keyboard Control:</b></nobr><br>"
            "<nobr> &nbsp; &nbsp; <b>Up</b> / <b>Down</b>: Scroll the page</nobr><br>",
            report.bottomWidget);
    
    report.bottomRightFrame = new QFrame(report.bottomWidget);
    report.bottomRightBoxLayout = new QBoxLayout(report.bottomRightFrame, QBoxLayout::LeftToRight);
    report.bottomRightBoxLayout->setSpacing(5);
    
    report.exportHTMLButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/export.png") ),
            "Export to HTML (F11)", report.bottomRightFrame);
    connect(report.exportHTMLButton, SIGNAL(clicked()), this, SLOT(exportHTML()));
    
    report.exportCSVButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/export.png") ),
            "Export to CSV (F12)", report.bottomRightFrame);
    connect(report.exportCSVButton, SIGNAL(clicked()), this, SLOT(exportCSV()));
    
    report.bottomRightBoxLayout->addWidget(report.exportHTMLButton, 0, Qt::AlignLeft | Qt::AlignTop);
    report.bottomRightBoxLayout->addWidget(report.exportCSVButton, 0, Qt::AlignLeft | Qt::AlignTop);
    
    report.bottomHBoxLayout->addWidget(report.bottomLabel);
    report.bottomHBoxLayout->addStretch();
    report.bottomHBoxLayout->addWidget(report.bottomRightFrame);
    
    report.vBoxLayout->addWidget(report.bottomWidget);
    
    addWidget(report.widget, 1);
    
}

ReportStack::~ReportStack()
{
    delete db;
}

void ReportStack::detailAccountsAllChanged(bool on)
{
    if(on)
    {
        main.generalDetailAccountRangeBegin->setEnabled(false);
        main.generalDetailAccountRangeLabelBetween->setEnabled(false);
        main.generalDetailAccountRangeEnd->setEnabled(false);
    }
    else
    {
        main.generalDetailAccountRangeBegin->setEnabled(true);
        main.generalDetailAccountRangeLabelBetween->setEnabled(true);
        main.generalDetailAccountRangeEnd->setEnabled(true);
    }
} 
        
void ReportStack::trialAccountsAllChanged(bool on)
{
    if(on)
    {
        main.generalTrialAccountRangeBegin->setEnabled(false);
        main.generalTrialAccountRangeLabelBetween->setEnabled(false);
        main.generalTrialAccountRangeEnd->setEnabled(false);
    }
    else
    {
        main.generalTrialAccountRangeBegin->setEnabled(true);
        main.generalTrialAccountRangeLabelBetween->setEnabled(true);
        main.generalTrialAccountRangeEnd->setEnabled(true);
    }
}

void ReportStack::dbOpened()
{
    updateAccounts();
    
    QStringList periodBegin = db->getPeriodBegin();
    QStringList periodEnd = db->getPeriodEnd();
    main.generalDetailDateRangeBegin->clear();
    main.generalDetailDateRangeBegin->insertStringList(periodBegin);
    main.generalDetailDateRangeEnd->clear();
    main.generalDetailDateRangeEnd->insertStringList(periodEnd);
    main.generalTrialMonthEndList->clear();
    main.generalTrialMonthEndList->insertStringList(periodEnd);
    main.balanceMonthEndList->clear();
    main.balanceMonthEndList->insertStringList(periodEnd);
    main.incomeMonthEndList->clear();
    main.incomeMonthEndList->insertStringList(periodEnd);
    
    if(widget(2))
        delete balance.widget;
    
    balance.widget = new QWidget(this);
    balance.vBoxLayout = new QVBoxLayout(balance.widget);
    balance.vBoxLayout->setMargin(5);
    balance.vBoxLayout->setSpacing(5);
    
    balance.labelLayout = new QHBoxLayout(balance.vBoxLayout);
    balance.topLabel = new GoBackLabel(balance.widget);
    connect(balance.topLabel, SIGNAL(goBack()), this, SLOT(switchWidget()));
    
    balance.labelLayout->addWidget(balance.topLabel);
    balance.labelLayout->addStretch();
    
    balance.editor = new BalanceSheetEditor(balance.widget);
    
    balance.vBoxLayout->addWidget(balance.editor);
    
    addWidget(balance.widget, 2);
}

void ReportStack::updateAccounts()
{
    accounts = db->getAccountsList();
    main.generalDetailAccountRangeBegin->updateAccounts(accounts);
    main.generalDetailAccountRangeEnd->updateAccounts(accounts);
    main.generalTrialAccountRangeBegin->updateAccounts(accounts);
    main.generalTrialAccountRangeEnd->updateAccounts(accounts);
}

void ReportStack::viewReport(QString &text)
{
    report.view->setText(text);

    switchWidget();
}

void ReportStack::switchWidget()
{
    if(active == 0)
    {
            raiseWidget(1);
            active = 1;
    }
    else
    {
        raiseWidget(0);
        active = 0;
    }
}

void ReportStack::generalReport()
{

}

void ReportStack::chartAccountsReport()
{
    viewReport(db->getChartAccounts()->reportString());
}

void ReportStack::balanceReport()
{}

void ReportStack::printerOptions()
{}

void ReportStack::exportHTML()
{
    QString file = QFileDialog::getSaveFileName(
            ".",
            "HTML Files (*.html)",
            this,
            0,
            "Export Report to HTML");
    
    QFile destFile(file);
    
    if(destFile.open(IO_WriteOnly))
    {
        QTextStream destStream(&destFile);
        destStream << db->reportHTML();
    }
    destFile.close();
}

void ReportStack::exportCSV()
{
    QString file = QFileDialog::getSaveFileName(
            ".",
            "Comma Separated Values (*.csv)",
            this,
            0,
            "Export Report to CSV");
    
    QFile destFile(file);
    
    if(destFile.open(IO_WriteOnly))
    {
        QTextStream destStream(&destFile);
        destStream << db->reportCSV();
    }
    destFile.close();
}

void ReportStack::switchToGeneralReport()
{}

void ReportStack::switchToEditBalance()
{
    raiseWidget(2);
    active = 2;
}

bool ReportStack::eventFilter(QObject *target, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = (QKeyEvent *)event;
        if(keyEvent->key() == Key_Escape)
        {
            emit goToMain();
            return true;
        }
    }
    return QWidgetStack::eventFilter(target,event);
}  

