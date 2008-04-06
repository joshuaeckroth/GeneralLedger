
#include <qtabwidget.h>
#include <qapplication.h>
#include <qsettings.h>

#include "tabs.h"
#include "mainStack.h"
#include "accountStack.h"
#include "journalStack.h"
#include "reportStack.h"
#include "generalLedger.h"

Tabs::Tabs(QApplication *parent, const char *name)
    : QTabWidget(0,name)
{
    setFocusPolicy(QWidget::NoFocus);
    
    QSettings settings;
    settings.setPath("eckroth.net", "GeneralLedger");
    settings.beginGroup("/GeneralLedger");
    move(settings.readNumEntry("/geometry/x", 0),
         settings.readNumEntry("/geometry/y", 0));
                 
    resize(settings.readNumEntry("/geometry/width", 600),
           settings.readNumEntry("/geometry/height", 500));
    
    QString curClient(settings.readEntry("/database/defaultClient", ""));
    
    mainStack = new MainStack(curClient);
    connect(mainStack, SIGNAL(openDefault()), parent, SLOT(openDefault()));
    connect(mainStack, SIGNAL(openDefault()), this, SLOT(dbOpened()));
        
    connect(mainStack, SIGNAL(openNew(QString)), parent, SLOT(openNew(QString)));
    connect(mainStack, SIGNAL(openNew(QString)), this, SLOT(dbOpened()));
    
    connect(mainStack, SIGNAL(createNew(QString)), parent, SLOT(createNew(QString)));
    connect(mainStack, SIGNAL(createNew(QString)), this, SLOT(dbOpened()));
    
    connect(parent, SIGNAL(dbOpened()), mainStack, SLOT(dbOpened()));
    
    connect(mainStack, SIGNAL(prepareQuit()), parent, SLOT(prepareQuit()));
    
    connect(mainStack, SIGNAL(switchToAccounts()), this, SLOT(switchToAccounts()));
    connect(mainStack, SIGNAL(switchToJournal()), this, SLOT(switchToJournal()));
    connect(mainStack, SIGNAL(switchToReports()), this, SLOT(switchToReports()));
    
    connect(mainStack, SIGNAL(closeDb()), this, SIGNAL(closeDb()));
    connect(parent, SIGNAL(dbClosed()), this, SLOT(dbClosed()));
    
    connect(mainStack, SIGNAL(copyDb(QString)), this, SIGNAL(copyDb(QString)));
    
    accountStack = new AccountStack;
    connect(parent, SIGNAL(dbOpened()), accountStack, SLOT(dbOpened()));
    connect(accountStack, SIGNAL(goBack()), this, SLOT(switchToMain()));
    
    journalStack = new JournalStack;
    connect(parent, SIGNAL(dbOpened()), journalStack, SLOT(dbOpened()));
    connect(journalStack, SIGNAL(goBack()), this, SLOT(switchToMain()));
    
    reportStack = new ReportStack;
    connect(parent, SIGNAL(dbOpened()), reportStack, SLOT(dbOpened()));
    connect(reportStack, SIGNAL(goBack()), this, SLOT(switchToMain()));
    
    addTab( mainStack, QIconSet( QPixmap::fromMimeSource("icons/mainTab.png") ), "Main (F1)" );
    addTab( accountStack, QIconSet( QPixmap::fromMimeSource("icons/accountsTab.png") ), "Accounts (F2)" );
    setTabEnabled(accountStack, 0);
    addTab( journalStack, QIconSet( QPixmap::fromMimeSource("icons/journalTab.png") ), "Journal Entry (F3)" );
    setTabEnabled(journalStack, 0);
    addTab( reportStack, QIconSet( QPixmap::fromMimeSource("icons/reportTab.png") ), "Reports (F4)" );
    setTabEnabled(reportStack, 0);
    
}

void Tabs::dbOpened()
{
    setTabEnabled(accountStack, true);
    setTabEnabled(journalStack, true);
    setTabEnabled(reportStack, true);
}

void Tabs::dbClosed()
{
    setTabEnabled(accountStack, false);
    setTabEnabled(journalStack, false);
    setTabEnabled(reportStack, false);
    mainStack->dbClosed();
}

void Tabs::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Key_F1:
            showPage(mainStack);
            break;
        case Key_F2:
            if(isTabEnabled(accountStack))
                showPage(accountStack);
            break;
        case Key_F3:
            if(isTabEnabled(journalStack))
                showPage(journalStack);
            break;
        case Key_F4:
            if(isTabEnabled(reportStack))
                showPage(reportStack);
            break;
    }
}

void Tabs::closeEvent(QCloseEvent *event)
{
    emit writeSettings();
    event->accept();   
}

void Tabs::switchToMain()
{
    setCurrentPage(0);
}

void Tabs::switchToAccounts()
{
    setCurrentPage(1);
}

void Tabs::switchToJournal()
{
    setCurrentPage(2);
}

void Tabs::switchToReports()
{
    setCurrentPage(3);
}

