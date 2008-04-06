#include <qtable.h>
#include <qstringlist.h>

#include "accountEditItem.h"
#include "accountEditList.h"
#include "accountEditSimple.h"

AccountEditItem::AccountEditItem(QTable *table, QStringList &newAccounts, const QString &newText, const QString &newKey)
    : QTableItem(table, QTableItem::OnTyping), accounts(newAccounts), itemText(newText), itemKey(newKey)
{}

AccountEditItem::AccountEditItem(QTable *table, const QString &newText, const QString &newKey)
    : QTableItem(table, QTableItem::OnTyping), itemText(newText), itemKey(newKey)
{
    accounts = QStringList();
}

QWidget* AccountEditItem::createEditor() const
{
    if(accounts.count())
    {
        AccountEditList *editor = new AccountEditList(table()->viewport(), 0, accounts, itemText);
        return editor;
    }
    else
    {
        AccountEditSimple *editor = new AccountEditSimple(table()->viewport(), 0, itemText);
        return editor;
    }
}

void AccountEditItem::setContentFromEditor(QWidget *widget)
{
    if(widget->inherits("QComboBox"))
        itemText = ((AccountEditList*)widget)->currentText();
    else
        itemText = ((AccountEditSimple*)widget)->text();
}

QString AccountEditItem::text() const
{
    return itemText;
}

QString AccountEditItem::getKey() const
{
    return itemKey;
}

