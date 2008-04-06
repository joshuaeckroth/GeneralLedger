#include <cstdlib>
using std::system;

#include <cstdio>
using std::remove;

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

#include "config.h"
#include "database.h"
#include "settings.h"
#include "printer.h"
#include "reportStack.h"
#include "goBackLabel.h"
#include "balanceSheetEditor.h"
#include "incomeSvcEditor.h"
#include "incomeRetailEditor.h"
#include "accountList.h"
#include "pdfOptions.h"

ReportStack::ReportStack(QWidget *parent, const char *name)
    : QWidgetStack(parent,name)
{
    db = Database::instance();
    settings = Settings::instance();
    printer = Printer::instance();

    clientPath = settings->getClientPath();
    exportPath = settings->getExportPath();
    importPath = settings->getImportPath();
    
    connect(db, SIGNAL(accountsChanged()), this, SLOT(updateAccounts()));
    accounts = QStringList("                    ");  // set a placeholder width

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
    
    main.reportsGridLayout = new QGridLayout(main.reportsWidget, 6, 2, 0, 15);  // rows, cols, margin, spacing
    
    
    main.generalDetailVGroupBox = new QVGroupBox(main.reportsWidget);
    main.generalDetailVGroupBox->setInsideSpacing(5);
    
    main.generalDetailDateRangeWidget = new QWidget(main.generalDetailVGroupBox);
    main.generalDetailDateRangeHBoxLayout = new QHBoxLayout(main.generalDetailDateRangeWidget);
    main.generalDetailDateRangeHBoxLayout->setSpacing(5);
    
    main.generalDetailDateRangeLabelBegin = new QLabel("Date Range:", main.generalDetailDateRangeWidget);
    main.generalDetailDateRangeBegin = new QComboBox(main.generalDetailDateRangeWidget);
    main.generalDetailDateRangeBegin->setFocus();
    main.generalDetailDateRangeBegin->installEventFilter(this);
    main.generalDetailDateRangeBegin->insertStringList(periodTmp);
    main.generalDetailDateRangeLabelBetween = new QLabel("to", main.generalDetailDateRangeWidget);
    main.generalDetailDateRangeEnd = new QComboBox(main.generalDetailDateRangeWidget);
    main.generalDetailDateRangeEnd->installEventFilter(this);
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
    main.generalDetailAccountRangeAll->installEventFilter(this);
    main.generalDetailAccountRangeAll->setChecked(true);
    connect(main.generalDetailAccountRangeAll, SIGNAL(toggled(bool)), this, SLOT(detailAccountsAllChanged(bool)));
    main.generalDetailAccountRangeBegin = new AccountList(main.generalDetailAccountRangeWidget, 0, accounts);
    main.generalDetailAccountRangeBegin->installEventFilter(this);
    main.generalDetailAccountRangeBegin->setEnabled(false);
    main.generalDetailAccountRangeLabelBetween = new QLabel("to", main.generalDetailAccountRangeWidget);
    main.generalDetailAccountRangeLabelBetween->setEnabled(false);
    main.generalDetailAccountRangeEnd = new AccountList(main.generalDetailAccountRangeWidget, 0, accounts);
    main.generalDetailAccountRangeEnd->installEventFilter(this);
    main.generalDetailAccountRangeEnd->setEnabled(false);

    main.generalDetailAccountRangeHBoxLayout->addWidget(main.generalDetailAccountRangeLabel);    
    main.generalDetailAccountRangeHBoxLayout->addWidget(main.generalDetailAccountRangeAll);
    main.generalDetailAccountRangeHBoxLayout->addWidget(main.generalDetailAccountRangeBegin);
    main.generalDetailAccountRangeHBoxLayout->addWidget(main.generalDetailAccountRangeLabelBetween);
    main.generalDetailAccountRangeHBoxLayout->addWidget(main.generalDetailAccountRangeEnd);
    main.generalDetailAccountRangeHBoxLayout->addStretch();
    
    main.generalDetailButtonVBoxLayout = new QVBoxLayout();
    main.generalDetailButton = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/generalDetailReport.png")),
                                               "General Ledger Detail", main.reportsWidget);
    connect(main.generalDetailButton, SIGNAL(clicked()), this, SLOT(generalDetailReport()));
    main.generalDetailButton->setDefault(true);
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
    main.generalTrialMonthEndList->installEventFilter(this);
    main.generalTrialMonthEndList->insertStringList(periodTmp);
    
    main.generalTrialMonthEndHBoxLayout->addWidget(main.generalTrialMonthEndLabel);
    main.generalTrialMonthEndHBoxLayout->addWidget(main.generalTrialMonthEndList);
    main.generalTrialMonthEndHBoxLayout->addStretch();
    
    main.generalTrialAccountRangeWidget = new QWidget(main.generalTrialVGroupBox);
    main.generalTrialAccountRangeHBoxLayout = new QHBoxLayout(main.generalTrialAccountRangeWidget);
    main.generalTrialAccountRangeHBoxLayout->setSpacing(5);
    
    main.generalTrialAccountRangeLabel = new QLabel("Account Range:", main.generalTrialAccountRangeWidget);
    main.generalTrialAccountRangeAll = new QCheckBox("All", main.generalTrialAccountRangeWidget);
    main.generalTrialAccountRangeAll->installEventFilter(this);
    main.generalTrialAccountRangeAll->setChecked(true);
    connect(main.generalTrialAccountRangeAll, SIGNAL(toggled(bool)), this, SLOT(trialAccountsAllChanged(bool)));
    main.generalTrialAccountRangeBegin = new AccountList(main.generalTrialAccountRangeWidget, 0, accounts);
    main.generalTrialAccountRangeBegin->installEventFilter(this);
    main.generalTrialAccountRangeBegin->setEnabled(false);
    main.generalTrialAccountRangeLabelBetween = new QLabel("to", main.generalTrialAccountRangeWidget);
    main.generalTrialAccountRangeLabelBetween->setEnabled(false);
    main.generalTrialAccountRangeEnd = new AccountList(main.generalTrialAccountRangeWidget, 0, accounts);
    main.generalTrialAccountRangeEnd->installEventFilter(this);
    main.generalTrialAccountRangeEnd->setEnabled(false);
    
    main.generalTrialAccountRangeHBoxLayout->addWidget(main.generalTrialAccountRangeLabel);
    main.generalTrialAccountRangeHBoxLayout->addWidget(main.generalTrialAccountRangeAll);
    main.generalTrialAccountRangeHBoxLayout->addWidget(main.generalTrialAccountRangeBegin);
    main.generalTrialAccountRangeHBoxLayout->addWidget(main.generalTrialAccountRangeLabelBetween);
    main.generalTrialAccountRangeHBoxLayout->addWidget(main.generalTrialAccountRangeEnd);
    main.generalTrialAccountRangeHBoxLayout->addStretch();
    
    main.generalTrialButtonVBoxLayout = new QVBoxLayout();
    main.generalTrialButton = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/generalTrialReport.png")),
                                              "General Trial Balance", main.reportsWidget);
    connect(main.generalTrialButton, SIGNAL(clicked()), this, SLOT(generalTrialReport()));
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
    main.balanceMonthEndList->installEventFilter(this);
    main.balanceMonthEndList->insertStringList(periodTmp);
    
    main.balanceMonthEndHBoxLayout->addWidget(main.balanceMonthEndLabel);
    main.balanceMonthEndHBoxLayout->addWidget(main.balanceMonthEndList);
    main.balanceMonthEndHBoxLayout->addStretch();
    
    main.balanceModifyWidget = new QWidget(main.balanceVGroupBox);
    main.balanceModifyHBoxLayout = new QHBoxLayout(main.balanceModifyWidget);
    
    main.balanceModifyButton = new QPushButton(QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/modifyReport.png") ),
                                               "Modify Balance Sheet", main.balanceModifyWidget);
    connect(main.balanceModifyButton, SIGNAL(clicked()), this, SLOT(switchToEditBalance()));
    main.balanceModifyButton->installEventFilter(this);
    
    main.balanceModifyHBoxLayout->addWidget(main.balanceModifyButton);
    main.balanceModifyHBoxLayout->addStretch();
    
    main.balanceReportButtonVBoxLayout = new QVBoxLayout();
    main.balanceReportButton = new QPushButton(QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/balanceReport.png") ),
                                               "Balance Sheet Report", main.reportsWidget);
    connect(main.balanceReportButton, SIGNAL(clicked()), this, SLOT(balanceReport()));
    main.balanceReportButton->installEventFilter(this);
    main.balanceReportButtonVBoxLayout->addWidget(main.balanceReportButton);
        
    
    main.reportsGridLayout->addWidget(main.balanceVGroupBox, 2, 0);
    main.reportsGridLayout->addLayout(main.balanceReportButtonVBoxLayout, 2, 1);
    
    
    main.incomeSvcVGroupBox = new QVGroupBox(main.reportsWidget);
    main.incomeSvcVGroupBox->setInsideSpacing(5);
    
    main.incomeSvcMonthEndWidget = new QWidget(main.incomeSvcVGroupBox);
    main.incomeSvcMonthEndHBoxLayout = new QHBoxLayout(main.incomeSvcMonthEndWidget);
    main.incomeSvcMonthEndHBoxLayout->setSpacing(5);
    
    main.incomeSvcMonthEndLabel = new QLabel("Month-End:", main.incomeSvcMonthEndWidget);
    main.incomeSvcMonthEndList = new QComboBox(main.incomeSvcMonthEndWidget);
    main.incomeSvcMonthEndList->installEventFilter(this);
    main.incomeSvcMonthEndList->insertStringList(periodTmp);
    
    main.incomeSvcMonthEndHBoxLayout->addWidget(main.incomeSvcMonthEndLabel);
    main.incomeSvcMonthEndHBoxLayout->addWidget(main.incomeSvcMonthEndList);
    main.incomeSvcMonthEndHBoxLayout->addStretch();
    
    main.incomeSvcModifyWidget = new QWidget(main.incomeSvcVGroupBox);
    main.incomeSvcModifyHBoxLayout = new QHBoxLayout(main.incomeSvcModifyWidget);
    
    main.incomeSvcModifyButton = new QPushButton(QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/modifyReport.png") ),
                                               "Modify Income Statement Svc. Co.", main.incomeSvcModifyWidget);
    connect(main.incomeSvcModifyButton, SIGNAL(clicked()), this, SLOT(switchToEditIncomeSvc()));
    main.incomeSvcModifyButton->installEventFilter(this);
    
    main.incomeSvcModifyHBoxLayout->addWidget(main.incomeSvcModifyButton);
    main.incomeSvcModifyHBoxLayout->addStretch();
    
    main.incomeSvcButtonVBoxLayout = new QVBoxLayout();
    main.incomeSvcButton = new QPushButton(QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/incomeReport.png") ),
                                        "Income Statement Svc. Co.", main.reportsWidget);
    connect(main.incomeSvcButton, SIGNAL(clicked()), this, SLOT(incomeSvcReport()));
    main.incomeSvcButton->installEventFilter(this);
    main.incomeSvcButtonVBoxLayout->addWidget(main.incomeSvcButton);
    
    
    main.reportsGridLayout->addWidget(main.incomeSvcVGroupBox, 3, 0);
    main.reportsGridLayout->addLayout(main.incomeSvcButtonVBoxLayout, 3, 1);
    

    
    main.incomeRetailVGroupBox = new QVGroupBox(main.reportsWidget);
    main.incomeRetailVGroupBox->setInsideSpacing(5);
    
    main.incomeRetailMonthEndWidget = new QWidget(main.incomeRetailVGroupBox);
    main.incomeRetailMonthEndHBoxLayout = new QHBoxLayout(main.incomeRetailMonthEndWidget);
    main.incomeRetailMonthEndHBoxLayout->setSpacing(5);
    
    main.incomeRetailMonthEndLabel = new QLabel("Month-End:", main.incomeRetailMonthEndWidget);
    main.incomeRetailMonthEndList = new QComboBox(main.incomeRetailMonthEndWidget);
    main.incomeRetailMonthEndList->installEventFilter(this);
    main.incomeRetailMonthEndList->insertStringList(periodTmp);
    
    main.incomeRetailMonthEndHBoxLayout->addWidget(main.incomeRetailMonthEndLabel);
    main.incomeRetailMonthEndHBoxLayout->addWidget(main.incomeRetailMonthEndList);
    main.incomeRetailMonthEndHBoxLayout->addStretch();
    
    main.incomeRetailModifyWidget = new QWidget(main.incomeRetailVGroupBox);
    main.incomeRetailModifyHBoxLayout = new QHBoxLayout(main.incomeRetailModifyWidget);
    
    main.incomeRetailModifyButton = new QPushButton(QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/modifyReport.png") ),
                                            "Modify Income Statement Retail", main.incomeRetailModifyWidget);
    connect(main.incomeRetailModifyButton, SIGNAL(clicked()), this, SLOT(switchToEditIncomeRetail()));
    main.incomeRetailModifyButton->installEventFilter(this);
    
    main.incomeRetailModifyHBoxLayout->addWidget(main.incomeRetailModifyButton);
    main.incomeRetailModifyHBoxLayout->addStretch();
    
    main.incomeRetailButtonVBoxLayout = new QVBoxLayout();
    main.incomeRetailButton = new QPushButton(QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/incomeReport.png") ),
                                           "Income Statement Retail", main.reportsWidget);
    connect(main.incomeRetailButton, SIGNAL(clicked()), this, SLOT(incomeRetailReport()));
    main.incomeRetailButton->installEventFilter(this);
    main.incomeRetailButtonVBoxLayout->addWidget(main.incomeRetailButton);
    
    
    main.reportsGridLayout->addWidget(main.incomeRetailVGroupBox, 4, 0);
    main.reportsGridLayout->addLayout(main.incomeRetailButtonVBoxLayout, 4, 1);



    main.chartAccountsButtonVBoxLayout = new QVBoxLayout();
    main.chartAccountsButton = new QPushButton(QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/chartOfAccounts.png") ),
                                               "Chart of Accounts", main.reportsWidget);
    connect(main.chartAccountsButton, SIGNAL(clicked()), this, SLOT(chartAccountsReport()));
    main.chartAccountsButton->installEventFilter(this);
    main.chartAccountsButtonVBoxLayout->addWidget(main.chartAccountsButton);
    
    
    main.reportsGridLayout->addLayout(main.chartAccountsButtonVBoxLayout, 5, 1);
    
    
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
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/print.png") ),
            "Print Report (F8)", report.widget);
    report.print->setFocusPolicy(QWidget::NoFocus);
    connect(report.print, SIGNAL(clicked()), this, SLOT(print()));
    
    report.labelLayout->addWidget(report.topLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    report.labelLayout->addStretch();
    report.labelLayout->addWidget(report.print);
    
    report.view = new QTextEdit(report.widget);
    report.view->setTextFormat(QTextEdit::RichText);
    report.view->setReadOnly(true);
    report.view->setVScrollBarMode(QScrollView::AlwaysOn);
    report.view->installEventFilter(this);
    
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
    
    report.exportPDFButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/exportpdf.png") ),
            "Export to PDF (F11)", report.bottomRightFrame);
    connect(report.exportPDFButton, SIGNAL(clicked()), this, SLOT(exportPDF()));
    
    report.exportCSVButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/export.png") ),
            "Export to CSV (F12)", report.bottomRightFrame);
    connect(report.exportCSVButton, SIGNAL(clicked()), this, SLOT(exportCSV()));
    
    report.bottomRightBoxLayout->addWidget(report.exportPDFButton, 0, Qt::AlignLeft | Qt::AlignTop);
    report.bottomRightBoxLayout->addWidget(report.exportCSVButton, 0, Qt::AlignLeft | Qt::AlignTop);
    
    report.bottomHBoxLayout->addWidget(report.bottomLabel);
    report.bottomHBoxLayout->addStretch();
    report.bottomHBoxLayout->addWidget(report.bottomRightFrame);
    
    report.vBoxLayout->addWidget(report.bottomWidget);
    
    addWidget(report.widget, 1);
    
}

ReportStack::~ReportStack()
{}

void ReportStack::detailAccountsAllChanged(bool on)
{
    if(on)
    {
        main.generalDetailAccountRangeBegin->setEnabled(false);
        main.generalDetailAccountRangeLabelBetween->setEnabled(false);
        main.generalDetailAccountRangeEnd->setEnabled(false);
        main.generalDetailAccountRangeAll->setFocus();
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
        main.generalTrialAccountRangeAll->setFocus();
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
    main.incomeSvcMonthEndList->clear();
    main.incomeSvcMonthEndList->insertStringList(periodEnd);
    main.incomeRetailMonthEndList->clear();
    main.incomeRetailMonthEndList->insertStringList(periodEnd);

    raiseWidget(0);
    active = 0;
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
            report.view->setFocus();
            active = 1;
    }
    else
    {
        raiseWidget(0);
        active = 0;
    }
}

void ReportStack::generalDetailReport()
{
    if(main.generalDetailAccountRangeAll->isChecked())
        viewReport(db->getGeneralDetail(
                main.generalDetailDateRangeBegin->currentText(),
                main.generalDetailDateRangeEnd->currentText(),
                "", "")->reportString());
    else
        viewReport(db->getGeneralDetail(
                main.generalDetailDateRangeBegin->currentText(),
                main.generalDetailDateRangeEnd->currentText(),
                main.generalDetailAccountRangeBegin->accountNumber(),
                main.generalDetailAccountRangeEnd->accountNumber())->reportString());
}

void ReportStack::generalTrialReport()
{
    if(main.generalTrialAccountRangeAll->isChecked())
        viewReport(db->getGeneralTrialBalance(
            main.generalTrialMonthEndList->currentText(),
            "", "")->reportString());
    else
        viewReport(db->getGeneralTrialBalance(
            main.generalTrialMonthEndList->currentText(),
            main.generalTrialAccountRangeBegin->accountNumber(),
            main.generalTrialAccountRangeEnd->accountNumber())->reportString());
}

void ReportStack::balanceReport()
{
    viewReport(db->getBalanceReport(main.balanceMonthEndList->currentText())->reportString());
}

void ReportStack::chartAccountsReport()
{
    viewReport(db->getChartAccounts()->reportString());
}

void ReportStack::incomeSvcReport()
{
    viewReport(db->getIncomeSvcReport(main.incomeSvcMonthEndList->currentText())->reportString());
}

void ReportStack::incomeRetailReport()
{
    viewReport(db->getIncomeRetailReport(main.incomeRetailMonthEndList->currentText())->reportString());
}

void ReportStack::print()
{
    printer->printReport(db->reportHeader(), db->reportData());
}

void ReportStack::exportPDF()
{
    QString file = QFileDialog::getSaveFileName(
            exportPath,
            "PDF Files (*.pdf)",
            this,
            0,
            "Export Report to PDF");
    
    if(file != "")
    {
        if(file.right(4) != ".pdf")
            file.append(".pdf");

        PDFOptions pdfOptions;
        QString options = pdfOptions.exec();
        if(options == QString::null)
            return;

        QFile tmpHTML(clientPath + "/tmp.html");
        if(!tmpHTML.open( IO_WriteOnly ))
            return;

        QTextStream tmpStream(&tmpHTML);

        QString html = db->reportHTML();
        html.replace("<tr><td> </td></tr>", "<tr><td>&nbsp;<br></td></tr>");

        tmpStream << html;

        tmpStream.unsetDevice();
        tmpHTML.close();

        system(QString(HTMLDOC_PATH + "/htmldoc --footer ./. --fontsize 9.0 "
                + options +
                "--right .5in --left .5in --top .2in "
                "--textfont helvetica --no-toc --no-title --quiet -f \""
                + file + "\" \"" + clientPath + "/tmp.html\""));

        remove(clientPath + "/tmp.html");
    }
}

void ReportStack::exportCSV()
{
    QString file = QFileDialog::getSaveFileName(
            importPath,
            "Comma Separated Values (*.csv)",
            this,
            0,
            "Export Report to CSV");
    
    if(file != "")
    {
        if(file.right(4) != ".csv")
            file.append(".csv");

        QFile destFile(file);
    
        if(destFile.open(IO_WriteOnly))
        {
            QTextStream destStream(&destFile);
            destStream << db->reportCSV();
        }
        destFile.close();
    }
}

void ReportStack::switchToEditBalance()
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

    raiseWidget(2);
    active = 2;
}

void ReportStack::switchToEditIncomeSvc()
{
    if(widget(3))
        delete incomeSvc.widget;

    incomeSvc.widget = new QWidget(this);
    incomeSvc.vBoxLayout = new QVBoxLayout(incomeSvc.widget);
    incomeSvc.vBoxLayout->setMargin(5);
    incomeSvc.vBoxLayout->setSpacing(5);
    
    incomeSvc.labelLayout = new QHBoxLayout(incomeSvc.vBoxLayout);
    incomeSvc.topLabel = new GoBackLabel(incomeSvc.widget);
    connect(incomeSvc.topLabel, SIGNAL(goBack()), this, SLOT(switchWidget()));
    
    incomeSvc.labelLayout->addWidget(incomeSvc.topLabel);
    incomeSvc.labelLayout->addStretch();
    
    incomeSvc.editor = new IncomeSvcEditor(incomeSvc.widget);
    
    incomeSvc.vBoxLayout->addWidget(incomeSvc.editor);
    
    addWidget(incomeSvc.widget, 3);

    raiseWidget(3);
    active = 3;
}

void ReportStack::switchToEditIncomeRetail()
{
    if(widget(4))
        delete incomeRetail.widget;

    incomeRetail.widget = new QWidget(this);
    incomeRetail.vBoxLayout = new QVBoxLayout(incomeRetail.widget);
    incomeRetail.vBoxLayout->setMargin(5);
    incomeRetail.vBoxLayout->setSpacing(5);
    
    incomeRetail.labelLayout = new QHBoxLayout(incomeRetail.vBoxLayout);
    incomeRetail.topLabel = new GoBackLabel(incomeRetail.widget);
    connect(incomeRetail.topLabel, SIGNAL(goBack()), this, SLOT(switchWidget()));
    
    incomeRetail.labelLayout->addWidget(incomeRetail.topLabel);
    incomeRetail.labelLayout->addStretch();
    
    incomeRetail.editor = new IncomeRetailEditor(incomeRetail.widget);
    
    incomeRetail.vBoxLayout->addWidget(incomeRetail.editor);
    
    addWidget(incomeRetail.widget, 4);

    raiseWidget(4);
    active = 4;
}

bool ReportStack::eventFilter(QObject *target, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = (QKeyEvent *)event;
        if(keyEvent->key() == Key_Escape)
        {
            if(active == 0)
                emit goToMain();
            else
                switchWidget();
            return true;
        }
        if((keyEvent->key() == Key_Enter || keyEvent->key() == Key_Return)
           && active == 0)
        {
            if(main.generalDetailButton->isDefault())
                generalDetailReport();
            else if(main.generalTrialButton->isDefault())
                generalTrialReport();
            else if(main.balanceReportButton->isDefault())
                balanceReport();
            else if(main.incomeSvcModifyButton->isDefault())
                switchToEditIncomeSvc();
            else if(main.incomeSvcButton->isDefault())
                incomeSvcReport();
            else if(main.incomeRetailModifyButton->isDefault())
                switchToEditIncomeRetail();
            else if(main.incomeRetailButton->isDefault())
                incomeRetailReport();
            else if(main.chartAccountsButton->isDefault())
                chartAccountsReport();
        }
        if(keyEvent->key() == Key_F8 && active == 1)
        {
            print();
            return true;
        }
        if(keyEvent->key() == Key_F11 && active == 1)
        {
            exportPDF();
            return true;
        }
        if(keyEvent->key() == Key_F12 && active == 1)
        {
            exportCSV();
            return true;
        }
    }
    if(event->type() == QEvent::FocusIn)
    {
        if(target == main.generalDetailDateRangeBegin
           || target == main.generalDetailDateRangeEnd
           || target == main.generalDetailAccountRangeBegin
           || target == main.generalDetailAccountRangeEnd
           || target == main.generalDetailAccountRangeAll
           || target == main.generalDetailButton)
        {
            if(!main.generalDetailButton->isDefault())
            {
                main.generalDetailButton->setDefault(true);
                main.generalTrialButton->setDefault(false);
                main.balanceReportButton->setDefault(false);
                main.incomeSvcModifyButton->setDefault(false);
                main.incomeSvcButton->setDefault(false);
                main.incomeRetailModifyButton->setDefault(false);
                main.incomeRetailButton->setDefault(false);
                main.chartAccountsButton->setDefault(false);
            }
        }
        else if(target == main.generalTrialMonthEndList
                || target == main.generalTrialAccountRangeBegin
                || target == main.generalTrialAccountRangeEnd
                || target == main.generalTrialAccountRangeAll
                || target == main.generalTrialButton)
        {
            if(!main.generalTrialButton->isDefault())
            {
                main.generalDetailButton->setDefault(false);
                main.generalTrialButton->setDefault(true);
                main.balanceReportButton->setDefault(false);
                main.incomeSvcModifyButton->setDefault(false);
                main.incomeSvcButton->setDefault(false);
                main.incomeRetailModifyButton->setDefault(false);
                main.incomeRetailButton->setDefault(false);
                main.chartAccountsButton->setDefault(false);
            }
        }
        else if(target == main.balanceMonthEndList
                || target == main.balanceModifyButton   ///// do this too
                || target == main.balanceReportButton)
        {
            if(!main.balanceReportButton->isDefault())
            {
                main.generalDetailButton->setDefault(false);
                main.generalTrialButton->setDefault(false);
                main.balanceReportButton->setDefault(true);
                main.incomeSvcModifyButton->setDefault(false);
                main.incomeSvcButton->setDefault(false);
                main.incomeRetailModifyButton->setDefault(false);
                main.incomeRetailButton->setDefault(false);
                main.chartAccountsButton->setDefault(false);
            }
        }
        else if(target == main.incomeSvcMonthEndList
                || target == main.incomeSvcButton)
        {
            if(!main.incomeSvcButton->isDefault())
            {
                main.generalDetailButton->setDefault(false);
                main.generalTrialButton->setDefault(false);
                main.balanceReportButton->setDefault(false);
                main.incomeSvcModifyButton->setDefault(false);
                main.incomeSvcButton->setDefault(true);
                main.incomeRetailModifyButton->setDefault(false);
                main.incomeRetailButton->setDefault(false);
                main.chartAccountsButton->setDefault(false);
            }
        }
        else if(target == main.incomeSvcModifyButton)
        {
            if(!main.incomeSvcModifyButton->isDefault())
            {
                main.generalDetailButton->setDefault(false);
                main.generalTrialButton->setDefault(false);
                main.balanceReportButton->setDefault(false);
                main.incomeSvcModifyButton->setDefault(true);
                main.incomeSvcButton->setDefault(false);
                main.incomeRetailModifyButton->setDefault(false);
                main.incomeRetailButton->setDefault(false);
                main.chartAccountsButton->setDefault(false);
            }
        }
        else if(target == main.incomeRetailMonthEndList
                || target == main.incomeRetailButton)
        {
            if(!main.incomeRetailButton->isDefault())
            {
                main.generalDetailButton->setDefault(false);
                main.generalTrialButton->setDefault(false);
                main.balanceReportButton->setDefault(false);
                main.incomeSvcModifyButton->setDefault(false);
                main.incomeSvcButton->setDefault(false);
                main.incomeRetailModifyButton->setDefault(false);
                main.incomeRetailButton->setDefault(true);
                main.chartAccountsButton->setDefault(false);
            }
        }
        else if(target == main.incomeRetailModifyButton)
        {
            if(!main.incomeRetailModifyButton->isDefault())
            {
                main.generalDetailButton->setDefault(false);
                main.generalTrialButton->setDefault(false);
                main.balanceReportButton->setDefault(false);
                main.incomeSvcModifyButton->setDefault(false);
                main.incomeSvcButton->setDefault(false);
                main.incomeRetailModifyButton->setDefault(true);
                main.incomeRetailButton->setDefault(false);
                main.chartAccountsButton->setDefault(false);
            }
        }
        else if(target == main.chartAccountsButton)
        {
            main.generalDetailButton->setDefault(false);
            main.generalTrialButton->setDefault(false);
            main.balanceReportButton->setDefault(false);
            main.incomeSvcModifyButton->setDefault(false);
            main.incomeSvcButton->setDefault(false);
            main.incomeRetailModifyButton->setDefault(false);
            main.incomeRetailButton->setDefault(false);
            main.chartAccountsButton->setDefault(true);
        }
    }
    return QWidgetStack::eventFilter(target,event);
}  

