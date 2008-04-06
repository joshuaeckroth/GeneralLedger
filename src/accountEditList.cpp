#include <qcombobox.h>
#include <qstringlist.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qevent.h>

#include "accountEditList.h"

AccountEditList::AccountEditList(QWidget *parent, const char *name,
                                const QStringList &newAccounts, const QString &newText)
    : QComboBox(TRUE, parent, name), accounts(newAccounts), curText(newText)
{
    list = new QListBox(this);
    setListBox(list);
    list->setSelectionMode(QListBox::NoSelection);
    list->setEnabled(false);

    editor = new QLineEdit(curText, this);
    setLineEdit(editor);

    setSizeLimit(10);

    insertStringList(accounts);    // even though this is cleared on focusInEvent,
                                   // it's necessary to create a listbox that's the full size

    editor->setText(curText);
    
    setInsertionPolicy(QComboBox::NoInsertion);

    connect(editor, SIGNAL(textChanged(const QString&)), this, SLOT(changed(const QString&)));
    
}

void AccountEditList::updateAccounts(const QStringList &newAccounts)
{
    accounts = newAccounts;
    list->clear();
    list->insertStringList(accounts);
}

void AccountEditList::changed(const QString &newText)
{
    curText = newText;

    list->clear();

    for(QStringList::Iterator it = accounts.begin(); it != accounts.end(); it++)
    {
        if((*it).startsWith(curText))
            list->insertItem(*it);
    }

}

QString AccountEditList::currentText() const
{
    return curText;
}

void AccountEditList::focusInEvent(QFocusEvent *event)
{
    QComboBox::focusInEvent(event);
    popup();
    changed(curText);
}

void AccountEditList::focusOutEvent(QFocusEvent *event)
{
    QComboBox::focusOutEvent(event);
    if(event->reason() == QFocusEvent::Mouse ||
       event->reason() == QFocusEvent::Tab ||
       event->reason() == QFocusEvent::Backtab)
        list->hide();
}

