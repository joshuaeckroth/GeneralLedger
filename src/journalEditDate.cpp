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
#include <qdatetime.h>

#include "journalEditDate.h"

JournalEditDate::JournalEditDate(QTable *table, const QDate newDate)
    : QTableItem(table, QTableItem::OnTyping), recordDate(newDate)
{
    if(recordDate.isValid())
        setText(recordDate.toString());
}

QWidget* JournalEditDate::createEditor() const
{
    QLineEdit *lineEdit = new QLineEdit(table()->viewport());
    lineEdit->setInputMask("99/99/99");
    lineEdit->setText(text());
    lineEdit->selectAll();
    return lineEdit;
}

QString JournalEditDate::text() const
{
    if(recordDate.isValid())
        return recordDate.toString("MM/dd/yy");
    else
        return QString::null;
}

void JournalEditDate::setContentFromEditor(QWidget *widget)
{
    QString content = ((QLineEdit*)(widget))->text();
    QString month = content.left(2);
    QString day = content.mid(3, 2);
    QString year = content.right(2);

    if(year.toInt() > (QDate::currentDate().year() + 5))
        recordDate.setYMD(year.toInt() + 1900, month.toInt(), day.toInt());
    else
        recordDate.setYMD(year.toInt() + 2000, month.toInt(), day.toInt());
}

QDate JournalEditDate::date() const
{
    return recordDate;
}


