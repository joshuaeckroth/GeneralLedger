/* General Ledger, Copyright (C) 2004  Joshua Eckroth <josh@eckroth.net>
 * http://www.eckroth.net/software/genledg
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  US
*/

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
#include "settings.h"
#include "globalGoBackLabel.h"
#include "accountsData.h"
#include "client.h"
#include "reportsTab.h"
#include "reportsGenerator.h"
#include "reportsEditor.h"
#include "reportsPrint.h"
#include "reportsPDFDialog.h"
#include "importExport.h"

ReportsTab::ReportsTab(QWidget *parent, const char *name)
    : QWidgetStack(parent,name)
{
    settings = Settings::instance();
    accountsData = AccountsData::instance();
    client = Client::instance();
    reportsGenerator = ReportsGenerator::instance();
    reportsPrint = ReportsPrint::instance();

    clientPath = settings->getClientPath();
    exportPath = settings->getExportPath();
    
    connect(accountsData, SIGNAL(accountsChanged()),
                    this, SLOT(updateAccounts()));
    connect(client, SIGNAL(clientOpened(QString)),
                    this, SLOT(clientOpened()));
    
    accounts = QStringList("                                  ");
    // set a placeholder width

    QStringList periodTmp("Period X (00/00/00)  ");
    // placeholder for combobox sizes
    
    main.widget = new QWidget(this, "reportsTab::main.widget");
    
    main.vBoxLabel = new QVBoxLayout(main.widget, 0, -1,
                    "reportsTab::main.vBoxLabel");
    
    main.hBoxLabel = new QHBoxLayout(main.vBoxLabel, -1,
                    "reportsTab::main.hBoxLabel");
    main.hBoxLabel->setMargin(5);
    
    main.topLabel = new GlobalGoBackLabel(main.widget,
                    "reportsTab::main.topLabel");
    connect(main.topLabel, SIGNAL(goBack()), this, SIGNAL(switchToHome()));
    
    main.hBoxLabel->addWidget(main.topLabel);
    main.hBoxLabel->addStretch();
    
    main.hBoxLayout = new QHBoxLayout(main.vBoxLabel, -1,
                    "reportsTab::main.hBoxLayout");
    
    main.hBoxLayout->addStretch();
    
    main.vBoxLayout = new QVBoxLayout(main.hBoxLayout, -1,
                    "reportsTab::main.vBoxLayout");
    main.vBoxLayout->setSpacing(30);
    
    main.vBoxLayout->addStretch();
    
    
    main.reportsGroup = new QVGroupBox("Reports", main.widget,
                    "reportsTab::main.reportsGroup");
    main.reportsGroup->setInsideMargin(30);
    main.reportsGroup->setInsideSpacing(20);
    
    main.vBoxLayout->add(main.reportsGroup);
    
    main.reportsWidget = new QWidget(main.reportsGroup,
                    "reportsTab::main.reportsWidget");
    
    main.reportsGridLayout = new QGridLayout(main.reportsWidget, 6, 2, 0, 15,
                    "reportsTab::main.reportsGridLayout");
                    // rows, cols, margin, spacing
   
    
    main.generalDetailVGroupBox = new QVGroupBox(main.reportsWidget,
                    "reportsTab::main.generalDetailVGroupBox");
    main.generalDetailVGroupBox->setInsideSpacing(5);
    
    main.generalDetailDateRangeWidget = new QWidget(main.generalDetailVGroupBox,
                    "reportsTab::main.generalDetailDateRangeWidget");
    main.generalDetailDateRangeHBoxLayout = new QHBoxLayout(
                    main.generalDetailDateRangeWidget, 0, -1,
                    "reportsTab::main.generalDetailDateRangeHBoxLayout");
    main.generalDetailDateRangeHBoxLayout->setSpacing(5);
    
    main.generalDetailDateRangeLabelBegin = new QLabel("Date Range:",
                    main.generalDetailDateRangeWidget,
                    "reportsTab::main.generalDetailDateRangeLabelBegin");
    main.generalDetailDateRangeBegin = new QComboBox(
                    main.generalDetailDateRangeWidget,
                    "reportsTab::main.generalDetailDateRangeBegin");
    main.generalDetailDateRangeBegin->setFocus();
    main.generalDetailDateRangeBegin->installEventFilter(this);
    main.generalDetailDateRangeBegin->insertStringList(periodTmp);
    main.generalDetailDateRangeLabelBetween = new QLabel("to",
                    main.generalDetailDateRangeWidget,
                    "reportsTab::main.generalDetailDateRangeLabelBetween");
    main.generalDetailDateRangeEnd = new QComboBox(
                    main.generalDetailDateRangeWidget,
                    "reportsTab::main.generalDetailDateRangeEnd");
    main.generalDetailDateRangeEnd->installEventFilter(this);
    main.generalDetailDateRangeEnd->insertStringList(periodTmp);
    
    main.generalDetailDateRangeHBoxLayout->addWidget(
                    main.generalDetailDateRangeLabelBegin);
    main.generalDetailDateRangeHBoxLayout->addWidget(
                    main.generalDetailDateRangeBegin);
    main.generalDetailDateRangeHBoxLayout->addWidget(
                    main.generalDetailDateRangeLabelBetween);
    main.generalDetailDateRangeHBoxLayout->addWidget(
                    main.generalDetailDateRangeEnd);
    main.generalDetailDateRangeHBoxLayout->addStretch();
    
    main.generalDetailAccountRangeWidget = new QWidget(
                    main.generalDetailVGroupBox,
                    "reportsTab::main.generalDetailAccountRangeWidget");
    main.generalDetailAccountRangeHBoxLayout = new QHBoxLayout(
                    main.generalDetailAccountRangeWidget, 0, -1,
                    "reportsTab::main.generalDetailAccountRangeHBoxLayout");
    main.generalDetailAccountRangeHBoxLayout->setSpacing(5);
    
    main.generalDetailAccountRangeLabel = new QLabel("Account Range:",
                    main.generalDetailAccountRangeWidget,
                    "reportsTab::main.generalDetailAccountRangeLabel");
    main.generalDetailAccountRangeAll = new QCheckBox("All",
                    main.generalDetailAccountRangeWidget,
                    "reportsTab::main.generalDetailAccountRangeAll");
    main.generalDetailAccountRangeAll->installEventFilter(this);
    main.generalDetailAccountRangeAll->setChecked(true);
    connect(main.generalDetailAccountRangeAll, SIGNAL(toggled(bool)),
                    this, SLOT(detailAccountsAllChanged(bool)));
    main.generalDetailAccountRangeBegin = new QComboBox(
                    main.generalDetailAccountRangeWidget,
                    "reportsTab::main.generalDetailAccountRangeBegin");
    main.generalDetailAccountRangeBegin->insertStringList(accounts);
    main.generalDetailAccountRangeBegin->installEventFilter(this);
    main.generalDetailAccountRangeBegin->setEnabled(false);
    main.generalDetailAccountRangeLabelBetween = new QLabel("to",
                    main.generalDetailAccountRangeWidget,
                    "reportsTab::main.generalDetailAccountRangeLabelBetween");
    main.generalDetailAccountRangeLabelBetween->setEnabled(false);
    main.generalDetailAccountRangeEnd = new QComboBox(
                    main.generalDetailAccountRangeWidget,
                    "reportsTab::main.generalDetailAccountRangeEnd");
    main.generalDetailAccountRangeEnd->insertStringList(accounts);
    main.generalDetailAccountRangeEnd->installEventFilter(this);
    main.generalDetailAccountRangeEnd->setEnabled(false);

    main.generalDetailAccountRangeHBoxLayout->addWidget(
                    main.generalDetailAccountRangeLabel);    
    main.generalDetailAccountRangeHBoxLayout->addWidget(
                    main.generalDetailAccountRangeAll);
    main.generalDetailAccountRangeHBoxLayout->addWidget(
                    main.generalDetailAccountRangeBegin);
    main.generalDetailAccountRangeHBoxLayout->addWidget(
                    main.generalDetailAccountRangeLabelBetween);
    main.generalDetailAccountRangeHBoxLayout->addWidget(
                    main.generalDetailAccountRangeEnd);
    main.generalDetailAccountRangeHBoxLayout->addStretch();
    
    main.generalDetailButtonVBoxLayout = new QVBoxLayout(0, 0, -1,
                    "reportsTab::main.generalDetailButtonVBoxLayout");
    main.generalDetailButton = new QPushButton(
                    QIconSet(QPixmap::fromMimeSource(
                                    ICON_PATH + "/generalDetailReport.png")),
                    "General Ledger Detail", main.reportsWidget,
                    "reportsTab::main.generalDetailButton");
    connect(main.generalDetailButton, SIGNAL(clicked()), this,
                    SLOT(generalDetailReport()));
    main.generalDetailButton->setDefault(true);
    main.generalDetailButton->installEventFilter(this);
    main.generalDetailButtonVBoxLayout->addWidget(main.generalDetailButton);
    
    main.reportsGridLayout->addWidget(main.generalDetailVGroupBox, 0, 0);
    main.reportsGridLayout->addLayout(main.generalDetailButtonVBoxLayout, 0, 1);
    
    
    main.generalTrialVGroupBox = new QVGroupBox(main.reportsWidget,
                    "reportsTab::main.generalTrialVGroupBox");
    main.generalTrialVGroupBox->setInsideSpacing(5);
    
    main.generalTrialMonthEndWidget = new QWidget(main.generalTrialVGroupBox,
                    "reportsTab::main.generalTrialMonthEndWidget");
    main.generalTrialMonthEndHBoxLayout = new QHBoxLayout(
                    main.generalTrialMonthEndWidget, 0, -1,
                    "reportsTab::main.generalTrialMonthEndHBoxLayout");
    main.generalTrialMonthEndHBoxLayout->setSpacing(5);
    
    main.generalTrialMonthEndLabel = new QLabel("Month-End:",
                    main.generalTrialMonthEndWidget,
                    "reportsTab::main.generalTrialMonthEndLabel");
    main.generalTrialMonthEndList = new QComboBox(
                    main.generalTrialMonthEndWidget,
                    "reportsTab::main.generalTrialMonthEndList");
    main.generalTrialMonthEndList->installEventFilter(this);
    main.generalTrialMonthEndList->insertStringList(periodTmp);
    
    main.generalTrialMonthEndHBoxLayout->addWidget(
                    main.generalTrialMonthEndLabel);
    main.generalTrialMonthEndHBoxLayout->addWidget(
                    main.generalTrialMonthEndList);
    main.generalTrialMonthEndHBoxLayout->addStretch();
    
    main.generalTrialAccountRangeWidget = new QWidget(
                    main.generalTrialVGroupBox,
                    "reportsTab::main.generalTrialAccountRangeWidget");
    main.generalTrialAccountRangeHBoxLayout = new QHBoxLayout(
                    main.generalTrialAccountRangeWidget, 0, -1,
                    "reportsTab::main.generalTrialAccountRangeHBoxLayout");
    main.generalTrialAccountRangeHBoxLayout->setSpacing(5);
    
    main.generalTrialAccountRangeLabel = new QLabel("Account Range:",
                    main.generalTrialAccountRangeWidget,
                    "reportsTab::main.generalTrialAccountRangeLabel");
    main.generalTrialAccountRangeAll = new QCheckBox("All",
                    main.generalTrialAccountRangeWidget,
                    "reportsTab::main.generalTrialAccountRangeAll");
    main.generalTrialAccountRangeAll->installEventFilter(this);
    main.generalTrialAccountRangeAll->setChecked(true);
    connect(main.generalTrialAccountRangeAll, SIGNAL(toggled(bool)),
                    this, SLOT(trialAccountsAllChanged(bool)));
    main.generalTrialAccountRangeBegin = new QComboBox(
                    main.generalTrialAccountRangeWidget,
                    "reportsTab::main.generalTrialAccountRangeBegin");
    main.generalTrialAccountRangeBegin->insertStringList(accounts);
    main.generalTrialAccountRangeBegin->installEventFilter(this);
    main.generalTrialAccountRangeBegin->setEnabled(false);
    main.generalTrialAccountRangeLabelBetween = new QLabel("to",
                    main.generalTrialAccountRangeWidget,
                    "reportsTab::main.generalTrialAccountRangeLabelBetween");
    main.generalTrialAccountRangeLabelBetween->setEnabled(false);
    main.generalTrialAccountRangeEnd = new QComboBox(
                    main.generalTrialAccountRangeWidget,
                    "reportsTab::main.generalTrialAccountRangeEnd");
    main.generalTrialAccountRangeEnd->insertStringList(accounts);
    main.generalTrialAccountRangeEnd->installEventFilter(this);
    main.generalTrialAccountRangeEnd->setEnabled(false);
    
    main.generalTrialAccountRangeHBoxLayout->addWidget(
                    main.generalTrialAccountRangeLabel);
    main.generalTrialAccountRangeHBoxLayout->addWidget(
                    main.generalTrialAccountRangeAll);
    main.generalTrialAccountRangeHBoxLayout->addWidget(
                    main.generalTrialAccountRangeBegin);
    main.generalTrialAccountRangeHBoxLayout->addWidget(
                    main.generalTrialAccountRangeLabelBetween);
    main.generalTrialAccountRangeHBoxLayout->addWidget(
                    main.generalTrialAccountRangeEnd);
    main.generalTrialAccountRangeHBoxLayout->addStretch();
    
    main.generalTrialButtonVBoxLayout = new QVBoxLayout(0, 0, -1,
                    "reportsTab::main.generalTrialButtonVBoxLayout");
    main.generalTrialButton = new QPushButton(
                    QIconSet(QPixmap::fromMimeSource(
                                    ICON_PATH + "/generalTrialReport.png")),
                    "General Trial Balance", main.reportsWidget,
                    "reportsTab::main.generalTrialButton");
    connect(main.generalTrialButton, SIGNAL(clicked()),
                    this, SLOT(generalTrialReport()));
    main.generalTrialButton->installEventFilter(this);
    main.generalTrialButtonVBoxLayout->addWidget(main.generalTrialButton);
    
    main.reportsGridLayout->addWidget(main.generalTrialVGroupBox, 1, 0);
    main.reportsGridLayout->addLayout(main.generalTrialButtonVBoxLayout, 1, 1);
    
    
    main.editableVGroupBox = new QVGroupBox(main.reportsWidget,
                    "reportsTab::main.editableVGroupBox");
    main.editableVGroupBox->setInsideSpacing(5);
    
    main.editableMonthEndWidget = new QWidget(main.editableVGroupBox,
                    "reportsTab::main.editableMonthEndWidget");
    main.editableMonthEndHBoxLayout = new QHBoxLayout(
                    main.editableMonthEndWidget, 0, -1,
                    "reportsTab::main.editableMonthEndHBoxLayout");
    main.editableMonthEndHBoxLayout->setSpacing(5);
    
    main.editableMonthEndLabel = new QLabel("Month-End:",
                    main.editableMonthEndWidget,
                    "reportsTab::main.editableMonthEndLabel");
    main.editableMonthEndList = new QComboBox(main.editableMonthEndWidget,
                    "reportsTab::main.editableMonthEndList");
    main.editableMonthEndList->installEventFilter(this);
    main.editableMonthEndList->insertStringList(periodTmp);
    
    main.editableMonthEndHBoxLayout->addWidget(main.editableMonthEndLabel);
    main.editableMonthEndHBoxLayout->addWidget(main.editableMonthEndList);
    main.editableMonthEndHBoxLayout->addStretch();

    main.editableViewWidget = new QWidget(main.reportsWidget,
                    "reportsTab::main.editableViewWidget");
    main.editableViewVBoxLayout = new QVBoxLayout(main.editableViewWidget,
                    0, -1, "reportsTab::main.editableViewVBoxLayout");
    main.editableViewVBoxLayout->addSpacing(
                    main.editableMonthEndList->height());

    main.editableListWidget = new QWidget(main.editableVGroupBox,
                    "reportsTab::main.editableListWidget");
    main.editableListHBoxLayout = new QHBoxLayout(main.editableListWidget,
                    0, -1, "reportsTab::main.editableListHBoxLayout");
    main.editableListVBoxLayout = new QVBoxLayout(main.editableListHBoxLayout,
                    5, "reportsTab::main.editableListVBoxLayout");

    main.editableBalanceHBoxLayout = new QHBoxLayout(
                    main.editableListVBoxLayout, 5,
                    "reportsTab::main.editableBalanceHBoxLayout");

    main.editableBalanceLabel = new QLabel("Balance Sheet",
                    main.editableListWidget,
                    "reportsTab::main.editableBalanceLabel");
    main.editableBalanceModify = new QPushButton(
                    QIconSet(QPixmap::fromMimeSource(
                                    ICON_PATH + "/modifyReport.png")),
                    "Modify", main.editableListWidget,
                    "reportsTab::main.editableBalanceModify");
    connect(main.editableBalanceModify, SIGNAL(clicked()),
                    this, SLOT(switchToEditBalance()));
    main.editableBalanceModify->installEventFilter(this);
    main.editableBalanceView = new QPushButton(
                    QIconSet(QPixmap::fromMimeSource(
                                    ICON_PATH + "/viewEditableReport.png")),
                    "Balance Sheet", main.editableViewWidget,
                    "reportsTab::main.editableBalanceView");
    connect(main.editableBalanceView, SIGNAL(clicked()),
                    this, SLOT(balanceReport()));
    main.editableBalanceView->installEventFilter(this);

    main.editableBalanceHBoxLayout->addWidget(main.editableBalanceLabel);
    main.editableBalanceHBoxLayout->addWidget(main.editableBalanceModify);
    main.editableViewVBoxLayout->addWidget(main.editableBalanceView);

    main.editableIncomeSvcHBoxLayout = new QHBoxLayout(
                    main.editableListVBoxLayout, 5,
                    "reportsTab::main.editableIncomeSvcHBoxLayout");

    main.editableIncomeSvcLabel = new QLabel("Income Statement Service Co.",
                    main.editableListWidget,
                    "reportsTab::main.editableIncomeSvcLabel");
    main.editableIncomeSvcModify = new QPushButton(
                    QIconSet(QPixmap::fromMimeSource(
                                    ICON_PATH + "/modifyReport.png")),
                    "Modify", main.editableListWidget,
                    "reportsTab::main.editableIncomeSvcModify");
    connect(main.editableIncomeSvcModify, SIGNAL(clicked()),
                    this, SLOT(switchToEditIncomeSvc()));
    main.editableIncomeSvcModify->installEventFilter(this);
    main.editableIncomeSvcView = new QPushButton(
                    QIconSet(QPixmap::fromMimeSource(
                                    ICON_PATH + "/viewEditableReport.png")),
                    "Income Statement Svc. Co.", main.editableViewWidget,
                    "reportsTab::main.editableIncomeSvcView");
    connect(main.editableIncomeSvcView, SIGNAL(clicked()),
                    this, SLOT(incomeSvcReport()));
    main.editableIncomeSvcView->installEventFilter(this);

    main.editableIncomeSvcHBoxLayout->addWidget(main.editableIncomeSvcLabel);
    main.editableIncomeSvcHBoxLayout->addWidget(main.editableIncomeSvcModify);
    main.editableViewVBoxLayout->addWidget(main.editableIncomeSvcView);

    main.editableIncomeRetailHBoxLayout = new QHBoxLayout(
                    main.editableListVBoxLayout, 5,
                    "reportsTab::main.editableIncomeRetailHBoxLayout");

    main.editableIncomeRetailLabel = new QLabel("Income Statement Retail",
                    main.editableListWidget,
                    "reportsTab::main.editableIncomeRetailLabel");
    main.editableIncomeRetailModify = new QPushButton(
                    QIconSet(QPixmap::fromMimeSource(
                                    ICON_PATH + "/modifyReport.png")),
                    "Modify", main.editableListWidget,
                    "reportsTab::main.editableIncomeRetailModify");
    connect(main.editableIncomeRetailModify, SIGNAL(clicked()),
                    this, SLOT(switchToEditIncomeRetail()));
    main.editableIncomeRetailModify->installEventFilter(this);
    main.editableIncomeRetailView = new QPushButton(
                    QIconSet(QPixmap::fromMimeSource(
                                    ICON_PATH + "/viewEditableReport.png")),
                    "Income Statement Retail", main.editableViewWidget,
                    "reportsTab::main.editableIncomeRetailView");
    connect(main.editableIncomeRetailView, SIGNAL(clicked()),
                    this, SLOT(incomeRetailReport()));
    main.editableIncomeRetailView->installEventFilter(this);

    main.editableIncomeRetailHBoxLayout->addWidget(
                    main.editableIncomeRetailLabel);
    main.editableIncomeRetailHBoxLayout->addWidget(
                    main.editableIncomeRetailModify);
    main.editableViewVBoxLayout->addWidget(
                    main.editableIncomeRetailView);
    
    
    main.reportsGridLayout->addWidget(main.editableVGroupBox, 2, 0);
    main.reportsGridLayout->addWidget(main.editableViewWidget, 2, 1);

    main.chartAccountsButtonVBoxLayout = new QVBoxLayout(0, 0, -1,
                    "reportsTab::main.chartAccountsButtonVBoxLayout");
    main.chartAccountsButton = new QPushButton(
                    QIconSet(QPixmap::fromMimeSource(
                                    ICON_PATH + "/chartOfAccounts.png")),
                    "Chart of Accounts", main.reportsWidget,
                    "reportsTab::main.chartAccountsButton");
    connect(main.chartAccountsButton, SIGNAL(clicked()),
                    this, SLOT(chartAccountsReport()));
    main.chartAccountsButton->installEventFilter(this);
    main.chartAccountsButtonVBoxLayout->addWidget(main.chartAccountsButton);
    
    
    main.reportsGridLayout->addLayout(main.chartAccountsButtonVBoxLayout, 5, 1);
    
    
    main.vBoxLayout->addStretch();
    
    main.hBoxLayout->addStretch();
    
    
    addWidget(main.widget, 0);
    
    
    report.widget = new QWidget(this, "reportsTab::report.widget");
    report.vBoxLayout = new QVBoxLayout(report.widget, 0, -1,
                    "reportsTab::report.vBoxLayout");
    report.vBoxLayout->setMargin(5);
    report.vBoxLayout->setSpacing(5);
    
    report.labelLayout = new QBoxLayout(report.vBoxLayout,
                    QBoxLayout::LeftToRight, -1,
                    "reportsTab::report.labelLayout");
    report.topLabel = new GlobalGoBackLabel(report.widget,
                    "reportsTab::report.topLabel");
    connect(report.topLabel, SIGNAL(goBack()), this, SLOT(switchWidget()));
    
    report.print = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/print.png") ),
            "Print Report (F8)", report.widget, "reportsTab::report.print");
    report.print->setFocusPolicy(QWidget::NoFocus);
    connect(report.print, SIGNAL(clicked()), this, SLOT(print()));
    
    report.labelLayout->addWidget(report.topLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    report.labelLayout->addStretch();
    report.labelLayout->addWidget(report.print);
    
    report.view = new QTextEdit(report.widget, "reportsTab::report.view");
    report.view->setTextFormat(QTextEdit::RichText);
    report.view->setReadOnly(true);
    report.view->setVScrollBarMode(QScrollView::AlwaysOn);
    report.view->installEventFilter(this);
    
    report.vBoxLayout->addWidget(report.view);
    
    report.bottomWidget = new QWidget(report.widget,
                    "reportsTab::report.bottomWidget");
    report.bottomHBoxLayout = new QHBoxLayout(report.bottomWidget, 0, -1,
                    "reportsTab::report.bottomHBoxLayout");
    
    report.bottomLabel = new QLabel(
            "<nobr><b>Keyboard Control:</b></nobr><br>"
            "<nobr> &nbsp; &nbsp; <b>Up</b> / <b>Down</b>: "
            "Scroll the page</nobr><br>",
            report.bottomWidget, "reportsTab::report.bottomLabel");
    
    report.bottomRightFrame = new QFrame(report.bottomWidget,
                    "reportsTab::report.bottomRightFrame");
    report.bottomRightBoxLayout = new QBoxLayout(report.bottomRightFrame,
                    QBoxLayout::LeftToRight, 0, -1,
                    "reportsTab::report.bottomRightBoxLayout");
    report.bottomRightBoxLayout->setSpacing(5);
    
    report.exportPDFButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/exportPDF.png") ),
            "Export to PDF (F11)", report.bottomRightFrame,
            "reportsTab::report.exportPDFButton");
    connect(report.exportPDFButton, SIGNAL(clicked()), this, SLOT(exportPDF()));
    
    report.exportCSVButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/export.png") ),
            "Export to CSV (F12)", report.bottomRightFrame,
            "reportsTab::report.exportCSVButton");
    connect(report.exportCSVButton, SIGNAL(clicked()), this, SLOT(exportCSV()));
    
    report.bottomRightBoxLayout->addWidget(report.exportPDFButton, 0,
                    Qt::AlignLeft | Qt::AlignTop);
    report.bottomRightBoxLayout->addWidget(report.exportCSVButton, 0,
                    Qt::AlignLeft | Qt::AlignTop);
    
    report.bottomHBoxLayout->addWidget(report.bottomLabel);
    report.bottomHBoxLayout->addStretch();
    report.bottomHBoxLayout->addWidget(report.bottomRightFrame);
    
    report.vBoxLayout->addWidget(report.bottomWidget);
    
    addWidget(report.widget, 1);

    raiseWidget(0);
    active = 0;
}

void ReportsTab::clientOpened()
{
    QStringList periodBegin = client->getPeriodList(true);
    QStringList periodEnd = client->getPeriodList(false);
    int currentPeriod = (client->getCurrentPeriod() - 1);
    
    main.generalDetailDateRangeBegin->clear();
    main.generalDetailDateRangeBegin->insertStringList(periodBegin);
    main.generalDetailDateRangeBegin->setCurrentItem(currentPeriod);

    main.generalDetailDateRangeEnd->clear();
    main.generalDetailDateRangeEnd->insertStringList(periodEnd);
    main.generalDetailDateRangeEnd->setCurrentItem(currentPeriod);
    
    main.generalTrialMonthEndList->clear();
    main.generalTrialMonthEndList->insertStringList(periodEnd);
    main.generalTrialMonthEndList->setCurrentItem(currentPeriod);
    
    main.editableMonthEndList->clear();
    main.editableMonthEndList->insertStringList(periodEnd);
    main.editableMonthEndList->setCurrentItem(currentPeriod);

    updateAccounts();
}

void ReportsTab::detailAccountsAllChanged(bool on)
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
        
void ReportsTab::trialAccountsAllChanged(bool on)
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

void ReportsTab::updateAccounts()
{
    accounts = accountsData->getFormattedList(true);
    
    main.generalDetailAccountRangeBegin->clear();
    main.generalDetailAccountRangeBegin->insertStringList(accounts);
    
    main.generalDetailAccountRangeEnd->clear();
    main.generalDetailAccountRangeEnd->insertStringList(accounts);
    main.generalDetailAccountRangeEnd->setCurrentItem(accounts.count() - 1);
    
    main.generalTrialAccountRangeBegin->clear();
    main.generalTrialAccountRangeBegin->insertStringList(accounts);
    
    main.generalTrialAccountRangeEnd->clear();
    main.generalTrialAccountRangeEnd->insertStringList(accounts);
    main.generalTrialAccountRangeEnd->setCurrentItem(accounts.count() - 1);
}

void ReportsTab::viewReport()
{
    report.view->setText(reportsGenerator->getReportHTML());

    switchWidget();
}

void ReportsTab::switchWidget()
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

void ReportsTab::generalDetailReport()
{
    if(main.generalDetailAccountRangeAll->isChecked())
    {
        reportsGenerator->genGeneralDetailReport(
                client->getPeriodDate(main.generalDetailDateRangeBegin->currentItem() + 1, false),
                client->getPeriodDate(main.generalDetailDateRangeEnd->currentItem() + 1, true),
                "", "");
    }
    else
    {
        QString accountBegin = main.generalDetailAccountRangeBegin->currentText().left(
                        main.generalDetailAccountRangeBegin->currentText().find(" - "));
        QString accountEnd = main.generalDetailAccountRangeEnd->currentText().left(
                        main.generalDetailAccountRangeEnd->currentText().find(" - "));

        reportsGenerator->genGeneralDetailReport(
                client->getPeriodDate(main.generalDetailDateRangeBegin->currentItem() + 1, false),
                client->getPeriodDate(main.generalDetailDateRangeEnd->currentItem() + 1, true),
                accountBegin, accountEnd);
    }

    viewReport();
}

void ReportsTab::generalTrialReport()
{
    if(main.generalTrialAccountRangeAll->isChecked())
    {
        reportsGenerator->genGeneralTrialReport(
                client->getPeriodDate(main.generalTrialMonthEndList->currentItem() + 1, true));
    }
    else
    {
        QString accountBegin = main.generalTrialAccountRangeBegin->currentText().left(
                        main.generalTrialAccountRangeBegin->currentText().find(" - "));
        QString accountEnd = main.generalTrialAccountRangeEnd->currentText().left(
                        main.generalTrialAccountRangeBegin->currentText().find(" - "));

        reportsGenerator->genGeneralTrialReport(
                        client->getPeriodDate(
                            main.generalTrialMonthEndList->currentItem() + 1,
                            true),
                        accountBegin, accountEnd);
    }

    viewReport();
}

void ReportsTab::balanceReport()
{
    reportsGenerator->genBalanceSheetReport(
        client->getPeriodDate(main.editableMonthEndList->currentItem() + 1,
                true));

    viewReport();
}

void ReportsTab::chartAccountsReport()
{
    reportsGenerator->genChartOfAccountsReport();
    viewReport();
}

void ReportsTab::incomeSvcReport()
{
    reportsGenerator->genIncomeSvcReport(
        client->getPeriodDate(
        main.editableMonthEndList->currentItem() + 1, true));

    viewReport();
}

void ReportsTab::incomeRetailReport()
{
    reportsGenerator->genIncomeRetailReport(
        client->getPeriodDate(
        main.editableMonthEndList->currentItem() + 1, true));

    viewReport();
}

void ReportsTab::print()
{
    reportsPrint->printReport(reportsGenerator->getReportHeader(),
		    reportsGenerator->getReportHTML(),
		    reportsGenerator->getDocName());
}

void ReportsTab::exportPDF()
{
    ImportExport::exportPDF(reportsGenerator->getReportHTML());
}

void ReportsTab::exportCSV()
{
    ImportExport::exportCSV(reportsGenerator->getReportCSV());
}

void ReportsTab::switchToEditBalance()
{
    switchToEditor(ReportsEditor::BalanceSheet);
}

void ReportsTab::switchToEditIncomeSvc()
{
    switchToEditor(ReportsEditor::IncomeSvc);
}

void ReportsTab::switchToEditIncomeRetail()
{
    switchToEditor(ReportsEditor::IncomeRetail);
}

void ReportsTab::switchToEditor(ReportsEditor::Editor editorType)
{
    if(widget(2))
        delete editor.widget;

    editor.widget = new QWidget(this, "reportsTab::editor.widget");
    editor.vBoxLayout = new QVBoxLayout(editor.widget, 0, -1,
                    "reportsTab::editor.vBoxLayout");
    editor.vBoxLayout->setMargin(5);
    editor.vBoxLayout->setSpacing(5);
    
    editor.labelLayout = new QHBoxLayout(editor.vBoxLayout, -1,
                    "reportsTab::editor.labelLayout");
    editor.topLabel = new GlobalGoBackLabel(editor.widget,
                    "reportsTab::editor.topLabel");
    connect(editor.topLabel, SIGNAL(goBack()), this, SLOT(switchWidget()));
    
    editor.labelLayout->addWidget(editor.topLabel);
    editor.labelLayout->addStretch();
   
    editor.editor = reportsEditor->createEditor(editor.widget,
                    "reportsTab::editor.editor", editorType); 
    
    editor.vBoxLayout->addWidget(editor.editor);

    addWidget(editor.widget, 2);

    raiseWidget(2);
    active = 2;
}

bool ReportsTab::eventFilter(QObject *target, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = (QKeyEvent *)event;
        if(keyEvent->key() == Key_Escape)
        {
            if(active == 0)
                emit switchToHome();
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
                main.chartAccountsButton->setDefault(false);
            }
        }
        else if(target == main.chartAccountsButton)
        {
            main.generalDetailButton->setDefault(false);
            main.generalTrialButton->setDefault(false);
            main.chartAccountsButton->setDefault(true);
        }
    }
    return QWidgetStack::eventFilter(target,event);
}

ReportsTab::~ReportsTab()
{
    reportsGenerator->destroy();
    reportsPrint->destroy();
}

