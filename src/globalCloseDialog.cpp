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

#include <qdialog.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include "config.h"
#include "globalCloseDialog.h"

GlobalCloseDialog::GlobalCloseDialog(QWidget *parent, const char *name) : QDialog(parent, name)
{
    setCaption("Close Client");

    vBoxLayout = new QVBoxLayout(this, 5, 5, "globalCloseDialog::vBoxLayout");

    label = new QLabel("<nobr>There are entries in the journal which have not been saved and cleared.</nobr><br>"
                       "<nobr>Would you like to save and clear these entries now?</nobr><br><br>"
                       "Note: if you do not save and clear these entries, "
                       "they will <u>not</u> be lost.  You will find them "
                       "in the same place when you open this client again.", this, "globalCloseDialog::label");

    vBoxLayout->addWidget(label);
    vBoxLayout->addSpacing(10);

    hBoxLayout = new QHBoxLayout(vBoxLayout, 5, "globalCloseDialog::hBoxLayout");

    saveButton = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/saveAndClear.png")),
                                 "Save and Clear Entries", this, "globalCloseDialog::saveButton");
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveAndClear()));

    dontSaveButton = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/cancelButton.png")),
                                     "Don't Save and Clear", this, "globalCloseDialog::dontSaveButton");
    connect(dontSaveButton, SIGNAL(clicked()), this, SLOT(dontSave()));

    returnButton = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/returnToJournal.png")),
                                   "Return to Journal Entry", this, "globalCloseDialog::returnButton");
    connect(returnButton, SIGNAL(clicked()), this, SLOT(retToEntry()));

    returnButton->setDefault(true);
    returnButton->setFocus();

    hBoxLayout->addStretch();
    hBoxLayout->addWidget(saveButton);
    hBoxLayout->addWidget(dontSaveButton);
    hBoxLayout->addWidget(returnButton);
}

void GlobalCloseDialog::saveAndClear()
{
    done(0);
}

void GlobalCloseDialog::dontSave()
{
    done(1);
}

void GlobalCloseDialog::retToEntry()
{
    done(2);
}


