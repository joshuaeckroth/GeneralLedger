
#include <qapplication.h>
#include <qtabwidget.h>
#include <qiconset.h>
#include <qpixmap.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>

#include "generalLedger.h"
#include "mainStack.h"
#include "accountStack.h"
#include "journalStack.h"
#include "reportStack.h"

GeneralLedger::GeneralLedger(int argc, char *argv[])
    : QApplication(argc, argv)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    
    tabs = new QTabWidget;
    tabs->setCaption("General Ledger");
    
    mainStack = new MainStack;
    connect(mainStack, SIGNAL(openDefault()), this, SLOT(openDefault()));
    connect(mainStack, SIGNAL(openNew(QString)), this, SLOT(openNew(QString)));
    connect(mainStack, SIGNAL(createNew(QString)), this, SLOT(createNew(QString)));
    connect(mainStack, SIGNAL(quit()), this, SLOT(quit()));
    
    accountStack = new AccountStack;
    connect(this, SIGNAL(dbOpened()), accountStack, SLOT(dbOpened()));
    
    journalStack = new JournalStack;
    connect(this, SIGNAL(dbOpened()), journalStack, SLOT(dbOpened()));
    
    reportStack = new ReportStack;
    connect(this, SIGNAL(dbOpened()), reportStack, SLOT(dbOpened()));
    
    tabs->addTab( mainStack, QIconSet( QPixmap::fromMimeSource("icons/mainTab.png") ), "Main (F1)" );
    tabs->addTab( accountStack, QIconSet( QPixmap::fromMimeSource("icons/accountsTab.png") ), "Accounts (F2)" );
    tabs->setTabEnabled(accountStack, 0);
    tabs->addTab( journalStack, QIconSet( QPixmap::fromMimeSource("icons/journalTab.png") ), "Journal Entry (F3)" );
    tabs->setTabEnabled(journalStack, 0);
    tabs->addTab( reportStack, QIconSet( QPixmap::fromMimeSource("icons/reportTab.png") ), "Reports (F4)" );
    tabs->setTabEnabled(reportStack, 0);
    
    setMainWidget(tabs);
    
    tabs->show();
}

void GeneralLedger::openDefault()
{
    
}

void GeneralLedger::openNew(QString name)
{
    if(db->isOpen()) db->close();
    db->setDatabaseName(name);
    if(!db->open())
    {
        db->lastError().showMessage();
        return;
    }
    
    tabs->setTabEnabled(accountStack, 1);
    tabs->setTabEnabled(journalStack, 1);
    tabs->setTabEnabled(reportStack, 1);
    
    emit dbOpened();
}

void GeneralLedger::createNew(QString name)
{
    if(db->isOpen()) db->close();
    db->setDatabaseName(name);
    if (!db->open())
    {
        db->lastError().showMessage();
        return;
    }
    
    QString caption("General Ledger - ");
    caption.append(name);
    tabs->setCaption(caption);
    
    QSqlQuery createMain("CREATE TABLE main (name)");
    QSqlQuery createAccounts("CREATE TABLE accounts (id, name, PRIMARY KEY (id))");
    QSqlQuery createJournal("CREATE TABLE journal (id, date, account, reference, "
            "description, debit, credit, PRIMARY KEY (id))");
    
    QString insert("INSERT INTO main VALUES(");
    insert.append(name);
    insert.append(")");
    QSqlQuery insertMain(insert);
    
    tabs->setTabEnabled(accountStack, 1);
    tabs->setTabEnabled(journalStack, 1);
    tabs->setTabEnabled(reportStack, 1);

    
    emit dbOpened();
    
}

