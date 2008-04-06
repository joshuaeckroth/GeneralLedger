#ifndef ACCOUNT_TABLE_H
#define ACCOUNT_TABLE_H

#include <qdatatable.h>
#include <qsqlcursor.h>

class AccountTable : public QDataTable
{
    public:
        AccountTable(QWidget *parent = 0, const char *name = 0);
    private:
        QSqlCursor *cursor;
       
};

#endif  