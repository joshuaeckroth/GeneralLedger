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
#include "journalTab.h"
#include "journalData.h"
#include "journalTable.h"
#include "journalSummary.h"
#include "globalGoBackLabel.h"
#include "importExport.h"
#include "reportsGenerator.h"
#include "saveAndClearDialog.h"

JournalTab::JournalTab(QWidget *parent, const char *name)
    : QWidget(parent,name)
{
    journalData = JournalData::instance();

    printed = false;
    
    main.vBoxLayout = new QVBoxLayout(this, 0, -1, "journalTab::main.vBoxLayout");
    main.vBoxLayout->setMargin(5);
    
    main.labelLayout = new QBoxLayout(main.vBoxLayout, QBoxLayout::LeftToRight);
    
    main.topLabel = new GlobalGoBackLabel(this, "journalTab::main.topLabel");
    connect(main.topLabel, SIGNAL(goBack()), this, SIGNAL(switchToHome()));
    
    main.saveAndClear = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/saveAndClear.png") ),
            "Save and Clear (F8)", this, "journalTab::main.saveAndClear");
    main.saveAndClear->setFocusPolicy(QWidget::NoFocus);
    connect(main.saveAndClear, SIGNAL(clicked()), this, SLOT(saveAndClear()));
    
    main.print = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/print.png") ),
            "Print Journal Entries (F9)", this, "journalTab::main.print");
    main.print->setFocusPolicy(QWidget::NoFocus);
    connect(main.print, SIGNAL(clicked()), this, SLOT(printJournal()));
    
    main.labelLayout->addWidget(main.topLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    main.labelLayout->addStretch();
    main.labelLayout->addWidget(main.saveAndClear);
    main.labelLayout->addSpacing(5);
    main.labelLayout->addWidget(main.print);
    
    main.dataTable = new JournalTable(this, "journalTab::main.dataTable");
    connect(main.dataTable, SIGNAL(switchToHome()), this, SIGNAL(switchToHome()));
    connect(main.dataTable, SIGNAL(switchToAccounts()), this, SIGNAL(switchToAccounts()));
    connect(main.dataTable, SIGNAL(switchToReports()), this, SIGNAL(switchToReports()));
    connect(main.dataTable, SIGNAL(switchToHelp()), this, SIGNAL(switchToHelp()));
    main.dataTable->installEventFilter(this);
    
    main.vBoxLayout->addSpacing(5);
    main.vBoxLayout->addWidget(main.dataTable);
    
    main.summary = new JournalSummary(this, "journalTab::main.summary", main.dataTable);
    main.vBoxLayout->addWidget(main.summary);
    
    main.hr = new QLabel("<hr>", this, "journalTab::main.hr");
    main.vBoxLayout->addWidget(main.hr);
    
    
    main.bottomWidget = new QWidget(this, "journalTab::main.bottomWidget");
    main.bottomHBoxLayout = new QHBoxLayout(main.bottomWidget, 0, -1, "journalTab::main.bottomHBoxLayout");
    
    main.bottomLabel = new QLabel(
            "<nobr><b>Keyboard Control:</b></nobr><br>"
            "<nobr> &nbsp; &nbsp; <b>Enter</b>: Edit current field, move to next field</nobr><br>"
            "<nobr> &nbsp; &nbsp; <b>Insert</b> / <b>Delete</b>: Create new entry / Delete selected entry</nobr>",
    main.bottomWidget, "journalTab::main.bottomLabel");
    
    main.bottomRightFrame = new QFrame(main.bottomWidget, "journalTab::main.bottomRightFrame");
    main.bottomRightBoxLayout = new QBoxLayout(main.bottomRightFrame, QBoxLayout::LeftToRight, 0, -1,
                                               "journalTab::main.bottomRightBoxLayout");
    main.bottomRightBoxLayout->setSpacing(5);
    
    main.importButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/import.png") ),
            "Import from CSV (F11)", main.bottomRightFrame, "journalTab::main.importButton");
    main.importButton->setFocusPolicy(QWidget::NoFocus);
    connect(main.importButton, SIGNAL(clicked()), this, SLOT(importCSV()));
    
    main.exportButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/export.png") ),
            "Export to CSV (F12)", main.bottomRightFrame, "journalTab::main.exportButton");
    main.exportButton->setFocusPolicy(QWidget::NoFocus);
    connect(main.exportButton, SIGNAL(clicked()), this, SLOT(exportCSV()));
    
    main.bottomRightBoxLayout->addWidget(main.importButton, 0, Qt::AlignLeft | Qt::AlignTop);
    main.bottomRightBoxLayout->addWidget(main.exportButton, 0, Qt::AlignLeft | Qt::AlignTop);
    
    main.bottomHBoxLayout->addWidget(main.bottomLabel);
    main.bottomHBoxLayout->addStretch();
    main.bottomHBoxLayout->addWidget(main.bottomRightFrame);
    
    main.vBoxLayout->addWidget(main.bottomWidget);
}

void JournalTab::importCSV()
{
    if(ImportExport::importCSV(ImportExport::JournalTmp))
        main.dataTable->populate();
}

void JournalTab::exportCSV()
{
    if(journalData->journalTmpEmpty())
        return;

    ImportExport::exportCSV(ImportExport::JournalTmp);
}

bool JournalTab::eventFilter(QObject *target, QEvent *event)
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

void JournalTab::saveAndClear()
{
    if(journalData->journalTmpEmpty())
        return;

    if(!printed)
    {
	SaveAndClearDialog dialog(this, "journalTab::saveAndClear()::dialog");
	dialog.exec();

	switch(dialog.result())
	{
	    case 0:
		journalData->saveAndClear();
		main.dataTable->unpopulate();
		break;
	    case 1:
		printJournal();
		journalData->saveAndClear();
		main.dataTable->unpopulate();
		break;
	    default:
		break;
	}
    }
    else
    {
	journalData->saveAndClear();
	main.dataTable->unpopulate();
    }
}

void JournalTab::printJournal()
{
    if(journalData->journalTmpEmpty())
        return;

    ReportsGenerator *reportsGenerator = ReportsGenerator::instance();
    if(reportsGenerator->printJournalEntryReport())
	printed = true;
}

void JournalTab::showEvent(QShowEvent*)
{
    main.dataTable->setFocus();
}

