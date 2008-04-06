#include <qcombobox.h>
#include <qstringlist.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qevent.h>

#include "accountEditList.h"

AccountEditList::AccountEditList(QWidget *parent, const char *name,
                                const QStringList &newAccounts, const QString &newText)
    : QComboBox(TRUE, parent, name), accounts(newAccounts), text(newText)
{
    listbox = new QListBox(this);
    listbox->setSelectionMode(QListBox::NoSelection);
    listbox->setFocusPolicy(QWidget::NoFocus);
    listbox->insertStringList(accounts);    // even though this is cleared on focusInEvent,
                                            // it's necessary to create a listbox that's the full size
    
    setListBox(listbox);
    
    setSizeLimit(10);
    
    setInsertionPolicy(QComboBox::NoInsertion);
    
    connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(changed(const QString &)));
    
}

void AccountEditList::updateAccounts(const QStringList &newAccounts)
{
    accounts = newAccounts;
    listbox->clear();
    listbox->insertStringList(accounts);
}

void AccountEditList::setText(const QString &newText)
{
    text = newText;
}

void AccountEditList::changed(const QString &newText)
{
    text = newText;
    listbox->clear();
    
    for(QStringList::Iterator it = accounts.begin(); it != accounts.end(); it++)
    {
        if((*it).startsWith(text))
            listbox->insertItem(*it);
    }
    
    popup();
    lineEdit()->setFocus();
    
}

QString AccountEditList::currentText() const
{
    return text;
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
        listbox->hide();
}

