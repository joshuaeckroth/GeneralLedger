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
#include <qlayout.h>
#include <qlabel.h>

#include "journalSummary.h"
#include "journalTable.h"
#include "journalData.h"
#include "client.h"

JournalSummary::JournalSummary(QWidget *parent, const char *name, JournalTable *table)
    : QWidget(parent,name)
{
    journalData = JournalData::instance();
    connect(journalData, SIGNAL(journalTmpChanged()), this, SLOT(updateSummary()));

    client = Client::instance();
    connect(client, SIGNAL(clientOpened(QString)), this, SLOT(updateSummary()));
    
    int debitColWidth = 100;
    int creditColWidth = 100;
    int scrollBarWidth = (table->verticalScrollBar()->width() + 5);
    
    grid = new QGridLayout(this, 3, 5, 0, -1, "journalSummary::grid");
    
    debitLabel = new QLabel("Debit:", this, "journalSummary::debitLabel");
    debitLabel->setAlignment(Qt::AlignRight);
    
    debitValue = new QLabel("<tt></tt>", this,
                    "journalSummary::debitValue");
    debitValue->setFixedWidth(debitColWidth);
    debitValue->setAlignment(Qt::AlignRight);
    
    creditLabel = new QLabel("Credit:", this, "journalSummary::creditLabel");
    creditLabel->setAlignment(Qt::AlignRight);
    
    creditValue = new QLabel("<tt></tt>", this,
                    "journalSummary::creditValue");
    creditValue->setFixedWidth(creditColWidth);
    creditValue->setAlignment(Qt::AlignRight);
    
    balanceLabel = new QLabel(tr("<b>Out of Balance:</b>"), this,
		    "journalSummary::balanceLabel");
    balanceLabel->setAlignment(Qt::AlignRight);
    
    balanceValue = new QLabel("<b><tt></tt></b>", this,
                    "journalSummary::balanceValue");
    balanceValue->setAlignment(Qt::AlignRight);
    
    debitWidth = new QSpacerItem(debitColWidth, 1);
    creditWidth = new QSpacerItem(creditColWidth, 1);
    scrollWidth = new QSpacerItem(scrollBarWidth, 1);
    
    grid->setColStretch(0, 1);
    
    grid->addWidget(debitLabel, 0, 1);
    grid->addWidget(debitValue, 0, 2);
    grid->addItem(creditWidth, 0, 3);
    
    grid->addItem(scrollWidth, 0, 4);
    
    grid->addWidget(creditLabel, 1, 1);
    grid->addItem(debitWidth, 1, 2);
    grid->addWidget(creditValue, 1, 3);
    
    grid->addWidget(balanceLabel, 2, 1);
    grid->addItem(debitWidth, 2, 2);
    grid->addWidget(balanceValue, 2, 3);
    
}

JournalSummary::~JournalSummary()
{
    grid->removeItem(scrollWidth);
    delete scrollWidth;
    grid->removeItem(creditWidth);
    delete creditWidth;
    grid->removeItem(debitWidth);
    delete debitWidth;
    grid->remove(balanceValue);
    delete balanceValue;
    grid->remove(balanceLabel);
    delete balanceLabel;
    grid->remove(creditValue);
    delete creditValue;
    grid->remove(creditLabel);
    delete creditLabel;
    grid->remove(debitValue);
    delete debitValue;
    grid->remove(debitLabel);
    delete debitLabel;
    delete grid;
}

void JournalSummary::updateSummary()
{
    debitValue->setText("<tt>" + journalData->journalTmpDebit() + "</tt>");
    creditValue->setText("<tt>" + journalData->journalTmpCredit() + "</tt>");
    balanceValue->setText("<b><tt>" + journalData->journalTmpBalance() + "</tt></b>");
}

