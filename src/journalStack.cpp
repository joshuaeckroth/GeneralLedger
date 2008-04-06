#include <qwidget.h>
#include <qvgroupbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qfiledialog.h>
#include <qevent.h>
#include <qframe.h>
#include <qdialog.h>

#include "config.h"
#include "database.h"
#include "settings.h"
#include "printer.h"
#include "journalStack.h"
#include "journalTable.h"
#include "journalSummary.h"
#include "goBackLabel.h"

JournalStack::JournalStack(QWidget *parent, const char *name)
    : QWidget(parent,name)
{
    db = Database::instance();
    printer = Printer::instance();
    settings = Settings::instance();

    exportPath = settings->getExportPath();
    importPath = settings->getImportPath();
    
    active = false;
    printed = false;
}

JournalStack::~JournalStack()
{}

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
        delete main.saveAndClear;
        delete main.print;
        delete main.topLabel;
        delete main.labelLayout;
        delete main.vBoxLayout;
    }
    
    main.vBoxLayout = new QVBoxLayout(this);
    main.vBoxLayout->setMargin(5);
    
    main.labelLayout = new QBoxLayout(main.vBoxLayout, QBoxLayout::LeftToRight);
    
    main.topLabel = new GoBackLabel(this);
    connect(main.topLabel, SIGNAL(goBack()), this, SIGNAL(goToMain()));
    
    main.saveAndClear = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/saveAndClear.png") ),
            "Save and Clear (F8)", this);
    main.saveAndClear->setFocusPolicy(QWidget::NoFocus);
    connect(main.saveAndClear, SIGNAL(clicked()), this, SLOT(saveAndClear()));
    
    main.print = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/print.png") ),
            "Print Journal Entries (F9)", this);
    main.print->setFocusPolicy(QWidget::NoFocus);
    connect(main.print, SIGNAL(clicked()), this, SLOT(printJournal()));
    
    main.labelLayout->addWidget(main.topLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    main.labelLayout->addStretch();
    main.labelLayout->addWidget(main.saveAndClear);
    main.labelLayout->addSpacing(5);
    main.labelLayout->addWidget(main.print);
    
    main.dataTable = new JournalTable(this);
    connect(main.dataTable, SIGNAL(goToMain()), this, SIGNAL(goToMain()));
    connect(main.dataTable, SIGNAL(goToAccounts()), this, SIGNAL(goToAccounts()));
    connect(main.dataTable, SIGNAL(goToReports()), this, SIGNAL(goToReports()));
    connect(main.dataTable, SIGNAL(goToHelp()), this, SIGNAL(goToHelp()));
    main.dataTable->installEventFilter(this);
    
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
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/import.png") ),
            "Import from CSV (F11)", main.bottomRightFrame);
    main.importButton->setFocusPolicy(QWidget::NoFocus);
    connect(main.importButton, SIGNAL(clicked()), this, SLOT(importCSV()));
    
    main.exportButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/export.png") ),
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

void JournalStack::importCSV()
{
    QString file = QFileDialog::getOpenFileName(
            importPath,
            "Comma Separated Values (*.csv)",
            this,
            0,
            "Import Journal Entries");
    
    if(file != "")
    {
        db->importCSV("journalTmp", file);
        main.dataTable->populate();
    }
}

void JournalStack::exportCSV()
{
    if(db->journalTmpEmpty())
        return;

    QString file = QFileDialog::getSaveFileName(
            exportPath,
            "Comma Separated Values (*.csv)",
            this,
            0,
            "Export Journal Entries");

    if(file != "")
        db->exportCSV("journalTmp", file);
}

bool JournalStack::eventFilter(QObject *target, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = (QKeyEvent*)event;
        if(keyEvent->key() == Key_F8)
        {
            saveAndClear();
            return true;
        }
        if(keyEvent->key() == Key_F9)
        {
            printJournal();
            return true;
        }
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

void JournalStack::saveAndClear()
{
    if(db->journalTmpEmpty())
        return;

    if(!printed)
    {
        QDialog dialog;
        dialog.setCaption("Print Before Closing");

        QVBoxLayout vBoxLayout(&dialog);
        vBoxLayout.setMargin(5);
        vBoxLayout.setSpacing(5);

        QLabel label("<nobr>You have not printed the Journal Entries.</nobr><br>"
                "<nobr>Would you like to print them now?</nobr>", &dialog);
        vBoxLayout.addWidget(&label);

        vBoxLayout.addSpacing(10);

        QHBoxLayout hBoxLayout(&vBoxLayout);
        hBoxLayout.setSpacing(5);

        QPushButton noPrintButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/saveAndClear.png")),
                                  "Save Without Printing", &dialog);
        connect(&noPrintButton, SIGNAL(clicked()), &dialog, SLOT(reject()));
        noPrintButton.setDefault(false);

        QPushButton printButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/print.png")),
                                "Print and Save", &dialog);
        connect(&printButton, SIGNAL(clicked()), &dialog, SLOT(accept()));
        printButton.setDefault(true);

        hBoxLayout.addStretch();
        hBoxLayout.addWidget(&noPrintButton);
        hBoxLayout.addWidget(&printButton);

        if(dialog.exec())
            printJournal();

    }
    db->commitJournalTmp();
    main.dataTable->clearTable();
}

void JournalStack::printJournal()
{
    if(db->journalTmpEmpty())
        return;

    db->getJournalTmpReport();
    printer->printReport(db->journalTmpReportHeader(), db->journalTmpReportData());
}

void JournalStack::showEvent(QShowEvent*)
{
    main.dataTable->setFocus();
}

