#include <qcombobox.h>
#include <qstringlist.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qevent.h>

#include "accountEdit.h"

AccountEdit::AccountEdit(QWidget *parent, const char *name, QStringList &newAccounts)
    : QComboBox(TRUE, parent, name)
{
    accounts = newAccounts;
    
    listbox = new QListBox(this);
    listbox->setSelectionMode(QListBox::NoSelection);
    listbox->setFocusPolicy(QWidget::NoFocus);
    listbox->insertStringList(accounts);    // even though this is cleared on focusInEvent,
                                            // it's necessary to create a listbox that's the full size
    
    setListBox(listbox);
    
    setSizeLimit(10);
    
    setInsertionPolicy(QComboBox::NoInsertion);
    
    text = "";

    connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(changed(const QString &)));
    
}

void AccountEdit::changed(const QString &newText)
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

QString AccountEdit::currentText() const
{
    return text;
}

void AccountEdit::focusInEvent(QFocusEvent *event)
{
    QComboBox::focusInEvent(event);
    changed("");
}

void AccountEdit::focusOutEvent(QFocusEvent *event)
{
    QComboBox::focusOutEvent(event);
    if(event->reason() == QFocusEvent::Mouse ||
       event->reason() == QFocusEvent::Tab ||
       event->reason() == QFocusEvent::Backtab)
        listbox->hide();
}

