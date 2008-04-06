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
    connect(list, SIGNAL(selected(const QString&)), this, SLOT(setText(const QString&)));
    setListBox(list);

    list->insertStringList(accounts);    // even though this is cleared on focusInEvent,
                                         // it's necessary to create a listbox that's the full size
    setSizeLimit(10);
    
    setInsertionPolicy(QComboBox::NoInsertion);
    
    connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(changed(const QString &)));
    
}

void AccountEditList::updateAccounts(const QStringList &newAccounts)
{
    accounts = newAccounts;
    list->clear();
    list->insertStringList(accounts);
}

void AccountEditList::setText(const QString &newText)
{
    curText = newText.left(4);
    lineEdit()->setText(curText);
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
    
    popup();
    lineEdit()->setFocus();
}

QString AccountEditList::currentText() const
{
    return curText;
}

void AccountEditList::focusInEvent(QFocusEvent *event)
{
    QComboBox::focusInEvent(event);
    changed("");
}

void AccountEditList::focusOutEvent(QFocusEvent *event)
{
    QComboBox::focusOutEvent(event);
    if(event->reason() == QFocusEvent::Mouse ||
       event->reason() == QFocusEvent::Tab ||
       event->reason() == QFocusEvent::Backtab)
        list->hide();
}

