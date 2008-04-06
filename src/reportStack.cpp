#include <qwidgetstack.h>
#include <qwidget.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include "reportStack.h"
#include "goBackLabel.h"

ReportStack::ReportStack(QWidget *parent, const char *name)
    : QWidgetStack(parent,name)
{
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainVBoxLayout = new QVBoxLayout(mainWidget);
    QPushButton *edit = new QPushButton("Edit", mainWidget);
//     connect(edit, SIGNAL(clicked()), this, SLOT(switchWidget()));
    mainVBoxLayout->addWidget(edit);
    
    addWidget(mainWidget, 0);
    
    active = 0;
}

void ReportStack::dbOpened()
{
}

void ReportStack::switchWidget()
{
    if(active == 0) {
            raiseWidget(1);
            active = 1;
    }
    else {
        raiseWidget(0);
        active = 0;
    }
}

