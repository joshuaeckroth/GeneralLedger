#ifndef ACCOUNT_LIST_H
#define ACCOUNT_LIST_H

#include <qcombobox.h>

class QStringList;

class AccountList : public QComboBox
{
    public:
        AccountList(QWidget *parent, const char *name, QStringList &accounts);
        QString accountNumber() const;
        void updateAccounts(const QStringList &accounts);

};


#endif
