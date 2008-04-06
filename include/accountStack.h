#ifndef ACCOUNT_STACK_H
#define ACCOUNT_STACK_H

#include <qwidgetstack.h>
#include <qwidget.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include "goBackLabel.h"
#include "accountTable.h"

class AccountStack : public QWidgetStack
{
    Q_OBJECT
    public:
        AccountStack( QWidget *parent = 0, const char *name = 0 );
        
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
            AccountTable *dataTable;
        } table;
        
};


#endif
