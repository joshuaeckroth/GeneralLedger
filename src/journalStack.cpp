#include <qwidget.h>
#include <qvgroupbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qfiledialog.h>
#include <qevent.h>
#include <qframe.h>

#include "journalStack.h"
#include "journalTable.h"
#include "journalSummary.h"
#include "goBackLabel.h"
#include "database.h"

JournalStack::JournalStack(QWidget *parent, const char *name)
    : QWidget(parent,name)
{
    db = new Database;
    active = false;
}

JournalStack::~JournalStack()
{
    delete db;
    /*
    if(active)
    {
        delete main.exportButton;
        delete main.importButton;
        delete main.bottomRightBoxLayout;
        delete main.bottomRightFrame;
        delete main.bottomLabel;
        delete main.bottomHBoxLayout;
        delete main.bottomWidget;
        delete main.summary;
        delete main.hr;
        delete main.dataTable;
        delete main.saveAndClose;
        delete main.print;
        delete main.topLabel;
        delete main.labelLayout;
        delete main.vBoxLayout;
    }
    */
}

void JournalStack::dbOpened()
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
        delete main.hr;
        delete main.summary;
        delete main.dataTable;
        delete main.saveAndClose;
        delete main.print;
        delete main.topLabel;
        delete main.labelLayout;
        delete main.vBoxLayout;
    }
    
    main.vBoxLayout = new QVBoxLayout(this);
    main.vBoxLayout->setMargin(5);
    
    main.labelLayout = new QBoxLayout(main.vBoxLayout, QBoxLayout::LeftToRight);
    
    main.topLabel = new GoBackLabel(this);
    connect(main.topLabel, SIGNAL(goBack()), this, SIGNAL(goBack()));
    
    main.saveAndClose = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/saveAndClear.png") ),
            "Save and Clear (F8)", this);
    main.saveAndClose->setFocusPolicy(QWidget::NoFocus);
    connect(main.saveAndClose, SIGNAL(clicked()), this, SLOT(saveAndClose()));
    
    main.print = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/print.png") ),
            "Print Journal Entries (F9)", this);
    main.print->setFocusPolicy(QWidget::NoFocus);
    connect(main.print, SIGNAL(clicked()), this, SLOT(printJournal()));
    
    main.labelLayout->addWidget(main.topLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    main.labelLayout->addStretch();
    main.labelLayout->addWidget(main.saveAndClose);
    main.labelLayout->addWidget(main.print);
    
    main.dataTable = new JournalTable(this);
    main.dataTable->setFocus();
    
    main.vBoxLayout->addSpacing(5);
    main.vBoxLayout->addWidget(main.dataTable);
    
    main.summary = new JournalSummary(this, 0, main.dataTable);
    main.vBoxLayout->addWidget(main.summary);
    
    main.hr = new QLabel("<hr>", this);
    main.vBoxLayout->addWidget(main.hr);
    
    
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
    
    main.exportButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/export.png") ),
            "Export to CSV (F12)", main.bottomRightFrame);
    
    main.bottomRightBoxLayout->addWidget(main.importButton, 0, Qt::AlignLeft | Qt::AlignTop);
    main.bottomRightBoxLayout->addWidget(main.exportButton, 0, Qt::AlignLeft | Qt::AlignTop);
    
    main.bottomHBoxLayout->addWidget(main.bottomLabel);
    main.bottomHBoxLayout->addStretch();
    main.bottomHBoxLayout->addWidget(main.bottomRightFrame);
    
    main.vBoxLayout->addWidget(main.bottomWidget);
    
    active = true;
}

void JournalStack::importCSV()
{
    QString file = QFileDialog::getOpenFileName(
            ".",
            "Comma Separated Values (*.csv)",
            this,
            0,
            "Import Journal Entries");
    
    db->importCSV("journal", file);
}

void JournalStack::exportCSV()
{
    QString file = QFileDialog::getSaveFileName(
            ".",
            "Comma Separated Values (*.csv)",
            this,
            0,
            "Export Journal Entries");

    db->exportCSV("journal", file);    
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
    
    return QWidget::eventFilter(target,event);
}

void JournalStack::saveAndClose()
{
    
}

void JournalStack::printJournal()
{
    
}

