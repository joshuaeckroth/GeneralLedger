#ifndef BASIC_EDIT_H
#define BASIC_EDIT_H

#include <qtable.h>

class QEvent;

class BasicEdit : public QTableItem
{
    public:
        BasicEdit(QTable *table, const QString &contents);
};

#endif

