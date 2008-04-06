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

#include "reportStack.h"
#include "goBackLabel.h"

ReportStack::ReportStack(QWidget *parent, const char *name)
    : QWidgetStack(parent,name)
{
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
    
    main.generalGroup = new QVGroupBox("General Ledger Reports", main.widget);
    main.generalGroup->setInsideMargin(30);
    
    main.monthRangeLabel = new QLabel("Month Range:", main.generalGroup);
    
    main.monthRangeVBox = new QVBox(main.generalGroup);
    main.monthRangeVBox->setSpacing(5);
    
    main.monthRangeFrame = new QFrame(main.monthRangeVBox);
    
    main.monthRangeLayout = new QHBoxLayout(main.monthRangeFrame);
    main.monthRangeLayout->addSpacing(10);
    
    main.monthRangeFirst = new QComboBox(FALSE, main.monthRangeFrame);
    main.monthRangeFirst->setFocus();
    main.monthRangeFirst->insertItem("January");
    main.monthRangeFirst->installEventFilter(this);
    
    main.monthRangeMiddleLabel = new QLabel("   to   ", main.monthRangeFrame);
    
    main.monthRangeLast = new QComboBox(FALSE, main.monthRangeFrame);
    main.monthRangeLast->insertItem("September");
    main.monthRangeLast->installEventFilter(this);
    
    main.monthRangeLayout->add(main.monthRangeFirst);
    main.monthRangeLayout->add(main.monthRangeMiddleLabel);
    main.monthRangeLayout->add(main.monthRangeLast);
    main.monthRangeLayout->addStretch();
    
    main.generalGroup->addSpace(10);
    
    main.accountRangeLabel = new QLabel("Account Range:", main.generalGroup);
    
    main.accountRangeVBox = new QVBox(main.generalGroup);
    main.accountRangeVBox->setSpacing(5);
    
    main.accountRangeFrame = new QFrame(main.accountRangeVBox);
    
    main.accountRangeLayout = new QHBoxLayout(main.accountRangeFrame);
    main.accountRangeLayout->addSpacing(10);
    
    main.accountRangeRadio = new QRadioButton(main.accountRangeFrame);
    main.accountRangeRadio->setChecked(false);
    connect(main.accountRangeRadio, SIGNAL(clicked()), this, SLOT(rangeToggled()));
    main.accountRangeRadio->installEventFilter(this);
    
    main.accountRangeFirst = new QLineEdit("1000", main.accountRangeFrame);
    main.accountRangeFirst->setMaxLength(6);
    main.accountRangeFirst->setEnabled(false);
    main.accountRangeFirst->installEventFilter(this);
    
    main.accountRangeMiddleLabel = new QLabel("   to   ", main.accountRangeFrame);
    main.accountRangeMiddleLabel->setEnabled(false);
    
    main.accountRangeLast = new QLineEdit("5000", main.accountRangeFrame);
    main.accountRangeLast->setEnabled(false);
    main.accountRangeLast->setMaxLength(6);
    main.accountRangeLast->installEventFilter(this);
                
    int maxSize = QFontMetrics(QFont(main.accountRangeFirst->font())).width("88888888");
    main.accountRangeFirst->setFixedWidth(maxSize);
    main.accountRangeLast->setFixedWidth(maxSize);
    
    main.accountRangeLayout->add(main.accountRangeRadio);
    main.accountRangeLayout->addSpacing(5);
    main.accountRangeLayout->add(main.accountRangeFirst);
    main.accountRangeLayout->add(main.accountRangeMiddleLabel);
    main.accountRangeLayout->add(main.accountRangeLast);
    main.accountRangeLayout->addStretch();
    
    main.accountRangeAllFrame = new QFrame(main.accountRangeVBox);
    
    main.accountRangeAllLayout = new QHBoxLayout(main.accountRangeAllFrame);
    main.accountRangeAllLayout->addSpacing(10);
    
    main.accountRangeAllRadio = new QRadioButton(main.accountRangeAllFrame);
    main.accountRangeAllRadio->setChecked(true);
    connect(main.accountRangeAllRadio, SIGNAL(clicked()), this, SLOT(allToggled()));
    main.accountRangeAllRadio->installEventFilter(this);
    
    main.accountRangeAllLabel = new QLabel("All Accounts", main.accountRangeAllFrame);
    
    main.accountRangeAllLayout->add(main.accountRangeAllRadio);
    main.accountRangeAllLayout->addSpacing(5);
    main.accountRangeAllLayout->add(main.accountRangeAllLabel);
    main.accountRangeAllLayout->addStretch();
    
    main.generalGroup->addSpace(10);
    
    main.generalDetailButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/generalDetailReport.png") ),
            "General Ledger Detail", main.generalGroup);
    main.generalDetailButton->installEventFilter(this);
    
    main.generalGroup->addSpace(5);
    
    main.generalTrialButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/generalTrialReport.png") ),
            "General Ledger Trial Balance", main.generalGroup);
    main.generalTrialButton->installEventFilter(this);
    
    main.vBoxLayout->add(main.generalGroup);
    
    
    main.chartGroup = new QVGroupBox("Account Reports", main.widget);
    main.chartGroup->setInsideMargin(30);
    
    main.vBoxLayout->add(main.chartGroup);
    
    
    main.chartAccountsButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/chartOfAccounts.png") ),
            "Chart of Accounts", main.chartGroup);
    main.chartAccountsButton->installEventFilter(this);
    
    main.adminGroup = new QVGroupBox("Administrative Options", main.widget);
    main.adminGroup->setInsideMargin(30);
    
    main.vBoxLayout->add(main.adminGroup);
    
    main.printerOptionsButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/printerOptionsButton.png") ),
            "Printer Options", main.adminGroup);
    main.printerOptionsButton->installEventFilter(this);
    
    main.vBoxLayout->addStretch();
    
    main.hBoxLayout->addStretch();
    
    addWidget(main.widget, 0);
    
    active = 0;
    
}

void ReportStack::rangeToggled()
{
    main.accountRangeRadio->setChecked(true);
    main.accountRangeAllRadio->setChecked(false);
    main.accountRangeFirst->setEnabled(true);
    main.accountRangeMiddleLabel->setEnabled(true);
    main.accountRangeLast->setEnabled(true);
}        
        
void ReportStack::allToggled()
{
    main.accountRangeAllRadio->setChecked(true);
    main.accountRangeRadio->setChecked(false);
    main.accountRangeFirst->setEnabled(false);
    main.accountRangeMiddleLabel->setEnabled(false);
    main.accountRangeLast->setEnabled(false);
}

void ReportStack::dbOpened()
{
    
}

void ReportStack::viewReport()
{
    if(id(report.widget) == 1)
    {
        removeWidget(report.widget);
        delete report.view;
        delete report.print;
        delete report.topLabel;
        delete report.labelLayout;
        delete report.vBoxLayout;
        delete report.widget;
    }
    
    report.widget = new QWidget(this);
    report.vBoxLayout = new QVBoxLayout(report.widget);
    report.vBoxLayout->setMargin(5);
    report.vBoxLayout->setSpacing(5);
    
    report.labelLayout = new QBoxLayout(report.vBoxLayout, QBoxLayout::LeftToRight);
    report.topLabel = new GoBackLabel(report.widget);
    connect(report.topLabel, SIGNAL(goBack()), this, SLOT(switchWidget()));
    
    report.print = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/reportPrint.png") ),
            "Print Report (F8)", report.widget);
    report.print->setFocusPolicy(QWidget::NoFocus);
    
    report.labelLayout->addWidget(report.topLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    report.labelLayout->addStretch();
    report.labelLayout->addWidget(report.print);
    
    report.view = new QTextEdit(report.widget);
    report.view->setTextFormat(QTextEdit::RichText);
    report.view->setReadOnly(true);
    
        
    report.vBoxLayout->addWidget(report.view);
    
    addWidget(report.widget, 1);

    switchWidget();
}

void ReportStack::switchWidget()
{
    if(active == 0) {
            raiseWidget(1);
            active = 1;
    }
    else {
        raiseWidget(0);
        active = 0;
    }
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

