#ifndef ACCOUNT_EDIT_ITEM_H
#define ACCOUNT_EDIT_ITEM_H

#include <qtable.h>

class QStringList;

class AccountEditItem : public QTableItem
{
    public:
        AccountEditItem(QTable *table, QStringList &newAccounts, const QString &newText, const QString &newKey);
        AccountEditItem(QTable *table, const QString &newText, const QString &newKey);
        QWidget* createEditor() const;
        void setContentFromEditor(QWidget *widget);
        QString text() const;
        QString getKey() const;
        
    private:
        QStringList accounts;
        QString itemText;
        QString itemKey;
       
};

#endif

