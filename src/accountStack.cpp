#include <typeinfo>

#include <qwidget.h>
#include <qvgroupbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qlabel.h>
#include <qfiledialog.h>
#include <qevent.h>

#include "accountStack.h"
#include "accountTable.h"
#include "goBackLabel.h"
#include "database.h"

AccountStack::AccountStack(QWidget *parent, const char *name)
    : QWidget(parent,name)
{
    db = new Database;
    active = false;
}

AccountStack::~AccountStack()
{
    delete db;
}

void AccountStack::dbOpened()
{
    if(active)
    {
        delete main.exportButton;
        delete main.importButton;
        delete main.bottomRightBoxLayout;
        delete main.bottomRightFrame;
        delete main.bottomLabel;
        delete main.bottomHBoxLayout;
        delete main.bottomWidget;
        delete main.dataTable;
        delete main.topLabel;
        delete main.labelLayout;
        delete main.vBoxLayout;
    }
    
    main.vBoxLayout = new QVBoxLayout(this);
    main.vBoxLayout->setMargin(5);
    main.vBoxLayout->setSpacing(5);
    
    main.labelLayout = new QHBoxLayout(main.vBoxLayout);
    main.topLabel = new GoBackLabel(this);
    connect(main.topLabel, SIGNAL(goBack()), this, SIGNAL(goToMain()));
    main.labelLayout->addWidget(main.topLabel);
    main.labelLayout->addStretch();
    
    main.dataTable = new AccountTable(this);
    connect(main.dataTable, SIGNAL(goToMain()), this, SIGNAL(goToMain()));
    connect(main.dataTable, SIGNAL(goToJournal()), this, SIGNAL(goToJournal()));
    connect(main.dataTable, SIGNAL(goToReports()), this, SIGNAL(goToReports()));
    connect(main.dataTable, SIGNAL(goToHelp()), this, SIGNAL(goToHelp()));
    main.dataTable->installEventFilter(this);
    
    main.vBoxLayout->addWidget(main.dataTable);
    
    main.bottomWidget = new QWidget(this);
    main.bottomHBoxLayout = new QHBoxLayout(main.bottomWidget);
    
    main.bottomLabel = new QLabel(
            "<nobr><b>Keyboard Control:</b></nobr><br>"
            "<nobr> &nbsp; &nbsp; <b>Enter</b>: Edit current field, move to next field</nobr><br>"
            "<nobr> &nbsp; &nbsp; <b>Insert</b> / <b>Delete</b>: Create new entry / Delete selected entry</nobr>",
            main.bottomWidget);
    
    main.bottomRightFrame = new QFrame(main.bottomWidget);
    main.bottomRightBoxLayout = new QBoxLayout(main.bottomRightFrame, QBoxLayout::LeftToRight);
    main.bottomRightBoxLayout->setSpacing(5);
    
    main.importButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/import.png") ),
            "Import from CSV (F11)", main.bottomRightFrame);
    main.importButton->setFocusPolicy(QWidget::NoFocus);
    connect(main.importButton, SIGNAL(clicked()), this, SLOT(importCSV()));
    
    main.exportButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/export.png") ),
            "Export to CSV (F12)", main.bottomRightFrame);
    main.exportButton->setFocusPolicy(QWidget::NoFocus);
    connect(main.exportButton, SIGNAL(clicked()), this, SLOT(exportCSV()));
    
    main.bottomRightBoxLayout->addWidget(main.importButton, 0, Qt::AlignLeft | Qt::AlignTop);
    main.bottomRightBoxLayout->addWidget(main.exportButton, 0, Qt::AlignLeft | Qt::AlignTop);
    
    main.bottomHBoxLayout->addWidget(main.bottomLabel);
    main.bottomHBoxLayout->addStretch();
    main.bottomHBoxLayout->addWidget(main.bottomRightFrame);
    
    main.vBoxLayout->addWidget(main.bottomWidget);
    
    active = true;
}

void AccountStack::importCSV()
{
    QString file = QFileDialog::getOpenFileName(
            ".",
            "Comma Separated Values (*.csv)",
            this,
            0,
            "Import Accounts");
    
    if(file != "")
    {
        db->importCSV("accounts", file);
        main.dataTable->populate();
    }
}

void AccountStack::exportCSV()
{
    QString file = QFileDialog::getSaveFileName(
            ".",
            "Comma Separated Values (*.csv)",
            this,
            0,
            "Export Accounts");

    if(file != "")
        db->exportCSV("accounts", file);
}

bool AccountStack::eventFilter(QObject *target, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = (QKeyEvent*)event;
        if(keyEvent->key() == Key_F11)
        {
            importCSV();
            return true;
        }
        if(keyEvent->key() == Key_F12)
        {
            exportCSV();
            return true;
        }
    }
    
    return QWidget::eventFilter(target,event);
}  

void AccountStack::showEvent(QShowEvent*)
{
    main.dataTable->setFocus();
}

