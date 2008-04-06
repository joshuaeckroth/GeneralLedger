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
#include "settings.h"
#include "balanceElement.h"

Database::Database() : QObject()
{
#ifdef Q_WS_WIN
    connection = QSqlDatabase::addDatabase("QSQLITEX");
#else
    connection = QSqlDatabase::addDatabase("QSQLITE");
#endif
    report = new ReportStruct;
    accountsList = new QStringList;

    settings = Settings::instance();
    
    curDb = settings->getDefaultDb();
    curClient = settings->getDefaultClient();
}

Database::~Database()
{
    delete report;
    delete accountsList;

    settings->setDefaultDb(curDb);
    settings->setDefaultClient(curClient);
}

Database* Database::instance()
{
    if(db == 0)
        db = new Database;
    return db;
}

void Database::destroy()
{
    if(db != 0)
        delete db;
}

bool Database::openDefault()
{
    if(!QFile("data/" + curDb).exists())
        return false;
    
    
    
    if(connection->isOpen()) connection->close();
    connection->setDatabaseName("data/" + curDb);
  
    if(!connection->open())
    {
        connection->lastError().showMessage();
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
    
    if(connection->isOpen()) connection->close();
    connection->setDatabaseName("data/" + file);
    if(!connection->open())
    {
        connection->lastError().showMessage();
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
    
    if(connection->isOpen()) connection->close();
    connection->setDatabaseName("data/" + file);
    if (!connection->open())
    {
        connection->lastError().showMessage();
        return false;
    }
    
    curDb = file;
    curClient = name;
        
    QSqlQuery createMain("CREATE TABLE main (name, yearEnd)", connection);
    QSqlQuery createAccounts("CREATE TABLE accounts (key, id, desc, type, PRIMARY KEY (key))", connection);
    QSqlQuery createJournal("CREATE TABLE journal (date, account, reference, "
            "desc, debit, credit)", connection);
    QSqlQuery createJournalTmp("CREATE TABLE journalTmp (key, date, account, reference, "
            "desc, debit, credit)", connection);
    QSqlQuery createBalanceAssets("CREATE TABLE balanceAssets (key, type, desc, begin, end)", connection);
    QSqlQuery createBalanceLiabilities("CREATE TABLE balanceLiabilities (key, type, desc, begin, end)", connection);
    QSqlQuery createBalanceEquities("CREATE TABLE balanceEquities (key, type, desc, begin, end)", connection);
    
    QSqlQuery("INSERT INTO main VALUES(\"" + name + "\", \"" + yearEnd + "\")", connection);
    
    return true;
}

void Database::closeDb()
{
  if(connection->isOpen()) connection->close();
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
    if(!connection->open())
    {
        connection->lastError().showMessage();
        return;
    }
    
    QSqlQuery update("UPDATE main SET name = '" + name + "'", connection);
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
        QSqlDatabase *connectionTemp = QSqlDatabase::addDatabase("QSQLITEX", "temp");
#else
        QSqlDatabase *connectionTemp = QSqlDatabase::addDatabase("QSQLITE", "temp");
#endif 
    
    QDir dir("data");
    QStringList::Iterator it;
    QStringList files = dir.entryList("*.db", QDir::Files);
    it = files.begin();
    while(it != files.end())
    {
        connectionTemp->setDatabaseName("data/" + *it);
        if(connectionTemp->open())
        {
            QSqlQuery query("SELECT name FROM main", connectionTemp);
            if(query.next())
                list << QString(query.value(0).toString() + "    (" + *it + ")");
            connectionTemp->close();
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
                QSqlQuery query("SELECT key FROM " + table, connection);
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
    
    QSqlQuery query("SELECT yearEnd FROM main", connection);
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
    
    QRegExp regexp("^(Period \\d+ \\()(\\d\\d)/\\d\\d/(\\d\\d)\\)");
    
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

    if(!connection->open())
    {
        connection->lastError().showMessage();
        return tmpList;
    }
    
    QSqlQuery query(QString::null, connection);
    
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
    if(!connection->open())
    {
        connection->lastError().showMessage();
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
        QSqlQuery queryDesc("SELECT desc FROM accounts WHERE id = \"" + accountBegin + "\"", connection);
        if(queryDesc.next())
            desc = queryDesc.value(0).toString();
    }
    
    QSqlQuery queryInsert("INSERT INTO " + table + " VALUES( \"" + oneUp + "\", \"" +
            type + "\", \"" + desc + "\", \"" + accountBegin + "\", \"" + accountEnd + "\")", connection);
}

void Database::removeBalanceElement(balanceCategory category, QString key)
{
    if(!connection->open())
    {
        connection->lastError().showMessage();
        return;
    }
    
    QSqlQuery query(QString::null, connection);
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
    
    if(!connection->open())
    {
        connection->lastError().showMessage();
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
    QSqlQuery queryLast(queryLastString, connection);
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

    QSqlQuery queryUpdate(queryUpdateString, connection);
    
    QSqlQuery queryUpdate2("UPDATE " + table + " SET key = " + key + " WHERE key = " + upOrDown, connection);
    QSqlQuery queryUpdate3("UPDATE " + table + " SET key = " + upOrDown + " WHERE key = 0", connection);
}

Database* Database::getGeneralDetail(QString periodBegin, QString periodEnd,
                                     QString accountBegin, QString accountEnd)
{
    QStringList accounts;
    
    bool allAccounts = false;
    if(accountBegin == "" && accountEnd == "")
        allAccounts = true;
    
    QSqlQuery query("SELECT * FROM accounts", connection);
    while(query.next())
    {
        if(!allAccounts)
        {
            if(query.value(1).toInt() < accountBegin.toInt()
               || query.value(1).toInt() > accountEnd.toInt())
                continue;
        }
        
        accounts << query.value(1).toString() + "," + query.value(2).toString();
    }
    accounts.sort();
    
    QRegExp dateRegexp("Period \\d+ \\((\\d\\d)/(\\d\\d)/(\\d\\d)\\)");
    
    dateRegexp.search(periodBegin);
    QDate periodBeginDate(dateRegexp.cap(3).insert(0,"20").toInt(), dateRegexp.cap(1).toInt(), dateRegexp.cap(2).toInt());
    
    dateRegexp.search(periodEnd);
    QDate periodEndDate(dateRegexp.cap(3).insert(0,"20").toInt(), dateRegexp.cap(1).toInt(), dateRegexp.cap(2).toInt());
    
    report->csv = "Account ID,Account Description,Date,Reference,Description,Debit Amt,Credit Amt,Balance\n";
    report->header = "<center><h1>";
    report->header += curClient;
    report->header += "</h1></center>\n";
    report->header += "<center><h2>General Ledger Detail</h2></center>\n";
    report->header += "<center><h3>For the Period From " + periodBeginDate.toString("MMM d, yyyy")
            + " to " + periodEndDate.toString("MMM d, yyyy") + "</h3></center>\n";
    
    if(allAccounts)
        report->header += "<center><h4>All Accounts</h4></center>\n";
    else
        report->header += "<center><h4>Account Range: " + accountBegin + " to " + accountEnd + "</h4></center>\n";
    
    report->data = "<table><tr>"
            "<td><b>Account ID<br>Account Description</b></td>"
            "<td><b>Date</b></td>"
            "<td><b>Reference</b></td>"
            "<td><b>Description</b></td>"
            "<td align=\"right\"><b>Debit Amt</b></td>"
            "<td align=\"right\"><b>Credit Amt</b></td>"
            "<td align=\"right\"><b>Balance</b></td></tr>\n";
    
    QString beginningBalance;
    QString periodChangeBalance;
    QString endingBalance;
    QString debitBalance;
    QString creditBalance;
    
    QString entryYear;
    QString entryMonth;
    QString entryDay;
    QDate entryDate;
    
    bool inDateRange;
    
    QDate curDate(QDate().currentDate());
    
    for(QStringList::Iterator it = accounts.begin(); it != accounts.end(); ++it)
    {
        query.prepare("SELECT date, reference, desc, debit, credit FROM journal WHERE account = " + (*it).left(4));
        query.exec();
        
        report->data += "<tr><td>" + (*it).left(4) + "</td><td>" + periodBeginDate.toString("MM/dd/yy") +
                "</td><td> </td><td>Beginning Balance</td><td> </td><td> </td><td align=\"right\">(BALANCE)</td></tr>";
        
        bool accountDescDisplayed = false;
        
        report->csv += (*it).left(4) + "," + (*it).mid(5) + "," + periodBeginDate.toString("MM/dd/yy") +
                ",,Beginning Balance,,,(BALANCE)\n";
        
        beginningBalance = "0.00";
        periodChangeBalance = "0.00";
        endingBalance = "0.00";
        debitBalance = "0.00";
        creditBalance = "0.00";
        
        if(query.first())
        {
            query.seek(-1);
            
            inDateRange = false;
            
            while(query.next())
            {
                QString entryYear = query.value(0).toString().right(2);
                    
                if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                    entryYear.insert(0, "20");
                else
                    entryYear.insert(0, "19");
                
                QString entryMonth = query.value(0).toString().left(2);
                QString entryDay = query.value(0).toString().mid(3,2);
                
                entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                
                if(entryDate >= periodBeginDate && entryDate <= periodEndDate)
                {
                    inDateRange = true;
                    
                    if(!accountDescDisplayed)
                    {
                        report->data += "<tr><td>" + (*it).mid(5) + "</td>";
                        accountDescDisplayed = true;
                    }
                    else
                        report->data += "<tr><td> </td>";
                    
                    report->data += "<td>" + query.value(0).toString() + "</td><td>"
                            + query.value(1).toString() + "</td><td>" + query.value(2).toString()
                            + "</td><td align=\"right\">" + query.value(3).toString() + "</td><td align=\"right\">"
                            + query.value(4).toString() + "</td><td> </td></tr>\n";
                    
                    report->csv += (*it).left(4) + "," + (*it).mid(5) + ","
                            + query.value(0).toString() + "," + query.value(1).toString()
                            + "," + query.value(2).toString() + "," + query.value(3).toString()
                            + "," + query.value(4).toString() + ",\n";
                    
                    debitBalance = addCurrency(debitBalance, query.value(3).toString());
                    creditBalance = addCurrency(creditBalance, query.value(4).toString());
                    
                    periodChangeBalance = addCurrency(debitBalance, QString(creditBalance).insert(0,"-"));
            
                }
                if(entryDate < periodBeginDate)
                {
                    beginningBalance = addCurrency(beginningBalance, query.value(3).toString());
                    beginningBalance = addCurrency(beginningBalance, QString("-" + query.value(4).toString()));
                }
            }
            
            if(inDateRange)
            {   
                report->data += "<tr><td> </td><td> </td><td> </td>"
                        "<td>Current Period Change</td><td align=\"right\">"
                        + debitBalance + "</td><td align=\"right\">"
                        + creditBalance + "</td><td align=\"right\">" + periodChangeBalance + "</td></tr>\n";
                
                report->csv += ",,,Current Period Change," + debitBalance + "," + creditBalance
                        + "," + periodChangeBalance + "\n";
            }
        }
        else
        {
            report->data += "<tr><td>" + (*it).mid(5) + "</td></tr>\n";
            accountDescDisplayed = true;
        }
        
        if(!accountDescDisplayed)
            report->data += "<tr><td>" + (*it).mid(5) + "</td></tr>";
        
        if(beginningBalance == "0.00" && debitBalance == "0.00" && creditBalance == "0.00")
            endingBalance = "";
        else
            endingBalance = addCurrency(beginningBalance, periodChangeBalance);
        
        if(beginningBalance == "0.00")
            beginningBalance = "";
        
        report->data = (report->data).replace("(BALANCE)", beginningBalance);
        report->csv = (report->csv).replace("(BALANCE)", beginningBalance);
        
        report->data += "<tr><td> </td><td><b>" + periodEndDate.toString("MM/dd/yy") + "</b></td>"
                "<td> </td><td><b>Ending Balance</b></td><td> </td><td> </td><td align=\"right\"><b>"
                + endingBalance + "</b></td></tr><tr><td> </td></tr><tr><td> </td></tr>\n";
        
        report->csv += "," + periodEndDate.toString("MM/dd/yy") + ",,Ending Balance,,,"
                + endingBalance + "\n";
        
    }

    report->data += "</table>";
    
    report->string = "<qt>" + report->header + report->data + "</qt>";
    
    report->html = report->string.replace(
            "<qt>", "<html><head><title>" + curClient +
            " - General Ledger Detail</title></head><body>\n").replace(
                    "</qt>", "</body></html>");
    
    return this;
}

Database* Database::getGeneralTrialBalance(QString periodEnd, QString accountBegin, QString accountEnd)
{
    QStringList accounts;
    
    bool allAccounts = false;
    if(accountBegin == "" && accountEnd == "")
        allAccounts = true;
    
    QSqlQuery query("SELECT * FROM accounts", connection);
    while(query.next())
    {
        if(!allAccounts)
        {
            if(query.value(1).toInt() < accountBegin.toInt()
                || query.value(1).toInt() > accountEnd.toInt())
                continue;
        }
        
        accounts << query.value(1).toString() + "," + query.value(2).toString();
    }
    accounts.sort();
    
    QDate curDate(QDate().currentDate());
    
    QRegExp dateRegexp("Period \\d+ \\((\\d\\d)/(\\d\\d)/(\\d\\d)\\)");
    dateRegexp.search(periodEnd);
    
    QDate periodDate(dateRegexp.cap(3).insert(0,"20").toInt(), dateRegexp.cap(1).toInt(), dateRegexp.cap(2).toInt());
    
    report->csv = "Account ID,Account Description,Debit Amt,Credit Amt\n";
    report->header = "<center><h1>";
    report->header += curClient;
    report->header += "</h1></center>\n";
    report->header += "<center><h2>General Ledger Trial Balance</h2></center>\n";
    report->header += "<center><h3>As of " + periodDate.toString("MMM d, yyyy") + "</h3></center>\n";
    
    if(allAccounts)
        report->header += "<center><h4>All Accounts</h4></center>\n";
    else
        report->header += "<center><h4>Account Range: " + accountBegin + " to " + accountEnd + "</h4></center>\n";
    
    report->data = "<table><tr><td><b>Account ID</b></td>"
            "<td><b>Account Description</b></td>"
            "<td align=\"right\"><b>Debit Amt</b></td><td align=\"right\"><b>Credit Amt</b></td></tr>\n";
    
    QString debit = "0.00";
    QString credit = "0.00";
    QString totalDebit = "0.00";
    QString totalCredit = "0.00";
    
    QString entryYear;
    QString entryMonth;
    QString entryDay;
    QDate entryDate;
    
    for(QStringList::Iterator it = accounts.begin(); it != accounts.end(); ++it)
    {
        query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + (*it).left(4));
        query.exec();
        
        report->data += "<tr><td>" + (*it).left(4) + "</td><td>" + (*it).mid(5)
                + "</td><td align=\"right\">";
        report->csv += (*it).left(4) + "," + (*it).mid(5) + ",";
        
        if(query.first())
        {
            debit = "0.00";
            credit = "0.00";
            
            query.seek(-1);
            while(query.next())
            {
                QString entryYear = query.value(0).toString().right(2);
                
                if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                    entryYear.insert(0, "20");
                else
                    entryYear.insert(0, "19");
                
                QString entryMonth = query.value(0).toString().left(2);
                QString entryDay = query.value(0).toString().mid(3,2);
                
                entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                if(entryDate > periodDate)
                    continue;
                
                debit = addCurrency(debit, query.value(1).toString());
                credit = addCurrency(credit, query.value(2).toString());
            }
            
            totalDebit = addCurrency(totalDebit, debit);
            totalCredit = addCurrency(totalCredit, credit);
            
            if(debit != "0.00")
                report->data += debit;
            report->data += "</td><td align=\"right\">";
            if(credit != "0.00")
                report->data += credit;
            report->data += "</td></tr>\n";
            
            report->csv += debit + "," + credit + "\n";
        }
        else
        {
            report->data += "</td><td> </td></tr>\n";
            report->csv += ",\n";
        }
    }
    
    report->data += "<tr><td> </td><td> </td><td> </td><td> </td>"
            "</tr><tr><td> </td><td><b>Total:</b></td><td align=\"right\"><b>" + totalDebit
            + "</b></td><td align=\"right\"><b>" + totalCredit + "</b></td></tr>\n</table>";
    
    
    report->string = "<qt>" + report->header + report->data + "</qt>";
    
    report->html = report->string.replace(
            "<qt>", "<html><head><title>" + curClient +
            " - General Ledger Trial Balance</title></head><body>\n").replace(
            "</qt>", "</body></html>");
    
    return this;
}

Database* Database::getBalanceReport(QString periodEnd)
{

    QRegExp dateRegexp("Period \\d+ \\((\\d\\d)/(\\d\\d)/(\\d\\d)\\)");
    dateRegexp.search(periodEnd);
    
    QDate curDate(QDate().currentDate());
    QDate periodDate(dateRegexp.cap(3).insert(0,"20").toInt(), dateRegexp.cap(1).toInt(), dateRegexp.cap(2).toInt());
    QString entryYear;
    QString entryMonth;
    QString entryDay;
    QDate entryDate;

    report->csv = "Account Description,Balance\n";
    report->header = "<center><h1>";
    report->header += curClient;
    report->header += "</h1></center>\n";
    report->header += "<center><h2>Balance Sheet</h2></center>\n";
    report->header += "<center><h3>For the Period Ending " + periodDate.toString("MMM d, yyyy") + "</h3></center>\n";

    report->data = "<table><tr><td align=\"center\"><b>Account Description</b></td>"
            "<td align=\"right\"><b>Balance</b></td></tr>\n";

    QPtrList<QStringList> assetsList;
    QPtrList<QStringList> liabilitiesList;
    QPtrList<QStringList> equitiesList;

    QSqlQuery query("SELECT * FROM balanceAssets", connection);
    while(query.next())
    {
        QStringList *list = new QStringList;
        *list << query.value(1).toString() << query.value(2).toString()
                << query.value(3).toString() << query.value(4).toString();
        assetsList.append(list);
    }

    query.prepare("SELECT * FROM balanceLiabilities");
    query.exec();
    while(query.next())
    {
        QStringList *list = new QStringList;
        *list << query.value(1).toString() << query.value(2).toString()
                << query.value(3).toString() << query.value(4).toString();
        liabilitiesList.append(list);
    }

    query.prepare("SELECT * FROM balanceEquities");
    query.exec();
    while(query.next())
    {
        QStringList *list = new QStringList;
        *list << query.value(1).toString() << query.value(2).toString()
                << query.value(3).toString() << query.value(4).toString();
        equitiesList.append(list);
    }


    QString sectionBalance;
    QString accountBalance;

    QStringList *list;
    sectionBalance = "0.00";
    for(list = assetsList.first(); list; list = assetsList.next())
    {

        accountBalance = "0.00";
        if(list->operator[](0) == "0")  // one account
        {
            query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + list->operator[](2));
            query.exec();
            while(query.next())
            {
                QString entryYear = query.value(0).toString().right(2);
                
                if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                    entryYear.insert(0, "20");
                else
                    entryYear.insert(0, "19");
                
                QString entryMonth = query.value(0).toString().left(2);
                QString entryDay = query.value(0).toString().mid(3,2);
                
                entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                if(entryDate > periodDate)
                    continue;

                if(query.value(1).toString() != "")
                {
                    accountBalance = addCurrency(accountBalance, query.value(1).toString());
                }
                if(query.value(2).toString() != "")
                {
                    accountBalance = addCurrency(accountBalance, QString("-" + query.value(2).toString()));
                }
            }
        }
        else  // begin and end accounts
        {
            QStringList accounts;

            query.prepare("SELECT id FROM accounts");
            query.exec();
            while(query.next())
            {
                if(query.value(0).toInt() >= (list->operator[](2)).toInt()
                   && query.value(0).toInt() <= (list->operator[](3)).toInt())
                    accounts << query.value(0).toString();
            }

            for(QStringList::Iterator it = accounts.begin(); it != accounts.end(); ++it)
            {
                query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + *it);
                query.exec();
                while(query.next())
                {
                    QString entryYear = query.value(0).toString().right(2);
                
                    if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                        entryYear.insert(0, "20");
                    else
                        entryYear.insert(0, "19");
                
                    QString entryMonth = query.value(0).toString().left(2);
                    QString entryDay = query.value(0).toString().mid(3,2);
                
                    entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                    if(entryDate > periodDate)
                        continue;

                    if(query.value(1).toString() != "")
                        accountBalance = addCurrency(accountBalance, query.value(1).toString());
                    if(query.value(2).toString() != "")
                        accountBalance = addCurrency(accountBalance, QString("-" + query.value(2).toString()));
                }
            }
        }

        sectionBalance = addCurrency(sectionBalance, accountBalance);

        report->data += "<tr><td>" + list->operator[](1) + "</td><td align=\"right\">" + accountBalance + "</td></tr>\n";
        report->csv += list->operator[](1) + "," + accountBalance + "\n";
    }

    report->data += "<tr><td> </td></tr>\n";
    report->data += "<tr><td><b>Total Assets</b></td><td align=\"right\"><b>" + sectionBalance + "</b></td></tr>\n";
    report->data += "<tr><td> </td></tr><tr><td> </td></tr>\n";
    report->csv += "Total Assets," + sectionBalance + "\n";


    sectionBalance = "0.00";
    for(list = liabilitiesList.first(); list; list = liabilitiesList.next())
    {

        accountBalance = "0.00";
        if(list->operator[](0) == "0")  // one account
        {
            query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + list->operator[](2));
            query.exec();
            while(query.next())
            {
                QString entryYear = query.value(0).toString().right(2);
                
                if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                    entryYear.insert(0, "20");
                else
                    entryYear.insert(0, "19");
                
                QString entryMonth = query.value(0).toString().left(2);
                QString entryDay = query.value(0).toString().mid(3,2);

                entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                if(entryDate > periodDate)
                    continue;

                if(query.value(1).toString() != "")
                {
                    accountBalance = addCurrency(accountBalance, query.value(1).toString());
                }
                if(query.value(2).toString() != "")
                {
                    accountBalance = addCurrency(accountBalance, QString("-" + query.value(2).toString()));
                }
            }
        }
        else  // begin and end accounts
        {
            QStringList accounts;

            query.prepare("SELECT id FROM accounts");
            query.exec();
            while(query.next())
            {
                if(query.value(0).toInt() >= (list->operator[](2)).toInt()
                   && query.value(0).toInt() <= (list->operator[](3)).toInt())
                    accounts << query.value(0).toString();
            }

            for(QStringList::Iterator it = accounts.begin(); it != accounts.end(); ++it)
            {
                query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + *it);
                query.exec();
                while(query.next())
                {
                    QString entryYear = query.value(0).toString().right(2);
                
                    if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                        entryYear.insert(0, "20");
                    else
                        entryYear.insert(0, "19");
                
                    QString entryMonth = query.value(0).toString().left(2);
                    QString entryDay = query.value(0).toString().mid(3,2);

                    entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                    if(entryDate > periodDate)
                        continue;

                    if(query.value(1).toString() != "")
                        accountBalance = addCurrency(accountBalance, query.value(1).toString());
                    if(query.value(2).toString() != "")
                        accountBalance = addCurrency(accountBalance, QString("-" + query.value(2).toString()));
                }
            }
        }

        sectionBalance = addCurrency(sectionBalance, accountBalance);

        report->data += "<tr><td>" + list->operator[](1) + "</td><td align=\"right\">" + accountBalance + "</td></tr>\n";
        report->csv += list->operator[](1) + "," + accountBalance + "\n";
    }

    report->data += "<tr><td> </td></tr>\n";
    report->data += "<tr><td><b>Total Liabilities</b></td><td align=\"right\"><b>" + sectionBalance + "</b></td></tr>\n";
    report->data += "<tr><td> </td></tr><tr><td> </td></tr>\n";
    report->csv += "Total Liabilities," + sectionBalance + "\n";


    sectionBalance = "0.00";
    for(list = equitiesList.first(); list; list = equitiesList.next())
    {

        accountBalance = "0.00";
        if(list->operator[](0) == "0")  // one account
        {
            query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + list->operator[](2));
            query.exec();
            while(query.next())
            {
                QString entryYear = query.value(0).toString().right(2);
                
                if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                    entryYear.insert(0, "20");
                else
                    entryYear.insert(0, "19");
                
                QString entryMonth = query.value(0).toString().left(2);
                QString entryDay = query.value(0).toString().mid(3,2);
                
                entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                if(entryDate > periodDate)
                    continue;

                if(query.value(1).toString() != "")
                {
                    accountBalance = addCurrency(accountBalance, query.value(1).toString());
                }
                if(query.value(2).toString() != "")
                {
                    accountBalance = addCurrency(accountBalance, QString("-" + query.value(2).toString()));
                }
            }
        }
        else  // begin and end accounts
        {
            QStringList accounts;

            query.prepare("SELECT id FROM accounts");
            query.exec();
            while(query.next())
            {
                if(query.value(0).toInt() >= (list->operator[](2)).toInt()
                   && query.value(0).toInt() <= (list->operator[](3)).toInt())
                    accounts << query.value(0).toString();
            }

            for(QStringList::Iterator it = accounts.begin(); it != accounts.end(); ++it)
            {
                query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + *it);
                query.exec();
                while(query.next())
                {
                    QString entryYear = query.value(0).toString().right(2);
                
                    if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                        entryYear.insert(0, "20");
                    else
                        entryYear.insert(0, "19");
                
                    QString entryMonth = query.value(0).toString().left(2);
                    QString entryDay = query.value(0).toString().mid(3,2);
                
                    entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                    if(entryDate > periodDate)
                        continue;

                    if(query.value(1).toString() != "")
                        accountBalance = addCurrency(accountBalance, query.value(1).toString());
                    if(query.value(2).toString() != "")
                        accountBalance = addCurrency(accountBalance, QString("-" + query.value(2).toString()));
                }
            }
        }

        sectionBalance = addCurrency(sectionBalance, accountBalance);

        report->data += "<tr><td>" + list->operator[](1) + "</td><td align=\"right\">" + accountBalance + "</td></tr>\n";
        report->csv += list->operator[](1) + "," + accountBalance + "\n";
    }
    
    report->data += "<tr><td> </td></tr>\n";
    report->data += "<tr><td><b>Total Equities</b></td><td align=\"right\"><b>" + sectionBalance + "</b></td></tr>\n";
    report->data += "<tr><td> </td></tr><tr><td> </td></tr>\n";
    report->csv += "Total Equities," + sectionBalance + "\n";

    report->data += "</table>";

    report->string = "<qt>" + report->header + report->data + "</qt>";
    
    report->html = report->string.replace(
            "<qt>", "<html><head><title>" + curClient +
            " - Balance Sheet</title></head><body>\n").replace(
                    "</qt>", "</body></html>");

    return this;
}

Database* Database::getChartAccounts()
{
    QSqlQuery query("SELECT * FROM accounts", connection);
    
    QDate date(QDate().currentDate());
    
    report->csv = "Account ID,Account Description\n";
    report->header = "<center><h1>";
    report->header += curClient;
    report->header += "</h1></center>\n";
    report->header += "<center><h2>Chart of Accounts</h2></center>\n";
    report->header += "<center><h3>As of " + date.toString("MMM d, yyyy") + "</h3></center>\n";

    report->data = "<table><tr><td align=\"center\"><b>Account ID</b></td>"
            "<td align=\"center\"><b>Account Description</b></td></tr>\n";
    
    QStringList reportString;
    QStringList reportCSV;
    
    while(query.next())
    {
        reportCSV << query.value(1).toString() + "," + query.value(2).toString() + "\n";
        reportString << "<tr><td>" + query.value(1).toString() + "</td><td>" +
                query.value(2).toString() + "</td></tr>\n";
    }
    
    reportString.sort();
    reportCSV.sort();
    
    report->data += reportString.join("") + "</table>";
    report->csv += reportCSV.join("");
    
    report->string = "<qt>" + report->header + report->data + "</qt>";
    
    report->html = report->string.replace(
            "<qt>", "<html><head><title>" + curClient + " - Chart of Accounts</title></head><body>\n").replace(
            "</qt>", "</body></html>");
    
    return this;
}

QString& Database::reportHeader()
{
    return report->header;
}

QString& Database::reportData()
{
    return report->data;
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
    
    QSqlQuery query("SELECT * FROM accounts", connection);
    while(query.next())
        accountsList->append(query.value(1).toString() + " - " + query.value(2).toString());
    accountsList->sort();
        
    return *accountsList;
    
}

QPtrList<QStringList> Database::getAccountsData()
{
    QPtrList<QStringList> data;
    
    QSqlQuery query("SELECT * FROM accounts", connection);
    while(query.next())
    {
        QStringList *list = new QStringList;
        *list << query.value(0).toString() << query.value(1).toString()
                << query.value(2).toString() << query.value(3).toString();
        data.append(list);
    }
    return data;
}

QPtrList<QStringList> Database::getJournalTmpData()
{
    QPtrList<QStringList> data;
    
    QSqlQuery query("SELECT * FROM journalTmp", connection);
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
    QSqlQuery query("SELECT key FROM accounts", connection);
    
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
    QSqlQuery query("DELETE FROM accounts WHERE key = \"" + key + "\"", connection);
    
    emit accountsChanged();
}

void Database::updateJournalTmp(QString key, QString date, QString account,
                                QString reference, QString desc, QString debit, QString credit)
{
    QSqlQuery query("SELECT key FROM journalTmp", connection);
    
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
    
    emit journalTmpChanged();
}

void Database::deleteJournalTmp(QString key)
{
    QSqlQuery query("DELETE FROM journalTmp WHERE key = " + key, connection);
}

QString Database::journalTmpDebit()
{
    QString debit("0.00");
    
    QSqlQuery query("SELECT debit FROM journalTmp", connection);
    
    while(query.next())
        debit = addCurrency(debit, query.value(0).toString());
    
    return debit;
}

QString Database::journalTmpCredit()
{
    QString credit("0.00");
    
    QSqlQuery query("SELECT credit FROM journalTmp", connection);
    
    while(query.next())
        credit = addCurrency(credit, query.value(0).toString());
    
    return credit;
}

QString Database::journalTmpBalance()
{
    QString balance;
    
    balance = addCurrency(journalTmpDebit(), journalTmpCredit().insert(0, "-"));
    
    return balance;
}

QString Database::addCurrency(QString base, QString add)
{
    // code adopted from "moncls.h" by donpa@linuxledgers.com
    
    QString resultCents;
    QString resultDollars;
    int cents;
    int dollars;
    
    QString baseCents;
    QString baseDollars;
    QString addCents;
    QString addDollars;
    
    baseCents = base.right(2);
    base.truncate(base.length() - 3);
    baseDollars = base;
    
    addCents = add.right(2);
    add.truncate(add.length() - 3);
    addDollars = add;
    
    if(baseDollars.left(1) == "-")
    {
        baseCents.setNum(baseCents.toInt() + 100);
        baseCents.insert(0, "-");
        baseDollars.setNum(baseDollars.toInt() + 1);
    }
    if(addDollars.left(1) == "-")
    {
        addCents.setNum(addCents.toInt() + 100);
        addCents.insert(0, "-");
        addDollars.setNum(addDollars.toInt() + 1);
    }
    
    dollars = (baseDollars.toInt() + addDollars.toInt());
    cents = (baseCents.toInt() + addCents.toInt());
    
    while(dollars > 0 && cents < 0)
    {
        dollars -= 1;
        cents += 100;
    }
    while(cents >= 100)
    {
        dollars += 1;
        cents -= 100;
    }
    while(cents <= -100)
    {
        dollars -= 1;
        cents += 100;
    }

    resultDollars.setNum(dollars);

    if(cents < 0)
    {
        cents = 0 - cents;
        if(dollars >= 0)
            resultDollars.insert(0, "-");
    }

    resultCents.setNum(cents);

    if(cents < 10)
        resultCents.insert(0, "0");
    
    return QString(resultDollars + "." + resultCents);   
}

void Database::commitJournalTmp()
{
    QStringList data;
    
    QSqlQuery query("SELECT * FROM journalTmp", connection);
    while(query.next())
    {
        data << "\"" + query.value(1).toString() + "\", \"" + query.value(2).toString()
                + "\", \"" + query.value(3).toString() + "\", \"" + query.value(4).toString()
                + "\", \"" + query.value(5).toString() + "\", \"" + query.value(6).toString() + "\"";
    }
    
    for(QStringList::Iterator it = data.begin(); it != data.end(); ++it)
    {
        query.prepare("INSERT INTO journal VALUES(" + *it + ")");
        query.exec();
    }
    
    query.prepare("DROP TABLE journalTmp");
    query.exec();
    
    query.prepare("CREATE TABLE journalTmp (key, date, account, reference, "
            "desc, debit, credit)");
    query.exec();
    
    emit journalTmpChanged();
}

QString Database::nextAccountKey()
{
    QSqlQuery query("SELECT key FROM accounts", connection);
    
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
    QSqlQuery query("SELECT key FROM journalTmp", connection);
    
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

Database *Database::db = 0;

