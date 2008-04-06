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
#include <qstring.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include "reportsEditorDialog.h"
#include "accountsData.h"
#include "config.h"

ReportsEditorDialog::ReportsEditorDialog(QWidget *parent, const char *name,
                QString section)
    : QDialog(parent,name)
{
    accountsData = AccountsData::instance();
    QStringList accounts = accountsData->getFormattedList(true);

    setCaption("Add to " + section);
    
    vBoxLayout = new QVBoxLayout(this, 0, 5,
                    "reportsEditorDialog::vBoxLayout");
    vBoxLayout->setMargin(5);
    vBoxLayout->setSpacing(5);

    oneAccountRadio = new QRadioButton("One Account", this,
                    "reportsEditorDialog::oneAccount");
    connect(oneAccountRadio, SIGNAL(clicked()), this, SLOT(oneAccountToggled()));
    vBoxLayout->addWidget(oneAccountRadio);

    oneAccountHBoxLayout = new QHBoxLayout(vBoxLayout, 5,
                    "reportsEditorDialog::oneAccountHBoxLayout");

    oneAccountHBoxLayout->addSpacing(5);
    oneAccountLabel = new QLabel("Account:", this,
                    "reportsEditorDialog::oneAccountLabel");
    oneAccountHBoxLayout->addWidget(oneAccountLabel);

    oneAccountList = new QComboBox(this,
                    "reportsEditorDialog::oneAccountList");
    oneAccountList->insertStringList(accounts);
    oneAccountHBoxLayout->addWidget(oneAccountList);
    oneAccountHBoxLayout->addStretch();

    rangeAccountRadio = new QRadioButton("Range of Accounts", this,
                    "reportsEditorDialog::rangeAccountRadio");
    connect(rangeAccountRadio, SIGNAL(clicked()), this, SLOT(rangeAccountToggled()));
    vBoxLayout->addWidget(rangeAccountRadio);

    rangeAccountHBoxLayout = new QHBoxLayout(vBoxLayout, -1,
                    "reportsEditorDialog::rangeAccountHBoxLayout");
    rangeAccountHBoxLayout->addSpacing(5);

    rangeAccountListVBoxLayout = new QVBoxLayout(rangeAccountHBoxLayout, 5,
                    "reportsEditorDialog::rangeAccountListVBoxLayout");

    rangeBeginEndHBoxLayout = new QHBoxLayout(rangeAccountListVBoxLayout, 5,
                    "reportsEditorDialog::rangeBeginEndHBoxLayout");

    rangeBeginLabel = new QLabel("Begin:", this,
                    "reportsEditorDialog::rangeBeginLabel");
    rangeBeginEndHBoxLayout->addWidget(rangeBeginLabel);

    rangeBeginList = new QComboBox(this,
                    "reportsEditorDialog::rangeBeginList");
    rangeBeginList->insertStringList(accounts);
    rangeBeginEndHBoxLayout->addWidget(rangeBeginList);

    rangeBetweenLabel = new QLabel(" to ", this,
                    "reportsEditorDialog::rangeBetweenLabel");
    rangeBeginEndHBoxLayout->addWidget(rangeBetweenLabel);

    rangeEndList = new QComboBox(this,
                    "reportsEditorDialog::rangeEndList");
    rangeEndList->insertStringList(accounts);
    rangeBeginEndHBoxLayout->addWidget(rangeEndList);
    
    rangeBeginEndHBoxLayout->addStretch();

    rangeDescHBoxLayout = new QHBoxLayout(vBoxLayout, 5,
                    "reportsEditorDialog::rangeDescHBoxLayout");

    rangeDescLabel = new QLabel("Description:", this,
                    "reportsEditorDialog::rangeDescLabel");
    rangeDescHBoxLayout->addWidget(rangeDescLabel);

    rangeDescEdit = new QLineEdit(this,
                    "reportsEditorDialog::rangeDescEdit");
    rangeDescHBoxLayout->addWidget(rangeDescEdit);

    
    vBoxLayout->addSpacing(10);


    buttonHBoxLayout = new QHBoxLayout(vBoxLayout, 5,
                    "reportsEditorDialog::buttonHBoxLayout");
    buttonHBoxLayout->addStretch();

    buttonOK = new QPushButton(QIconSet(
                    QPixmap::fromMimeSource(ICON_PATH + "/addEntity.png")),
                    "Add", this);
    connect(buttonOK, SIGNAL(clicked()), this, SLOT(accept()));
    buttonHBoxLayout->addWidget(buttonOK);

    buttonCancel = new QPushButton(QIconSet(
                    QPixmap::fromMimeSource(ICON_PATH + "/cancelButton.png")),
                    "Cancel", this);
    connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    buttonHBoxLayout->addWidget(buttonCancel);

    oneAccountRadio->setChecked(true);
    oneAccountToggled();
    
}

QString ReportsEditorDialog::getAccountBegin() const
{
    QString accountBegin;
    
    if(oneAccountRadio->isChecked())
    {
        accountBegin = oneAccountList->currentText();
    }
    else
    {
        accountBegin = rangeBeginList->currentText();
    }

    accountBegin = accountBegin.left(accountBegin.find(" - "));

    return accountBegin;
}

QString ReportsEditorDialog::getAccountEnd() const
{
    QString accountEnd;

    if(rangeAccountRadio->isChecked())
    {
        accountEnd = rangeEndList->currentText();
        accountEnd = accountEnd.left(accountEnd.find(" - "));
    }
    else
    {
        accountEnd = "";
    }

    return accountEnd;
}

QString ReportsEditorDialog::getDesc() const
{
    QString desc;
    
    if(oneAccountRadio->isChecked())
    {
        desc = accountsData->getDescription(getAccountBegin());
    }
    else
    {
        desc = rangeDescEdit->text();
    }

    return desc;
}

void ReportsEditorDialog::oneAccountToggled()
{
    rangeAccountRadio->setChecked(false);
    rangeBeginLabel->setEnabled(false);
    rangeBeginList->setEnabled(false);
    rangeBetweenLabel->setEnabled(false);
    rangeEndList->setEnabled(false);
    rangeDescLabel->setEnabled(false);
    rangeDescEdit->setEnabled(false);

    oneAccountLabel->setEnabled(true);
    oneAccountList->setEnabled(true);
}

void ReportsEditorDialog::rangeAccountToggled()
{
    rangeBeginLabel->setEnabled(true);
    rangeBeginList->setEnabled(true);
    rangeBetweenLabel->setEnabled(true);
    rangeEndList->setEnabled(true);
    rangeDescLabel->setEnabled(true);
    rangeDescEdit->setEnabled(true);

    oneAccountRadio->setChecked(false);
    oneAccountLabel->setEnabled(false);
    oneAccountList->setEnabled(false);
}



