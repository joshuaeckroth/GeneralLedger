#ifndef JOURNAL_TABLE_H
#define JOURNAL_TABLE_H

#include <qdatatable.h>

class QSqlCursor;
class EditorFactory;

class JournalTable : public QDataTable
{
    public:
        JournalTable(QWidget *parent = 0, const char *name = 0);
        int debitColWidth();
        int creditColWidth();
        
    private:
        QSqlCursor *cursor;
        EditorFactory *editorFactory;
       
};

#endif
