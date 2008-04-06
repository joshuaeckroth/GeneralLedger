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
#include "saveAndClearDialog.h"

SaveAndClearDialog::SaveAndClearDialog(QWidget *parent, const char *name) : QDialog(parent, name)
{
    setCaption("Print Before Saving");

    vBoxLayout = new QVBoxLayout(this, 5, 5, "saveAndClearDialog::vBoxLayout");

    label = new QLabel("<nobr>You have not printed the Journal Entries.</nobr><br>"
                       "<nobr>Would you like to print them now?</nobr>",
		       this, "saveAndClearDialog::label");

    vBoxLayout->addWidget(label);
    vBoxLayout->addSpacing(10);

    hBoxLayout = new QHBoxLayout(vBoxLayout, 5, "saveAndClearDialog::hBoxLayout");

    saveButton = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/saveAndClear.png")),
                                 "Save Without Printing", this, "saveAndClearDialog::saveButton");
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveAndClear()));

    printButton = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/print.png")),
                                     "Print and Save", this, "saveAndClearDialog::printButton");
    connect(printButton, SIGNAL(clicked()), this, SLOT(printAndSave()));

    dontSaveButton = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/cancelButton.png")),
                                   "Don't Save and Clear", this, "saveAndClearDialog::dontSaveButton");
    connect(dontSaveButton, SIGNAL(clicked()), this, SLOT(dontSave()));

    dontSaveButton->setDefault(true);
    dontSaveButton->setFocus();

    hBoxLayout->addStretch();
    hBoxLayout->addWidget(saveButton);
    hBoxLayout->addWidget(printButton);
    hBoxLayout->addWidget(dontSaveButton);
}

void SaveAndClearDialog::saveAndClear()
{
    done(0);
}

void SaveAndClearDialog::printAndSave()
{
    done(1);
}

void SaveAndClearDialog::dontSave()
{
    done(2);
}


