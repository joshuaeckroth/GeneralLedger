#ifndef MONEY_EDIT_H
#define MONEY_EDIT_H

#include <qtable.h>

class MoneyEdit : public QTableItem
{
    public:
        MoneyEdit(QTable *table, const QString &contents);
};

#endif

