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
#include <qregexp.h>

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
    
    if(curDb == "" || curClient == "")
    {
        QSettings settings;
        settings.setPath("eckroth.net", "GeneralLedger");
        settings.beginGroup("/GeneralLedger");
        curDb = settings.readEntry("/database/defaultDb", "");
        curClient = settings.readEntry("/database/defaultClient", "");
        settings.endGroup();
    }
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

bool Database::createNew(QString name, QString yearEnd)
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
        
    QSqlQuery createMain("CREATE TABLE main (name, yearEnd)", db);
    QSqlQuery createAccounts("CREATE TABLE accounts (key, id, desc, type, PRIMARY KEY (key))", db);
    QSqlQuery createJournal("CREATE TABLE journal (date, account, reference, "
            "desc, debit, credit)", db);
    QSqlQuery createJournalTmp("CREATE TABLE journalTmp (key, date, account, reference, "
            "desc, debit, credit)", db);
    QSqlQuery createBalanceAssets("CREATE TABLE balanceAssets (key, type, desc, begin, end)", db);
    QSqlQuery createBalanceLiabilities("CREATE TABLE balanceLiabilities (key, type, desc, begin, end)", db);
    QSqlQuery createBalanceEquities("CREATE TABLE balanceEquities (key, type, desc, begin, end)", db);
    
    QSqlQuery("INSERT INTO main VALUES(\"" + name + "\", \"" + yearEnd + "\")", db);
    
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
            destStream << "Account ID,Account Description,Account Type\n";
        
            while(query.next())
                destStream << query.value(1).toString() << "," << query.value(2).toString() << ","
                        << query.value(3).toString() << "\n";
        }
        if(table == "journalTmp")
        {
            destStream << "Date,Account,Reference,Description,Debit,Credit\n";
            
            while(query.next())
            {
                destStream << query.value(1).toString() << "," << query.value(2).toString() << ","
                        << query.value(3).toString() << "," << query.value(4).toString() << ","
                        << query.value(5).toString() << "," << query.value(6).toString() << "\n";
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
            
            if(i < 3)
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
            dataTable.setFocusStyle(QTable::FollowStyle);
            
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
                QSqlQuery query("SELECT key FROM " + table, db);
                int key = 0;
                while(query.next())
                {
                    if(key < query.value(0).toInt())
                        key = query.value(0).toInt();
                }
                key++;
                
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
                    
                    if(table == "accounts")
                    {
                        query.prepare("SELECT id FROM accounts");
                        query.exec();
                        
                        QRegExp regexp("^(\\d+)");
                        int pos = regexp.search(*it);
                        QString givenKey;
                        if(pos > -1)
                            givenKey = regexp.cap(1);
                        
                        bool isNew = true;
                        while(query.next())
                        {
                            if(givenKey == query.value(0).toString())
                                isNew = false;
                        }
                        
                        if(!isNew)
                        {
                            QString existingKey;
                            
                            query.prepare("SELECT key FROM accounts WHERE id = " + givenKey);
                            query.exec();
                            if(query.next())
                                existingKey = query.value(0).toString();
                            
                            query.prepare("INSERT OR REPLACE INTO accounts VALUES(\"" + existingKey +
                                    "\", \"" + *it + "\")");
                        }
                        else
                        {
                            QString newKey;
                            newKey.setNum(key++);
                            query.prepare("INSERT INTO accounts VALUES(\"" + newKey + "\", \"" + *it + "\")");
                        }
                    }
                    else
                    {
                        QString newKey;
                        newKey.setNum(key++);
                        query.prepare("INSERT INTO " + table + " VALUES(\"" + newKey + "\", \"" + *it + "\")");
                    }
                    query.exec();
                }
            }
        }
    }
    origFile.close();
}

QStringList Database::getPeriodBegin() const
{
    QStringList range;
    QString yearEnd;
    
    QDate date(QDate().currentDate());
    QString curYear = date.toString("yy");
    
    QSqlQuery query("SELECT yearEnd FROM main", db);
    if(query.next())
        yearEnd = query.value(0).toString();
    
    int startMonth = 0;
    
    if(yearEnd.startsWith("Jan"))
        startMonth = 2;
    if(yearEnd.startsWith("Feb"))
        startMonth = 3;
    if(yearEnd.startsWith("Mar"))
        startMonth = 4;
    if(yearEnd.startsWith("Apr"))
        startMonth = 5;
    if(yearEnd.startsWith("May"))
        startMonth = 6;
    if(yearEnd.startsWith("Jun"))
        startMonth = 7;
    if(yearEnd.startsWith("Jul"))
        startMonth = 8;
    if(yearEnd.startsWith("Aug"))
        startMonth = 9;
    if(yearEnd.startsWith("Sep"))
        startMonth = 10;
    if(yearEnd.startsWith("Oct"))
        startMonth = 11;
    if(yearEnd.startsWith("Nov"))
        startMonth = 12;
    if(yearEnd.startsWith("Dec"))
        startMonth = 1;
    
    if(startMonth > date.month())
    {
        curYear.setNum(curYear.toInt() - 1);
        if(curYear.toInt() < 10)
            curYear.insert(0, "0");
    }
        
    int period = 1;
    for(int i = startMonth; i <= 12; i++)
    {
        QString monthString;
        QString periodString;
        
        monthString.setNum(i);
        if(i < 10)
            monthString.insert(0, "0");
        
        range << "Period " + periodString.setNum(period++) + " (" + monthString + "/01/" + curYear + ")";
    }
    
    QString nextYear;
    nextYear.setNum(curYear.toInt() + 1);
    if(nextYear.toInt() < 10)
        nextYear.insert(0, "0");
    
    for(int i = 1; i < startMonth; i++)
    {
        QString monthString;
        QString periodString;

        monthString.setNum(i);        
        if(i < 10)
            monthString.insert(0, "0");
        
        range << "Period " + periodString.setNum(period++) + " (" + monthString + "/01/" + nextYear + ")";
    }
    
    return range;
}

QStringList Database::getPeriodEnd() const
{
    QStringList periodEnd;
    QStringList periodBegin = getPeriodBegin();
    
    QDate date(QDate().currentDate());
    
    QRegExp regexp("^(Period \\d+ )\\((\\d\\d)/\\d\\d/(\\d\\d)\\)");
    
    for(QStringList::Iterator it = periodBegin.begin(); it != periodBegin.end(); ++it)
    {
        QString periodString;
        
        QString month;
        QString year;
        
        if(regexp.search(*it) != -1)
        {
            periodString += regexp.cap(1);
            month = regexp.cap(2);
            year = regexp.cap(3);
            
            if(month == "01")
                periodString += "01/31/" + year + ")";
            if(month == "02")
            {
                QString testYear("20" + year);
                if(date.leapYear(testYear.toInt()))
                    periodString += "02/29/" + year + ")";
                else
                    periodString += "02/28/" + year + ")";
            }
            if(month == "03")
                periodString += "03/31/" + year + ")";
            if(month == "04")
                periodString += "04/30/" + year + ")";
            if(month == "05")
                periodString += "05/31/" + year + ")";
            if(month == "06")
                periodString += "06/30/" + year + ")";
            if(month == "07")
                periodString += "07/31/" + year + ")";
            if(month == "08")
                periodString += "08/31/" + year + ")";
            if(month == "09")
                periodString += "09/30/" + year + ")";
            if(month == "10")
                periodString += "10/31/" + year + ")";
            if(month == "11")
                periodString += "11/30/" + year + ")";
            if(month == "12")
                periodString += "12/31/" + year + ")";
        }
        
        periodEnd << periodString;
    }
    
    return periodEnd;
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
            query.prepare("SELECT * FROM balanceAssets ORDER BY key");
            break;
        case liabilities:
            query.prepare("SELECT * FROM balanceLiabilities ORDER BY key");
            break;
        case equities:
            query.prepare("SELECT * FROM balanceEquities ORDER BY key");
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
    
    QSqlQuery queryLast("SELECT key FROM " + table + " ORDER BY key");
    
    QString oneUp;
    if(queryLast.last())
        oneUp.setNum(queryLast.value(0).toInt() + 1);
    else
        oneUp.setNum(1);
    
    if(type == "0")
    {
        QSqlQuery queryDesc("SELECT desc FROM accounts WHERE id = \"" + accountBegin + "\"", db);
        if(queryDesc.next())
            desc = queryDesc.value(0).toString();
    }
    
    QSqlQuery queryInsert("INSERT INTO " + table + " VALUES( \"" + oneUp + "\", \"" +
            type + "\", \"" + desc + "\", \"" + accountBegin + "\", \"" + accountEnd + "\")", db);
}

void Database::removeBalanceElement(balanceCategory category, QString key)
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
            query.prepare("DELETE FROM balanceAssets WHERE key = " + key);
            break;
        case liabilities:
            query.prepare("DELETE FROM balanceLiabilities WHERE key = " + key);
            break;
        case equities:
            query.prepare("DELETE FROM balanceEquities WHERE key = " + key);
            break;
    }
    query.exec();
}

void Database::moveBalanceElement(balanceCategory category, QString key, bool up)
{
    if(key == "1" && up) return;
    
    if(!db->open())
    {
        db->lastError().showMessage();
        return;
    }
    
    QString oneUp;
    oneUp.setNum(key.toInt() - 1);
    QString oneDown;
    oneDown.setNum(key.toInt() + 1);

    QString queryLastString("SELECT key FROM ");
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

    queryLastString += table + " ORDER BY key";
    QSqlQuery queryLast(queryLastString, db);
    queryLast.last();
    if(queryLast.value(0).toString() == key && !up) return;
    
    queryUpdateString += table + " SET key = ";
    
    QString upOrDown;
    if(up)
        upOrDown = oneUp;
    else
        upOrDown = oneDown;
    queryUpdateString += "0 WHERE key = ";
    queryUpdateString += key;

    QSqlQuery queryUpdate(queryUpdateString, db);
    
    QSqlQuery queryUpdate2("UPDATE " + table + " SET key = " + key + " WHERE key = " + upOrDown, db);
    QSqlQuery queryUpdate3("UPDATE " + table + " SET key = " + upOrDown + " WHERE key = 0", db);
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
        report->csv += query.value(1).toString() + "," + query.value(2).toString() + "\n";
        report->string += "<tr><td>" + query.value(1).toString() + "</td><td>" + query.value(2).toString() + "</td></tr>\n";
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
        accountsList->append(query.value(1).toString() + " - " + query.value(2).toString());
    accountsList->sort();
        
    return *accountsList;
    
}

QPtrList<QStringList> Database::getAccountsData()
{
    QPtrList<QStringList> data;
    
    QSqlQuery query("SELECT * FROM accounts", db);
    while(query.next())
    {
        QStringList *list = new QStringList;
        *list << query.value(0).toString() << query.value(1).toString() << query.value(2).toString();
        data.append(list);
    }
    return data;
}

QPtrList<QStringList> Database::getJournalTmpData()
{
    QPtrList<QStringList> data;
    
    QSqlQuery query("SELECT * FROM journalTmp", db);
    while(query.next())
    {
        QStringList *list = new QStringList;
        *list << query.value(0).toString() << query.value(1).toString() << query.value(2).toString()
                << query.value(3).toString() << query.value(4).toString() << query.value(5).toString()
                << query.value(6).toString();
        data.append(list);
    }
    return data;
}

void Database::updateAccount(QString key, QString id, QString desc, QString type)
{
    QSqlQuery query("SELECT key FROM accounts", db);
    
    QString queryString;
    
    while(query.next())
    {
        if(key == query.value(0).toString())
        {
            queryString = "UPDATE accounts SET id = " + id + ", desc = \"" + desc + "\", type = \"" +
                    type + "\" WHERE key = " + key;
            break;
        }
    }
    
    if(queryString == "")
        queryString = "INSERT INTO accounts VALUES(\"" + key + "\", \"" + id + "\", \"" + desc + "\", \"" + type + "\")";
    
    query.prepare(queryString);
    query.exec();
    
    emit accountsChanged();
}

void Database::deleteAccount(QString key)
{
    QSqlQuery query("DELETE FROM accounts WHERE key = \"" + key + "\"", db);
    
    emit accountsChanged();
}

void Database::updateJournalTmp(QString key, QString date, QString account,
                                QString reference, QString desc, QString debit, QString credit)
{
    QSqlQuery query("SELECT key FROM journalTmp", db);
    
    QString queryString;
    
    while(query.next())
    {
        if(key == query.value(0).toString())
        {
            queryString = "UPDATE journalTmp SET date = \"" + date + "\", account = \"" + account +
                    "\", reference = \"" + reference + "\", desc = \"" + desc + "\", debit = \"" +
                    debit + "\", credit = \"" + credit + "\" WHERE key =  " + key;
            break;
        }
    }
    
    if(queryString == "")
        queryString = "INSERT INTO journalTmp VALUES(\"" + key + "\", \"" + date + "\", \"" +
                account + "\", \"" + reference + "\", \"" + desc + "\", \"" + debit +
                "\", \"" + credit + "\")";
    
    query.prepare(queryString);
    query.exec();
}

void Database::deleteJournalTmp(QString key)
{
    QSqlQuery query("DELETE FROM journalTmp WHERE key = " + key, db);
}

QString Database::nextAccountKey()
{
    QSqlQuery query("SELECT key FROM accounts", db);
    
    int key = 0;
    while(query.next())
    {
        if(key < query.value(0).toInt())
            key = query.value(0).toInt();
    }
    QString keyString;
    keyString.setNum(++key);
    
    return keyString;
}

QString Database::nextJournalTmpKey()
{
    QSqlQuery query("SELECT key FROM journalTmp", db);
    
    int key = 0;
    while(query.next())
    {
        if(key < query.value(0).toInt())
            key = query.value(0).toInt();
    }
    QString keyString;
    keyString.setNum(++key);
    
    return keyString;
}


QSqlDatabase *Database::db = 0;
QString Database::curClient = "";
QString Database::curDb = "";
Database::ReportStruct *Database::report = 0;
int Database::instances = 0;
QStringList *Database::accountsList = 0;


