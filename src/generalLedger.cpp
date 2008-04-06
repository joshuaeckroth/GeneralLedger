
#include <qapplication.h>
#include <qtabwidget.h>
#include <qiconset.h>
#include <qpixmap.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsettings.h>
#include <qevent.h>
#include <qfile.h>

#include "generalLedger.h"
#include "tabs.h"

GeneralLedger::GeneralLedger(int argc, char *argv[])
    : QApplication(argc, argv)
{
#ifdef Q_WS_WIN
    db = QSqlDatabase::addDatabase("QSQLITEX");
#else 
    db = QSqlDatabase::addDatabase("QSQLITE");
#endif 
    
    readDefaults();
    
    tabs = new Tabs(this);
    connect(tabs, SIGNAL(writeSettings()), this, SLOT(writeSettings()));
    connect(tabs, SIGNAL(closeDb()), this, SLOT(closeDb()));
    
    connect(tabs, SIGNAL(copyDb(QString)), this, SLOT(copyDb(QString)));
    
    tabs->setCaption("General Ledger");
    
    setMainWidget(tabs);
    tabs->show();
}

QString* GeneralLedger::getCurClient()
{
    return curClient;
}

QString* GeneralLedger::getCurDb()
{
    return curDb;
}

void GeneralLedger::openDefault()
{
    if(db->isOpen()) db->close();
    db->setDatabaseName(QString("data/").append(*curDb));
    if(!db->open())
    {
        db->lastError().showMessage();
        return;
    }
    
    tabs->setCaption(QString("General Ledger - ").append(*curClient));
    
    emit dbOpened();
}

void GeneralLedger::openNew(QString name)
{
    QString file(name);
    
    file.replace(QChar(' '), "_");
    file.replace(QChar('/'), "-");
    file.replace(QChar('\\'), "-");
    file.append(".db");
    
    if(db->isOpen()) db->close();
    db->setDatabaseName(QString("data/").append(file));
    if(!db->open())
    {
        db->lastError().showMessage();
        return;
    }
    
    delete curDb;
    curDb = new QString(file);
    
    delete curClient;
    curClient = new QString(name);
    
    tabs->setCaption(QString("General Ledger - ").append(name));
    
    emit dbOpened();
}

void GeneralLedger::createNew(QString name)
{
    QString file(name);
    
    file.replace(QChar(' '), "_");
    file.replace(QChar('/'), "-");
    file.replace(QChar('\\'), "-");
    file.append(".db");
    
    if(db->isOpen()) db->close();
    db->setDatabaseName(QString("data/").append(file));
    if (!db->open())
    {
        db->lastError().showMessage();
        return;
    }
    
    delete curDb;
    curDb = new QString(file);
    
    delete curClient;
    curClient = new QString(name);
    
    tabs->setCaption(QString("General Ledger - ").append(name));
    
    QSqlQuery createMain("CREATE TABLE main (name)");
    QSqlQuery createAccounts("CREATE TABLE accounts (id, name, PRIMARY KEY (id))");
    QSqlQuery createJournal("CREATE TABLE journal (id, date, account, reference, "
            "description, debit, credit, PRIMARY KEY (id))");
    
    QSqlQuery(QString("INSERT INTO main VALUES('").append(name).append("')"));
    
    emit dbOpened();
    
}

void GeneralLedger::writeSettings()
{
    QSettings settings;
    settings.setPath("eckroth.net", "GeneralLedger");
    settings.beginGroup("/GeneralLedger");
    settings.writeEntry("/geometry/x", tabs->x());
    settings.writeEntry("/geometry/y", tabs->y());
    settings.writeEntry("/geometry/width", tabs->width());
    settings.writeEntry("/geometry/height", tabs->height());
    settings.writeEntry("/database/defaultDb", *curDb);
    settings.writeEntry("/database/defaultClient", *curClient);
    settings.endGroup();
}

void GeneralLedger::readDefaults()
{
    QSettings settings;
    settings.setPath("eckroth.net", "GeneralLedger");
    settings.beginGroup("/GeneralLedger");
    
    curDb = new QString(settings.readEntry("/database/defaultDb", ""));
    curClient = new QString(settings.readEntry("/database/defaultClient", ""));
}

void GeneralLedger::closeDb()
{
    if(db->isOpen()) db->close();
    tabs->setCaption("General Ledger");
    emit dbClosed();   
}

void GeneralLedger::copyDb(QString file)
{
    QFile origDb(QString("data/").append(*curDb));
    QFile newDb(file);
    
    if( !origDb.open(IO_ReadOnly | IO_Raw) )
    {
        qDebug("origDb not open");
        qDebug(origDb.errorString());
        return;
    }
    if( !newDb.open(IO_WriteOnly) )
    {
        qDebug("newDb not open");
        qDebug(newDb.errorString());
        return;
    }
    
    char byte = origDb.getch();
    while(byte != -1)
    {
        newDb.putch(byte);
        byte = origDb.getch();
    }
    
    newDb.close();
    origDb.close();
}

void GeneralLedger::prepareQuit()
{
    writeSettings();
    emit quit();
}


