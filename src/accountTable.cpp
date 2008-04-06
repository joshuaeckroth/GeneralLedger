#include <qdatatable.h>
#include <qsqlcursor.h>

#include "accountTable.h"

AccountTable::AccountTable(QWidget *parent, const char *name)
    : QDataTable(parent,name)  
{
    setLeftMargin(0);
    
    cursor = new QSqlCursor("accounts");
    setSqlCursor(cursor);
    addColumn("id", "ID");
    addColumn("name", "Name");
    refresh();
}
   