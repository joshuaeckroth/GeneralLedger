#ifndef JOURNAL_STACK_H
#define JOURNAL_STACK_H

#include <qwidgetstack.h>
#include <qwidget.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include "goBackLabel.h"
#include "journalTable.h"

class JournalStack : public QWidgetStack
{
    Q_OBJECT
    public:
        JournalStack( QWidget *parent = 0, const char *name = 0 );
        
    private slots:
        void dbOpened();
        void switchWidget();
        
    private:
        int active;
        struct {
            QWidget *widget;
            QVBoxLayout *vBoxLayout;
            QHBoxLayout *labelLayout;
            GoBackLabel *topLabelLeft;
            QLabel *topLabelRight;
            JournalTable *dataTable;
        } table;
        
};


#endif
