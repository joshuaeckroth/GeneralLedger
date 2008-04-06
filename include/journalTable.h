#ifndef JOURNAL_TABLE_H
#define JOURNAL_TABLE_H

#include <qtable.h>

class JournalTable : public QTable
{
    public:
        JournalTable(QWidget *parent = 0, const char *name = 0);
        int debitColWidth();
        int creditColWidth();
        
    private:
        
};

#endif
