#include <qdatatable.h>
#include <qsqlcursor.h>
#include <qfontmetrics.h>
#include <qfont.h>

#include "journalTable.h"
#include "editorFactory.h"

JournalTable::JournalTable(QWidget *parent, const char *name)
    : QDataTable(parent,name)  
{
    editorFactory = new EditorFactory(this);
    installEditorFactory(editorFactory);
    
    setLeftMargin(0);
        
    cursor = new QSqlCursor("journal");
    setSqlCursor(cursor);
    addColumn("date", "Date");
    addColumn("account", "Account");
    addColumn("reference", "Reference");
    addColumn("description", "Description");
    addColumn("debit", "Debit");
    addColumn("credit", "Credit");
    
    setColumnStretchable(0, false);
    setColumnStretchable(1, false);
    setColumnStretchable(2, false);
    setColumnStretchable(3, true);
    setColumnStretchable(4, false);
    setColumnStretchable(5, false);
    
    refresh();
}

int JournalTable::debitColWidth()
{
    return columnWidth(4);
}

int JournalTable::creditColWidth()
{
    return columnWidth(5);
}


