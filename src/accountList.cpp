#include <qcombobox.h>
#include <qstring.h>
#include <qstringlist.h>

#include "accountList.h"

AccountList::AccountList(QWidget *parent, const char *name, QStringList &accounts)
    : QComboBox(false, parent, name)
{
    insertStringList(accounts);
}

QString AccountList::accountNumber() const
{
    int pos = currentText().find(" - ");
    return currentText().left(pos);
}

void AccountList::updateAccounts(const QStringList &accounts)
{
    clear();
    insertStringList(accounts);
}
