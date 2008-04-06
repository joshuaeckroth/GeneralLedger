#ifndef DATE_EDIT_H
#define DATE_EDIT_H

#include <qtable.h>

class DateEdit : public QTableItem
{
    public:
        DateEdit(QTable *table, const QString &contents);
        QWidget* createEditor() const;
};

#endif

