#include <qtable.h>
#include <qstringlist.h>

#include "journalTable.h"

JournalTable::JournalTable(QWidget *parent, const char *name)
    : QTable(parent,name)  
{
    setLeftMargin(0);
    setVScrollBarMode(QScrollView::AlwaysOn);
    insertColumns(0,6);
    
    QStringList labels;
    labels << "Date" << "Account" << "Reference" << "Description" << "Debit" << "Credit";
    setColumnLabels(labels);
              
    setColumnStretchable(3, true);
    
    insertRows(5);
    
}

int JournalTable::debitColWidth()
{
    return columnWidth(4);
}

int JournalTable::creditColWidth()
{
    return columnWidth(5);
}


