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
#include <qstring.h>
#include <qstringlist.h>
#include <qvgroupbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qlabel.h>
#include <qfiledialog.h>
#include <qevent.h>

#include "config.h"
#include "settings.h"
#include "accountsTab.h"
#include "accountsTable.h"
#include "accountsData.h"
#include "globalGoBackLabel.h"
#include "importExport.h"

AccountsTab::AccountsTab(QWidget *parent, const char *name)
    : QWidget(parent,name)
{
    accountsData = AccountsData::instance();

    main.vBoxLayout = new QVBoxLayout(this, 0, -1, "accountsTab::main.vBoxLayout");
    main.vBoxLayout->setMargin(5);
    main.vBoxLayout->setSpacing(5);
    
    main.labelLayout = new QHBoxLayout(main.vBoxLayout, -1, "accountsTab::main.labelLayout");
    main.topLabel = new GlobalGoBackLabel(this, "accountsTab::main.topLabel");
    connect(main.topLabel, SIGNAL(goBack()), this, SIGNAL(switchToHome()));
    main.labelLayout->addWidget(main.topLabel);
    main.labelLayout->addStretch();

    main.dataTable = new AccountsTable(this, "accountsTab::main.dataTable");
    connect(main.dataTable, SIGNAL(switchToHome()), this, SIGNAL(switchToHome()));
    connect(main.dataTable, SIGNAL(switchToJournal()), this, SIGNAL(switchToJournal()));
    connect(main.dataTable, SIGNAL(switchToReports()), this, SIGNAL(switchToReports()));
    connect(main.dataTable, SIGNAL(switchToHelp()), this, SIGNAL(switchToHelp()));
    main.dataTable->installEventFilter(this);
   
    main.vBoxLayout->addWidget(main.dataTable);
    
    main.bottomWidget = new QWidget(this, "accountsTab::main.bottomWidget");
    main.bottomHBoxLayout = new QHBoxLayout(main.bottomWidget, 0, -1, "accountsTab::main.bottomHBoxLayout");
    
    main.bottomLabel = new QLabel(
            "<nobr><b>Keyboard Control:</b></nobr><br>"
            "<nobr> &nbsp; &nbsp; <b>Enter</b>: Edit current field</nobr><br>"
            "<nobr> &nbsp; &nbsp; <b>Insert</b> / <b>Delete</b>: Create new account / "
            "Delete selected account</nobr>",
            main.bottomWidget, "accountsTab::main.bottomLabel");
    
    main.bottomRightFrame = new QFrame(main.bottomWidget, "accountsTab::main.bottomRightFrame");
    main.bottomRightBoxLayout = new QBoxLayout(main.bottomRightFrame, QBoxLayout::LeftToRight,
                                               0, -1, "accountsTab::main.bottomRightBoxLayout");
    main.bottomRightBoxLayout->setSpacing(5);
    
    main.importButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/import.png") ),
            "Import from CSV (F11)", main.bottomRightFrame, "accountsTab::main.importButton");
    main.importButton->setFocusPolicy(QWidget::NoFocus);
    connect(main.importButton, SIGNAL(clicked()), this, SLOT(importCSV()));
    
    main.exportButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/export.png") ),
            "Export to CSV (F12)", main.bottomRightFrame, "accountsTab::main.exportButton");
    main.exportButton->setFocusPolicy(QWidget::NoFocus);
    connect(main.exportButton, SIGNAL(clicked()), this, SLOT(exportCSV()));
    
    main.bottomRightBoxLayout->addWidget(main.importButton, 0, Qt::AlignLeft | Qt::AlignTop);
    main.bottomRightBoxLayout->addWidget(main.exportButton, 0, Qt::AlignLeft | Qt::AlignTop);
    
    main.bottomHBoxLayout->addWidget(main.bottomLabel);
    main.bottomHBoxLayout->addStretch();
    main.bottomHBoxLayout->addWidget(main.bottomRightFrame);
    
    main.vBoxLayout->addWidget(main.bottomWidget);
}

void AccountsTab::importCSV()
{
    if(ImportExport::importCSV(ImportExport::Accounts))
        main.dataTable->populate();
}

void AccountsTab::exportCSV()
{
    ImportExport::exportCSV(ImportExport::Accounts);
}

void AccountsTab::chooseREAccount()
{
}

bool AccountsTab::eventFilter(QObject *target, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = (QKeyEvent*)event;
        if(keyEvent->key() == Key_F8
           && accountsData->getREAccountId() == QString(""))
        {
            chooseREAccount();
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

void AccountsTab::showEvent(QShowEvent*)
{
    main.dataTable->setFocus();
}

