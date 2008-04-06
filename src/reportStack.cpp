#include <qwidgetstack.h>
#include <qvgroupbox.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qframe.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qtextedit.h>
#include <qevent.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qfiledialog.h>

#include "reportStack.h"
#include "goBackLabel.h"
#include "balanceSheetEditor.h"
#include "database.h"

ReportStack::ReportStack(QWidget *parent, const char *name)
    : QWidgetStack(parent,name)
{
    db = new Database();
    
    main.widget = new QWidget(this);
    
    main.vBoxLabel = new QVBoxLayout(main.widget);
    
    main.hBoxLabel = new QHBoxLayout(main.vBoxLabel);
    main.hBoxLabel->setMargin(5);
    
    main.topLabel = new GoBackLabel(main.widget);
    connect(main.topLabel, SIGNAL(goBack()), this, SIGNAL(goBack()));
    
    main.hBoxLabel->addWidget(main.topLabel);
    main.hBoxLabel->addStretch();
    
    main.hBoxLayout = new QHBoxLayout(main.vBoxLabel);
    
    main.hBoxLayout->addStretch();
    
    main.vBoxLayout = new QVBoxLayout(main.hBoxLayout);
    main.vBoxLayout->setSpacing(30);
    
    main.vBoxLayout->addStretch();
    
    main.generalGroup = new QVGroupBox("General Ledger and Account Reports", main.widget);
    main.generalGroup->setInsideMargin(30);
    main.generalGroup->setInsideSpacing(20);
    
    main.vBoxLayout->add(main.generalGroup);
    
    main.generalButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/generalDetailReport.png") ),
            "General Ledger Detail / Trial Balance", main.generalGroup);
    connect(main.generalButton, SIGNAL(clicked()), this, SLOT(generalReport()));
    main.generalButton->installEventFilter(this);
    
    main.chartAccountsButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/chartOfAccounts.png") ),
            "Chart of Accounts", main.generalGroup);
    connect(main.chartAccountsButton, SIGNAL(clicked()), this, SLOT(chartAccountsReport()));
    main.chartAccountsButton->installEventFilter(this);
    
    
    main.balanceGroup = new QVGroupBox("Balance Sheet Reports", main.widget);
    main.balanceGroup->setInsideMargin(30);
    main.balanceGroup->setInsideSpacing(20);
    
    main.vBoxLayout->add(main.balanceGroup);
    
    main.balanceModifyButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/balanceModify.png") ),
            "Modify Balance Sheet", main.balanceGroup);
    connect(main.balanceModifyButton, SIGNAL(clicked()), this, SLOT(switchToEditBalance()));
    main.balanceModifyButton->installEventFilter(this);
    
    main.balanceReportButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/balanceReport.png") ),
            "Balance Sheet Report", main.balanceGroup);
    connect(main.balanceReportButton, SIGNAL(clicked()), this, SLOT(balanceReport()));
    main.balanceReportButton->installEventFilter(this);
    
    
    main.adminGroup = new QVGroupBox("Administrative Options", main.widget);
    main.adminGroup->setInsideMargin(30);
    
    main.vBoxLayout->add(main.adminGroup);
    
    main.printerOptionsButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/printerOptionsButton.png") ),
            "Printer Options", main.adminGroup);
    connect(main.printerOptionsButton, SIGNAL(clicked()), this, SLOT(printerOptions()));
    main.printerOptionsButton->installEventFilter(this);
    
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
    
    /*
    removeWidget(report.widget);
    delete report.exportCSVButton;
    delete report.exportHTMLButton;
    delete report.bottomRightBoxLayout;
    delete report.bottomRightFrame;
    delete report.bottomLabel;
    delete report.bottomHBoxLayout;
    delete report.bottomWidget;
    delete report.view;
    delete report.print;
    delete report.topLabel;
    delete report.labelLayout;
    delete report.vBoxLayout;
    delete report.widget;
    
    if(widget(2))
    {
        removeWidget(balance.widget);   
        delete balance.editor;
        delete balance.topLabel;
        delete balance.labelLayout;
        delete balance.vBoxLayout;
        delete balance.widget;
    }
    
    removeWidget(main.widget);
    delete main.printerOptionsButton;
    delete main.adminGroup;
    delete main.balanceReportButton;
    delete main.balanceModifyButton;
    delete main.balanceGroup;
    delete main.chartAccountsButton;
    delete main.generalButton;
    delete main.generalGroup;
    delete main.vBoxLayout;
    delete main.hBoxLayout;
    delete main.topLabel;
    delete main.hBoxLabel;
    delete main.vBoxLabel;
    delete main.widget;
    */
}

void ReportStack::rangeToggled()
{
    /*
    main.accountRangeRadio->setChecked(true);
    main.accountRangeAllRadio->setChecked(false);
    main.accountRangeFirst->setEnabled(true);
    main.accountRangeMiddleLabel->setEnabled(true);
    main.accountRangeLast->setEnabled(true);
    */
} 
        
void ReportStack::allToggled()
{
    /*
    main.accountRangeAllRadio->setChecked(true);
    main.accountRangeRadio->setChecked(false);
    main.accountRangeFirst->setEnabled(false);
    main.accountRangeMiddleLabel->setEnabled(false);
    main.accountRangeLast->setEnabled(false);
    */
}

void ReportStack::dbOpened()
{
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
        void switchToEditBalance();
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
            emit goBack();
            return true;
        }
    }
    return QWidgetStack::eventFilter(target,event);
}  

