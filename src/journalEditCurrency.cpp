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

#include <qtable.h>
#include <qlineedit.h>
#include <qwidget.h>

#include "journalEditCurrency.h"
#include "journalEditCurrencyValidator.h"
#include "journalData.h"

JournalEditCurrency::JournalEditCurrency(QTable *table, const Q_INT64 amount)
    : QTableItem(table, QTableItem::OnTyping)
{
    journalData = JournalData::instance();

    if(amount)
        setText(journalData->convertCurrency(amount));
}

QWidget* JournalEditCurrency::createEditor() const
{
    QLineEdit *lineEdit = new QLineEdit(text(), table()->viewport());
    lineEdit->setValidator(new JournalEditCurrencyValidator(lineEdit,
                            "journalEditCurrency::lineEditValidator"));
    lineEdit->selectAll();
    return lineEdit;
}

Q_INT64 JournalEditCurrency::value() const
{
    QString newText = text();
    newText = newText.remove('.');
    Q_INT64 amount = newText.toULongLong();
    return amount;
}

