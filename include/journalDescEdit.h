#ifndef JOURNAL_DESC_EDIT_H
#define JOURNAL_DESC_EDIT_H

#include <qtable.h>

class JournalTable;

class JournalDescEdit : public QTableItem
{
    public:
        JournalDescEdit(JournalTable *table, const QString &contents);
        QWidget* createEditor() const;

    private:
        JournalTable *journalTable;
};

#endif
