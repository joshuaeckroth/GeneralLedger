#include <qwidgetstack.h>
#include <qwidget.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include "journalStack.h"
#include "journalTable.h"
#include "goBackLabel.h"

JournalStack::JournalStack(QWidget *parent, const char *name)
    : QWidgetStack(parent,name)
{
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainVBoxLayout = new QVBoxLayout(mainWidget);
    QPushButton *edit = new QPushButton("Edit", mainWidget);
    connect(edit, SIGNAL(clicked()), this, SLOT(switchWidget()));
    mainVBoxLayout->addWidget(edit);
    
    addWidget(mainWidget, 0);
    
    active = 0;
}

void JournalStack::dbOpened()
{
    if(table.widget == widget(1)) {
        removeWidget(table.widget);
        delete table.widget;
        delete table.vBoxLayout;
        delete table.labelLayout;
        delete table.topLabelLeft;
        delete table.topLabelRight;
        delete table.dataTable;
    }
    
    table.widget = new QWidget(this);
    table.vBoxLayout = new QVBoxLayout(table.widget, 5, 5);
    table.labelLayout = new QHBoxLayout(table.vBoxLayout);
    table.topLabelLeft = new GoBackLabel(table.widget);
    connect(table.topLabelLeft, SIGNAL(goBack()), this, SLOT(switchWidget()));
    table.topLabelRight = new QLabel(tr("(ESC)"), table.widget);
    table.labelLayout->addWidget(table.topLabelLeft);
    table.labelLayout->addWidget(table.topLabelRight);
    table.labelLayout->addStretch(1);
    table.dataTable = new JournalTable(table.widget);
    table.vBoxLayout->addWidget(table.dataTable);
    
    addWidget(table.widget, 1);
}

void JournalStack::switchWidget()
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

