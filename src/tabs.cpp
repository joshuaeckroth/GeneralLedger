#include <qtabwidget.h>
#include <qapplication.h>

#include "database.h"
#include "settings.h"
#include "tabs.h"
#include "mainStack.h"
#include "accountStack.h"
#include "journalStack.h"
#include "reportStack.h"
#include "helpStack.h"

Tabs::Tabs(QApplication *parent, const char *name)
    : QTabWidget(0,name)
{
    db = Database::instance();
    settings = Settings::instance();

    iconPath = settings->getIconPath();
    
    app = parent;
    
    setFocusPolicy(QWidget::NoFocus);
    
    move(settings->getDefaultX(), settings->getDefaultY());
    resize(settings->getDefaultWidth(), settings->getDefaultHeight());
    
    setCaption("General Ledger");
    
    mainStack = new MainStack;
    connect(mainStack, SIGNAL(dbOpened()), this, SLOT(dbOpened()));
    connect(mainStack, SIGNAL(switchToAccounts()), this, SLOT(switchToAccounts()));
    connect(mainStack, SIGNAL(switchToJournal()), this, SLOT(switchToJournal()));
    connect(mainStack, SIGNAL(switchToReports()), this, SLOT(switchToReports()));
    connect(mainStack, SIGNAL(dbClosed()), this, SLOT(dbClosed()));
    connect(mainStack, SIGNAL(nameChanged()), this, SLOT(nameChanged()));
    connect(mainStack, SIGNAL(quit()), this, SLOT(quit()));
    
    accountStack = new AccountStack;
    connect(mainStack, SIGNAL(dbOpened()), accountStack, SLOT(dbOpened()));
    connect(accountStack, SIGNAL(goToMain()), this, SLOT(switchToMain()));
    connect(accountStack, SIGNAL(goToJournal()), this, SLOT(switchToJournal()));
    connect(accountStack, SIGNAL(goToReports()), this, SLOT(switchToReports()));
    connect(accountStack, SIGNAL(goToHelp()), this, SLOT(switchToHelp()));
    
    journalStack = new JournalStack;
    connect(mainStack, SIGNAL(dbOpened()), journalStack, SLOT(dbOpened()));
    connect(journalStack, SIGNAL(goToMain()), this, SLOT(switchToMain()));
    connect(journalStack, SIGNAL(goToAccounts()), this, SLOT(switchToAccounts()));
    connect(journalStack, SIGNAL(goToReports()), this, SLOT(switchToReports()));
    connect(journalStack, SIGNAL(goToHelp()), this, SLOT(switchToHelp()));
    
    reportStack = new ReportStack;
    connect(mainStack, SIGNAL(dbOpened()), reportStack, SLOT(dbOpened()));
    connect(reportStack, SIGNAL(goToMain()), this, SLOT(switchToMain()));
    
    helpStack = new HelpStack;
    connect(helpStack, SIGNAL(goToMain()), this, SLOT(switchToMain()));
    
    addTab( mainStack, QIconSet( QPixmap::fromMimeSource(iconPath + "/mainTab.png") ), "Main (F1)" );
    addTab( accountStack, QIconSet( QPixmap::fromMimeSource(iconPath + "/accountsTab.png") ), "Accounts (F2)" );
    setTabEnabled(accountStack, 0);
    addTab( journalStack, QIconSet( QPixmap::fromMimeSource(iconPath + "/journalTab.png") ), "Journal Entry (F3)" );
    setTabEnabled(journalStack, 0);
    addTab( reportStack, QIconSet( QPixmap::fromMimeSource(iconPath + "/reportTab.png") ), "Reports (F4)" );
    setTabEnabled(reportStack, 0);
    addTab( helpStack, QIconSet( QPixmap::fromMimeSource(iconPath + "/help.png") ), "Help (F5)" );
    
}

Tabs::~Tabs()
{}

void Tabs::dbOpened()
{
    setCaption("General Ledger - " + db->getCurClient());
    
    setTabEnabled(accountStack, true);
    setTabEnabled(journalStack, true);
    setTabEnabled(reportStack, true);
}

void Tabs::dbClosed()
{
    setCaption("General Ledger");
    
    setTabEnabled(accountStack, false);
    setTabEnabled(journalStack, false);
    setTabEnabled(reportStack, false);
}

void Tabs::nameChanged()
{
    setCaption("General Ledger - " + db->getCurClient());
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
        case Key_F5:
            showPage(helpStack);
            break;
    }
}

void Tabs::closeEvent(QCloseEvent *event)
{
    quit();
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

void Tabs::switchToHelp()
{
    setCurrentPage(4);
}

void Tabs::quit()
{
    settings->setDefaultX(x());
    settings->setDefaultY(y());
    settings->setDefaultWidth(width());
    settings->setDefaultHeight(height());
    
    app->quit();
}

