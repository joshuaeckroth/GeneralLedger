#ifndef JOURNAL_TABLE_H
#define JOURNAL_TABLE_H

#include <qdatatable.h>
#include <qsqlcursor.h>

class JournalTable : public QDataTable
{
    public:
        JournalTable(QWidget *parent = 0, const char *name = 0);
    private:
        QSqlCursor *cursor;
       
};

#endif  