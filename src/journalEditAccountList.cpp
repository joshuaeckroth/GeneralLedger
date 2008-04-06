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

#include <qcombobox.h>
#include <qstringlist.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qevent.h>

#include "journalEditAccountList.h"

JournalEditAccountList::JournalEditAccountList(QWidget *parent, const QStringList newAccounts, const QString newText)
    : QComboBox(TRUE, parent), accounts(newAccounts), curText(newText)
{
    list = new QListBox(this, "journalEditAccountList::list");
    setListBox(list);
    list->setSelectionMode(QListBox::NoSelection);
    list->setEnabled(false);

    editor = new QLineEdit(curText, this, "journalEditAccountList::editor");
    setLineEdit(editor);

    setSizeLimit(10);

    insertStringList(accounts);    // even though this is cleared on focusInEvent,
                                   // it's necessary to create a listbox that's the full size

    editor->setText(curText);
    
    setInsertionPolicy(QComboBox::NoInsertion);

    connect(editor, SIGNAL(textChanged(const QString&)), this, SLOT(changed(const QString&)));
    
}

void JournalEditAccountList::changed(const QString &newText)
{
    curText = newText;

    list->clear();

    for(QStringList::Iterator it = accounts.begin(); it != accounts.end(); it++)
    {
        if((*it).startsWith(curText))
            list->insertItem(*it);
    }

}

QString JournalEditAccountList::currentText() const
{
    return curText;
}

void JournalEditAccountList::focusInEvent(QFocusEvent *event)
{
    QComboBox::focusInEvent(event);
    popup();
    changed(curText);
}

void JournalEditAccountList::focusOutEvent(QFocusEvent *event)
{
    QComboBox::focusOutEvent(event);
    if(event->reason() == QFocusEvent::Mouse ||
       event->reason() == QFocusEvent::Tab ||
       event->reason() == QFocusEvent::Backtab)
        list->hide();
}

