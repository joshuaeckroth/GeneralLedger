#ifndef MONEY_EDIT_ITEM_H
#define MONEY_EDIT_ITEM_H

#include <qtable.h>

class MoneyEditItem : public QTableItem
{
    public:
        MoneyEditItem(QTable *table, const QString &contents);
        QWidget* createEditor() const;
};

#endif

