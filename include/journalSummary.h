#ifndef JOURNAL_SUMMARY_H
#define JOURNAL_SUMMARY_H

#include <qwidget.h>

class QGridLayout;
class QLabel;
class QSpacerItem;

class JournalTable;

class JournalSummary : public QWidget
{
    Q_OBJECT
    public:
        JournalSummary(QWidget *parent = 0, const char *name = 0, JournalTable *table = 0);
        ~JournalSummary();
        
    private:
        QGridLayout *grid;
        QLabel *debitLabel;
        QLabel *debitValue;
        QLabel *creditLabel;
        QLabel *creditValue;
        QLabel *balanceLabel;
        QLabel *balanceValue;
        QSpacerItem *debitWidth;
        QSpacerItem *creditWidth;
        QSpacerItem *scrollWidth;
        
};


#endif
