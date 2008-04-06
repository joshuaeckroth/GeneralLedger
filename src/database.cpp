#include <sstream>
using std::ostringstream;

#include <qstring.h>
#include <qstringlist.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qptrlist.h>
#include <qsettings.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtable.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qframe.h>
#include <qdatetime.h>

#include "database.h"
#include "balanceElement.h"

Database::Database() : QObject()
{
    instances++;
    
    if(db == 0)
    {
#ifdef Q_WS_WIN
        db = QSqlDatabase::addDatabase("QSQLITEX");
#else
        db = QSqlDatabase::addDatabase("QSQLITE");
#endif
    }
    
    if(!report)
        report = new ReportStruct;
    if(!accountsList)
        accountsList = new QStringList;
    
    QSettings settings;
    settings.setPath("eckroth.net", "GeneralLedger");
    settings.beginGroup("/GeneralLedger");
    curDb = settings.readEntry("/database/defaultDb", "");
    curClient = settings.readEntry("/database/defaultClient", "");
    settings.endGroup();
}

Database::~Database()
{
    if(instances == 1)
    {
        delete report;
        delete accountsList;
    }
    
    instances--;
}

bool Database::openDefault()
{
    if(!QFile("data/" + curDb).exists())
        return false;
    
  if(db->isOpen()) db->close();
  db->setDatabaseName("data/" + curDb);
  
  if(!db->open())
  {
      db->lastError().showMessage();
      return false;
  }
  return true;
}

bool Database::openNew(QString name)
{
    QString file(name);

    file.replace(QChar(' '), "_");
    file.replace(QChar('/'), "-");
    file.replace(QChar('\\'), "-");
    file.append(".db");
    
    if(db->isOpen()) db->close();
    db->setDatabaseName("data/" + file);
    if(!db->open())
    {
        db->lastError().showMessage();
        return false;
    }
    
    curDb = file;
    
    QSqlQuery query("SELECT name FROM main");
    if(query.next())
        curClient = query.value(0).toString();
    else
        return false;
  
    return true;
}

bool Database::createNew(QString name)
{
    QString file(name);
    
    file.replace(QChar(' '), "_");
    file.replace(QChar('/'), "-");
    file.replace(QChar('\\'), "-");
    file.append(".db");
    
    if(db->isOpen()) db->close();
    db->setDatabaseName("data/" + file);
    if (!db->open())
    {
        db->lastError().showMessage();
        return false;
    }
    
    curDb = file;
    curClient = name;
        
    QSqlQuery createMain("CREATE TABLE main (name)", db);
    QSqlQuery createAccounts("CREATE TABLE accounts (id, name, PRIMARY KEY (id))", db);
    QSqlQuery createJournal("CREATE TABLE journal (date, account, reference, "
            "description, debit, credit)", db);
    QSqlQuery createBalanceAssets("CREATE TABLE balanceAssets (id, type, desc, begin, end)", db);
    QSqlQuery createBalanceLiabilities("CREATE TABLE balanceLiabilities (id, type, desc, begin, end)", db);
    QSqlQuery createBalanceEquities("CREATE TABLE balanceEquities (id, type, desc, begin, end)", db);
    
    QSqlQuery("INSERT INTO main VALUES('" + name + "')", db);
    
    return true;
}

void Database::closeDb()
{
  if(db->isOpen()) db->close();
}

void Database::copyDb(QString file) const
{
    QFile origDb("data/" + curDb);
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

void Database::editName(QString name)
{
    if(!db->open())
    {
        db->lastError().showMessage();
        return;
    }
    
    QSqlQuery update("UPDATE main SET name = '" + name + "'", db);
    curClient = name;
}

QString Database::getCurDb() const
{
    return curDb;
}

QString Database::getCurClient() const
{
    return curClient;
}

QStringList Database::getClientList() const
{
    QStringList list;
    
#ifdef Q_WS_WIN
        QSqlDatabase *dbTemp = QSqlDatabase::addDatabase("QSQLITEX", "temp");
#else
        QSqlDatabase *dbTemp = QSqlDatabase::addDatabase("QSQLITE", "temp");
#endif 
    
    QDir dir("data");
    QStringList::Iterator it;
    QStringList files = dir.entryList("*.db", QDir::Files);
    it = files.begin();
    while(it != files.end())
    {
        dbTemp->setDatabaseName("data/" + *it);
        if(dbTemp->open())
        {
            QSqlQuery query("SELECT name FROM main", dbTemp);
            if(query.next())
                list << QString(query.value(0).toString() + "    (" + *it + ")");
            dbTemp->close();
        }
        ++it;
    }
    
    return list;
}

void Database::exportCSV(QString table, QString file) const
{
    QFile destFile(file);
    
    if(destFile.open(IO_WriteOnly))
    {
        QTextStream destStream(&destFile);
        
        QSqlQuery query("SELECT * FROM " + table);
        
        if(table == "accounts")
        {
            destStream << "Account ID,Account Description\n";
        
            while(query.next())
                destStream << query.value(0).toString() << "," << query.value(1).toString() << "\n";
        }
        if(table == "journal")
        {
            destStream << "Date,Account,Reference,Description,Debit,Credit\n";
            
            while(query.next())
            {
                destStream << query.value(0).toString() << "," << query.value(1).toString() << ","
                        << query.value(2).toString() << "," << query.value(3).toString() << ","
                        << query.value(4).toString() << "," << query.value(5).toString() << "\n";
            }
        }
    }
    destFile.close();
}

void Database::importCSV(QString table, QString file)
{
    QFile origFile(file);
    
    if(origFile.open(IO_ReadOnly))
    {
        QTextStream origStream(&origFile);
        QStringList data;
        QPtrList<QStringList> values;
        QStringList *list;
        
        int i = 0;
        QString line;
        while(!origStream.atEnd())
        {
            line = origStream.readLine();
            data << line;
            
            if(i < 2)
            {
                list = new QStringList(QStringList::split(",", line));
                values.append(list);
            }
            i++;
        }
        
        if(!values.isEmpty())
        {
            QDialog dialog;
            dialog.setCaption("Import CSV Data");
            
            QVBoxLayout vBoxLayout(&dialog);
            vBoxLayout.setMargin(5);
            vBoxLayout.setSpacing(5);
            
            QLabel label("<nobr>The first three records of the file are shown.</nobr><br>"
                    "<nobr>Choose whether the first record is a header or data.</nobr>", &dialog);
            
            QTable dataTable(3, values.first()->count(), &dialog);
            dataTable.setLeftMargin(0);
            dataTable.setTopMargin(0);
            dataTable.setSelectionMode(QTable::SingleRow);
            
            int row = 0;
            int col = 0;
            for(list = values.first(); list; list = values.next())
            {
                col = 0;
                for(QStringList::Iterator it = list->begin(); it != list->end(); it++)
                {
                    dataTable.setItem(row, col, new QTableItem(&dataTable, QTableItem::Never, *it));
                    col++;
                }
                row++;
            }
            dataTable.setColumnStretchable((col - 1), true);
            dataTable.selectRow(0);
            
            QVButtonGroup buttonGroup(&dialog);
            QRadioButton headerButton("The first record is a header", &buttonGroup);
            headerButton.setChecked(true);
            QRadioButton dataButton("The first record is data", &buttonGroup);
        
            QFrame okCancelFrame(&dialog);
            QHBoxLayout okCancelLayout(&okCancelFrame);
            okCancelLayout.setSpacing(5);
        
            QPushButton okButton("OK", &okCancelFrame);
            connect(&okButton, SIGNAL(clicked()), &dialog, SLOT(accept()));
            QPushButton cancelButton("Cancel", &okCancelFrame);
            connect(&cancelButton, SIGNAL(clicked()), &dialog, SLOT(reject()));
            
            okCancelLayout.addStretch();
            okCancelLayout.addWidget(&okButton);
            okCancelLayout.addWidget(&cancelButton);
        
            vBoxLayout.addWidget(&label);
            vBoxLayout.addWidget(&dataTable);
            vBoxLayout.addWidget(&buttonGroup);
            vBoxLayout.addWidget(&okCancelFrame);
            
            if(dialog.exec())
            {
                int firstLine;
                if(headerButton.isChecked())
                    firstLine = 1;
                else
                    firstLine = 0;
                
                int curLine = 0;
                for(QStringList::Iterator it = data.begin(); it != data.end(); it++)
                {
                    if(curLine < firstLine)  {curLine++; continue;}
                    
                    (*it).replace(',', "\", \"");
                    
                    QSqlQuery query(QString::null, db);
                    if(table == "accounts")
                        query.prepare("INSERT OR REPLACE INTO " + table + " VALUES(\"" + *it + "\")");
                    else
                        query.prepare("INSERT INTO " + table + " VALUES(\"" + *it + "\")");
                    query.exec();
                }
            }
        }
    }
    origFile.close();
}

QPtrList<BalanceElement> Database::getBalanceElements(balanceCategory category)
{
    QPtrList<BalanceElement> tmpList;

    if(!db->open())
    {
        db->lastError().showMessage();
        return tmpList;
    }
    
    QSqlQuery query(QString::null, db);
    
    switch(category)
    {
        case assets:
            query.prepare("SELECT * FROM balanceAssets ORDER BY id");
            break;
        case liabilities:
            query.prepare("SELECT * FROM balanceLiabilities ORDER BY id");
            break;
        case equities:
            query.prepare("SELECT * FROM balanceEquities ORDER BY id");
            break;
    }
    
    query.exec();
    
    while(query.next())
    {
        BalanceElement *element = new BalanceElement(
                category,
                query.value(0).toString(),
                query.value(1).toString(),
                query.value(2).toString(),
                query.value(3).toString(),
                query.value(4).toString());
        tmpList.append(element);
    }
    return tmpList;
}

void Database::createBalanceElement(balanceCategory category, QString type,
                                    QString desc, QString accountBegin, QString accountEnd)
{
    if(!db->open())
    {
        db->lastError().showMessage();
        return;
    }
    
    QString table;
    switch(category)
    {
        case assets:
            table = "balanceAssets";
            break;
        case liabilities:
            table = "balanceLiabilities";
            break;
        case equities:
            table = "balanceEquities";
            break;
    }
    
    QSqlQuery queryLast("SELECT id FROM " + table + " ORDER BY id");
    
    QString oneUp;
    if(queryLast.last())
        oneUp.setNum(queryLast.value(0).toInt() + 1);
    else
        oneUp.setNum(1);
    
    if(type == "0")
    {
        QSqlQuery queryDesc("SELECT name FROM accounts WHERE id = " + accountBegin, db);
        if(queryDesc.next())
            desc = queryDesc.value(0).toString();
    }
    
    QSqlQuery queryInsert("INSERT INTO " + table + " VALUES( \"" + oneUp + "\", \"" +
            type + "\", \"" + desc + "\", \"" + accountBegin + "\", \"" + accountEnd + "\")", db);
}

void Database::removeBalanceElement(balanceCategory category, QString id)
{
    if(!db->open())
    {
        db->lastError().showMessage();
        return;
    }
    
    QSqlQuery query(QString::null, db);
    switch(category)
    {
        case assets:
            query.prepare("DELETE FROM balanceAssets WHERE id = " + id);
            break;
        case liabilities:
            query.prepare("DELETE FROM balanceLiabilities WHERE id = " + id);
            break;
        case equities:
            query.prepare("DELETE FROM balanceEquities WHERE id = " + id);
            break;
    }
    query.exec();
}

void Database::moveBalanceElement(balanceCategory category, QString id, bool up)
{
    if(id == "1" && up) return;
    
    if(!db->open())
    {
        db->lastError().showMessage();
        return;
    }
    
    QString oneUp;
    oneUp.setNum(id.toInt() - 1);
    QString oneDown;
    oneDown.setNum(id.toInt() + 1);

    QString queryLastString("SELECT id FROM ");
    QString queryUpdateString("UPDATE ");
    QString table;
    
    switch(category)
    {
        case assets:
            table = "balanceAssets";
            break;
        case liabilities:
            table = "balanceLiabilities";
            break;
        case equities:
            table = "balanceEquities";
            break;
    }

    queryLastString += table + " ORDER BY id";
    QSqlQuery queryLast(queryLastString, db);
    queryLast.last();
    if(queryLast.value(0).toString() == id && !up) return;
    
    queryUpdateString += table + " SET id = ";
    
    QString upOrDown;
    if(up)
        upOrDown = oneUp;
    else
        upOrDown = oneDown;
    queryUpdateString += "0 WHERE id = ";
    queryUpdateString += id;

    QSqlQuery queryUpdate(queryUpdateString, db);
    
    QSqlQuery queryUpdate2("UPDATE " + table + " SET id = " + id + " WHERE id = " + upOrDown, db);
    QSqlQuery queryUpdate3("UPDATE " + table + " SET id = " + upOrDown + " WHERE id = 0", db);
}

Database* Database::getChartAccounts()
{
    QSqlQuery query("SELECT * FROM accounts", db);
    
    QDate date(QDate().currentDate());
    
    report->csv = "Account ID,Account Description\n";
    report->string = "<qt>\n";
    report->string += "<center><h1>";
    report->string += curClient;
    report->string += "</h1></center>\n";
    report->string += "<center><h2>Chart of Accounts</h2></center>\n";
    report->string += "<center><h3>As of " + date.toString("MMM d, yyyy") + "</h3></center>\n";
    report->string += "<table><tr><th>Account ID</th><th>Account Description</th></tr>\n";
    
    while(query.next())
    {
        report->csv += query.value(0).toString() + "," + query.value(1).toString() + "\n";
        report->string += "<tr><td>" + query.value(0).toString() + "</td><td>" + query.value(1).toString() + "</td></tr>\n";
    }
    
    report->string += "</table></qt>";
    
    report->html = report->string.replace(
            "<qt>", "<html><head><title>Chart of Accounts</title></head><body>\n").replace(
            "</qt>", "</body></html>");
    
    return this;
}

QString& Database::reportString()
{
    return report->string;
}

QString& Database::reportCSV()
{
    return report->csv;
}

QString& Database::reportHTML()
{
    return report->html;
}

QStringList& Database::getAccountsList()
{
    accountsList->clear();
    
    QSqlQuery query("SELECT * FROM accounts", db);
    while(query.next())
        accountsList->append(query.value(0).toString() + " - " + query.value(1).toString());
    accountsList->sort();
        
    return *accountsList;
    
}


QSqlDatabase *Database::db = 0;
QString Database::curClient = "";
QString Database::curDb = "";
Database::ReportStruct *Database::report = 0;
int Database::instances = 0;
QStringList *Database::accountsList = 0;


