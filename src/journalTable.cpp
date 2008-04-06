#include <qdatatable.h>
#include <qsqlcursor.h>

#include "journalTable.h"

JournalTable::JournalTable(QWidget *parent, const char *name)
    : QDataTable(parent,name)  
{
    setLeftMargin(0);
    
    cursor = new QSqlCursor("journal");
    setSqlCursor(cursor);
    addColumn("id", "ID");
    addColumn("date", "Date");
    addColumn("account", "Account");
    addColumn("reference", "Reference");
    addColumn("description", "Description");
    addColumn("debit", "Debit");
    addColumn("credit", "Credit");
    refresh();
}
   