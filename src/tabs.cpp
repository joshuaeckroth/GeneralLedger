#include <qtabwidget.h>

#include "tabs.h"
#include "mainStack.h"
#include "accountStack.h"
#include "journalStack.h"
#include "reportStack.h"


Tabs::Tabs( QWidget* parent, const char* name, WFlags f ) 
    : QTabWidget(parent, name, f)  
{
    setCaption("General Ledger");
    
    MainStack *mainStack = new MainStack;
    connect(mainStack, SIGNAL(openDefault()), this, SLOT(openDefault()));
    connect(mainStack, SIGNAL(openNew(QString)), this, SLOT(openNew(QString)));
    connect(mainStack, SIGNAL(createNew(QString)), this, SLOT(createNew(QString)));
    connect(mainStack, SIGNAL(quit()), this, SLOT(quit()));
    
    AccountStack *accountStack = new AccountStack;
    JournalStack *journalStack = new JournalStack;
//    ReportStack *reportStack = new ReportStack;
    
    addTab( mainStack, "Main" );
    addTab( accountStack, "Accounts" );
    addTab( journalStack, "Journal Entry" );
//    addTab( reportStack, "Reports" );
    
} 

QString Tabs::getClient() const
{
    return QString("blah");
}

void Tabs::openDefault()
{
    
}

void Tabs::openNew(QString name)
{
    
}

void Tabs::createNew(QString name)
{
    
}

void Tabs::quit()
{
    
    emit appQuit();
}