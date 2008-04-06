#include <vector>
using std::vector;
#include <typeinfo>

#include <qwidgetstack.h>
#include <qwidget.h>
#include <qvgroupbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qfiledialog.h>
#include <qevent.h>

#include "accountStack.h"
#include "accountTable.h"
#include "goBackLabel.h"

AccountStack::AccountStack(QWidget *parent, const char *name)
    : QWidgetStack(parent,name)
{
    main.widget = new QWidget(this);
    
    main.vBoxLabel = new QVBoxLayout(main.widget);
    
    main.hBoxLabel = new QHBoxLayout(main.vBoxLabel);
    main.hBoxLabel->setMargin(5);
    
    main.topLabel = new GoBackLabel(main.widget);
    connect(main.topLabel, SIGNAL(goBack()), this, SIGNAL(goBack()));
    
    main.hBoxLabel->addWidget(main.topLabel);
    main.hBoxLabel->addStretch();
    
    main.hBoxLayout = new QHBoxLayout(main.vBoxLabel);
    
    main.hBoxLayout->addStretch();
    
    main.vBoxLayout = new QVBoxLayout(main.hBoxLayout);
    
    main.vBoxLayout->addStretch();
    
    main.taskGroup = new QVGroupBox("Accounts Tasks", main.widget);
    main.taskGroup->setInsideMargin(30);
    main.taskGroup->setInsideSpacing(20);
    
    main.vBoxLayout->add(main.taskGroup);
    
    main.editButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/editAccountsButton.png") ),
            "Edit / View Accounts", main.taskGroup);
    connect(main.editButton, SIGNAL(clicked()), this, SLOT(switchWidget()));
    main.editButton->setDefault(true);
    main.editButton->setFocus();
    main.editButton->installEventFilter(this);
    
    main.widgetVect.push_back(main.editButton);
    
    main.vBoxLayout->addSpacing(30);
    
    main.adminGroup = new QVGroupBox("Administrative Options", main.widget);
    main.adminGroup->setInsideMargin(30);
    main.adminGroup->setInsideSpacing(20);
    
    main.vBoxLayout->add(main.adminGroup);
    
    main.importButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/importAccountsButton.png") ),
            "Import Accounts (CSV format)", main.adminGroup);
    connect(main.importButton, SIGNAL(clicked()), this, SLOT(importCSV()));
    main.importButton->installEventFilter(this);
    
    main.widgetVect.push_back(main.importButton);
    
    main.exportButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/exportAccountsButton.png") ),
            "Export Accounts (CSV format)", main.adminGroup);
    connect(main.exportButton, SIGNAL(clicked()), this, SLOT(exportCSV()));
    main.exportButton->installEventFilter(this);
    
    main.widgetVect.push_back(main.exportButton);
    main.widgetIter = main.widgetVect.begin();
    
    main.vBoxLayout->addStretch();
    
    main.hBoxLayout->addStretch();
    
    addWidget(main.widget, 0);
    
    active = 0;
}

bool AccountStack::focusNextPrevChild(bool next)
{
    QWidget *prev;
    if(active == 0)
    {
        if(next)
        {
            if(main.widgetIter == (main.widgetVect.end() - 1))
            {
                prev = *main.widgetIter;
                main.widgetIter = main.widgetVect.begin();
            }
            else
            {
                prev = *main.widgetIter;
                main.widgetIter++;
            }
        }    
        else
        {
            if(main.widgetIter == main.widgetVect.begin())
            {
                prev = *main.widgetIter;
                main.widgetIter = (main.widgetVect.end() - 1);
            }
            else
            {
                prev = *main.widgetIter;
                main.widgetIter--;
            }
        }
    
        (*main.widgetIter)->setFocus();
        if(typeid(**main.widgetIter) == typeid(QPushButton))
            dynamic_cast<QPushButton*>(*main.widgetIter)->setDefault(true);
        if(typeid(*prev) == typeid(QPushButton))
            dynamic_cast<QPushButton*>(prev)->setDefault(false);
    }
    
    return true;
}

void AccountStack::dbOpened()
{
    if(id(table.widget) == 1)
    {
        removeWidget(table.widget);
        delete table.dataTable;
        delete table.topLabel;
        delete table.labelLayout;
        delete table.vBoxLayout;
        delete table.widget;
    }
    
    table.widget = new QWidget(this);
    table.vBoxLayout = new QVBoxLayout(table.widget);
    table.vBoxLayout->setMargin(5);
    table.vBoxLayout->setSpacing(5);
    table.labelLayout = new QHBoxLayout(table.vBoxLayout);
    table.topLabel = new GoBackLabel(table.widget);
    connect(table.topLabel, SIGNAL(goBack()), this, SLOT(switchWidget()));
    table.labelLayout->addWidget(table.topLabel);
    table.labelLayout->addStretch();
    table.dataTable = new AccountTable(table.widget);
    
    table.vBoxLayout->addWidget(table.dataTable);
    
    addWidget(table.widget, 1);
}

void AccountStack::importCSV()
{
    QString file = QFileDialog::getOpenFileName(
            ".",
            "Comma Separated Values (*.csv)",
            this,
            0,
            "Import Accounts");
    
    qDebug(file);
}

void AccountStack::exportCSV()
{
    QString file = QFileDialog::getSaveFileName(
            ".",
            "Comma Separated Values (*.csv)",
            this,
            0,
            "Export Accounts");
    
    qDebug(file);
}

void AccountStack::switchWidget()
{
    if(active == 0)
    {
        raiseWidget(1);
        active = 1;
    }
    else
    {
        (*main.widgetIter)->setFocus();
        raiseWidget(0);
        active = 0;
    }
}

bool AccountStack::eventFilter(QObject *target, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = (QKeyEvent *)event;
        if(keyEvent->key() == Key_Escape)
        {
            emit goBack();
            return true;
        }
    }
    return QWidgetStack::eventFilter(target,event);
}  


