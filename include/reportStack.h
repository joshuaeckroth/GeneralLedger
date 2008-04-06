#ifndef REPORT_STACK_H
#define REPORT_STACK_H

#include <qwidgetstack.h>
#include <qwidget.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include "goBackLabel.h"

class ReportStack : public QWidgetStack
{
    Q_OBJECT
    public:
        ReportStack( QWidget *parent = 0, const char *name = 0 );
        
    private slots:
        void dbOpened();
        void switchWidget();
        
    private:
        int active;
        
};


#endif
