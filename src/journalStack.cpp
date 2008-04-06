#include <qwidgetstack.h>
#include <qwidget.h>
#include <qvgroupbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qfiledialog.h>
#include <qevent.h>

#include "journalStack.h"
#include "journalTable.h"
#include "journalSummary.h"
#include "goBackLabel.h"

JournalStack::JournalStack(QWidget *parent, const char *name)
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
    
    main.vBoxLayout->addStretch();
    
    main.taskGroup = new QVGroupBox("Journal Entry Tasks", main.widget);
    main.taskGroup->setInsideMargin(30);
    main.taskGroup->setInsideSpacing(20);
    
    main.vBoxLayout->add(main.taskGroup);
    
    main.editButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/editJournalButton.png") ),
            "Edit / View Journal Entries", main.taskGroup);
    connect(main.editButton, SIGNAL(clicked()), this, SLOT(switchWidget()));
    main.editButton->setDefault(true);
    main.editButton->setFocus();
    main.editButton->installEventFilter(this);
    
    main.vBoxLayout->addSpacing(30);
    
    main.adminGroup = new QVGroupBox("Administrative Options", main.widget);
    main.adminGroup->setInsideMargin(30);
    main.adminGroup->setInsideSpacing(20);
    
    main.vBoxLayout->add(main.adminGroup);
    
    main.importButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/importJournalButton.png") ),
            "Import Journal Entries (CSV format)", main.adminGroup);
    connect(main.importButton, SIGNAL(clicked()), this, SLOT(importCSV()));
    main.importButton->installEventFilter(this);
    
    main.exportButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/exportJournalButton.png") ),
            "Export Journal Entries (CSV format)", main.adminGroup);
    connect(main.exportButton, SIGNAL(clicked()), this, SLOT(exportCSV()));
    main.exportButton->installEventFilter(this);
    
    main.vBoxLayout->addStretch();
    
    main.hBoxLayout->addStretch();
    
    addWidget(main.widget, 0);
    
    active = 0;
}

void JournalStack::dbOpened()
{
    if(id(table.widget) == 1)
    {
        removeWidget(table.widget);
        delete table.summary;
        delete table.dataTable;
        delete table.closePeriod;
        delete table.topLabel;
        delete table.labelLayout;
        delete table.vBoxLayout;
        delete table.widget;
    }
    
    table.widget = new QWidget(this);
    table.vBoxLayout = new QVBoxLayout(table.widget);
    table.vBoxLayout->setMargin(5);
    table.vBoxLayout->setSpacing(5);
    
    table.labelLayout = new QBoxLayout(table.vBoxLayout, QBoxLayout::LeftToRight);
    
    table.topLabel = new GoBackLabel(table.widget);
    connect(table.topLabel, SIGNAL(goBack()), this, SLOT(switchWidget()));
    
    table.closePeriod = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/closePeriod.png") ),
            "Close Period (F8)", table.widget);
    table.closePeriod->setFocusPolicy(QWidget::NoFocus);
    
    table.labelLayout->addWidget(table.topLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    table.labelLayout->addStretch();
    table.labelLayout->addWidget(table.closePeriod);
    
    table.dataTable = new JournalTable(table.widget);
    table.dataTable->setFocus();
    
    table.vBoxLayout->addWidget(table.dataTable);
    
    table.summary = new JournalSummary(table.widget, 0, table.dataTable);
    table.vBoxLayout->addWidget(table.summary);
    
    addWidget(table.widget, 1);
}

void JournalStack::importCSV()
{
    QString file = QFileDialog::getOpenFileName(
            ".",
            "Comma Separated Values (*.csv)",
            this,
            0,
            "Import Journal Entries");
}

void JournalStack::exportCSV()
{
    QString file = QFileDialog::getSaveFileName(
            ".",
            "Comma Separated Values (*.csv)",
            this,
            0,
            "Export Journal Entries");
    
    qDebug(file);
}

void JournalStack::switchWidget()
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

bool JournalStack::eventFilter(QObject *target, QEvent *event)
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

