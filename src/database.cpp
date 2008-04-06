#include <sstream>
using std::ostringstream;

#include <cstdlib>
using std::system;
using std::div;

#include <cstdio>
using std::remove;

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

#include "config.h"
#include "database.h"
#include "settings.h"
#include "editorElement.h"

Database::Database() : QObject()
{
#ifdef Q_WS_WIN
    connection = QSqlDatabase::addDatabase("QSQLITEX");
#else
    connection = QSqlDatabase::addDatabase("QSQLITE");
#endif
    report = new ReportStruct;
    journalTmpReport = new JournalTmpReport;
    accountsList = new QStringList;

    settings = Settings::instance();

    clientPath = settings->getClientPath();
    
    curDb = settings->getDefaultDb();
    curClient = settings->getDefaultClient();
}

Database::~Database()
{
    closeDb();

    delete report;
    delete journalTmpReport;
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

bool Database::openNew(QString file, QString password)
{
    clientPath = settings->getClientPath();

    QString file2(file);

    if(password != QString::null)
    {
        file2.append(".db_");
        file.append(".dbx");

        QFile encDb(clientPath + "/" + file);
        if(!encDb.open( IO_ReadOnly ))
            return false;

        QFile encDb2(clientPath + "/" + file + "2");
        if(!encDb2.open( IO_WriteOnly ))
            return false;

        QDataStream encStream2(&encDb2);

        QDataStream encStream(&encDb);
        char *string;
        encStream >> string;
        delete string;

        char *byte = new char;
        while(!encStream.atEnd())
        {
            encStream.readRawBytes(byte, 1);
            encStream2.writeRawBytes(byte, 1);
        }
        delete byte;

        encStream.unsetDevice();
        encStream2.unsetDevice();
        encDb.close();
        encDb2.close();

        int retVal = system(QString(CRYPT_PATH + "/aescrypt2 1 \"" + clientPath + "/" + file + "2\" \""
                            + clientPath + "/" + file2 + "\" \"" + password + "\"").ascii());

        remove(QString(clientPath + "/" + file + "2").ascii());

        if(retVal != 0)
            return false;
    }
    else
        file2.append(".db");
    
    if(connection->isOpen()) connection->close();
    connection->setDatabaseName(clientPath + "/" + file2);
    if(!connection->open())
    {
        connection->lastError().showMessage();
        return false;
    }
    
    curDb = file2;
    
    QSqlQuery query("SELECT name FROM main");
    if(query.next())
        curClient = query.value(0).toString();
    else
        return false;
  
    return true;
}

bool Database::createNew(QString name, QString yearEnd)
{
    clientPath = settings->getClientPath();

    QString file(name);
    
    file.replace(QChar(' '), "_");
    file.replace(QChar('/'), "-");
    file.replace(QChar('\\'), "-");
    file.append(".db");
    
    if(connection->isOpen()) connection->close();
    connection->setDatabaseName(clientPath + "/" + file);
    if (!connection->open())
    {
        connection->lastError().showMessage();
        return false;
    }
    
    curDb = file;
    curClient = name;
        
    QSqlQuery createMain("CREATE TABLE main (name, yearEnd)", connection);
    QSqlQuery createAccounts("CREATE TABLE accounts (key, id, desc, PRIMARY KEY (key))", connection);
    QSqlQuery createAccountsIndex("CREATE INDEX accountsIndex ON accounts (id)", connection);
    QSqlQuery createJournal("CREATE TABLE journal (date, account, reference, "
            "desc, debit, credit)", connection);
    QSqlQuery createJournalTmp("CREATE TABLE journalTmp (key, date, account, reference, "
            "desc, debit, credit, PRIMARY KEY (key))", connection);
    QSqlQuery createBalanceAssets("CREATE TABLE balanceAssets (key, type, desc, begin, end)", connection);
    QSqlQuery createBalanceLiabilities("CREATE TABLE balanceLiabilities (key, type, desc, begin, end)", connection);
    QSqlQuery createBalanceEquities("CREATE TABLE balanceEquities (key, type, desc, begin, end)", connection);
    QSqlQuery createIncomeSvcIncome("CREATE TABLE incomeSvcIncome (key, type, desc, begin, end)", connection);
    QSqlQuery createIncomeSvcExpenses("CREATE TABLE incomeSvcExpenses (key, type, desc, begin, end)", connection);
    QSqlQuery createIncomeRetailIncome("CREATE TABLE incomeRetailIncome (key, type, desc, begin, end)", connection);
    QSqlQuery createIncomeRetailCOS("CREATE TABLE incomeRetailCOS (key, type, desc, begin, end)", connection);
    QSqlQuery createIncomeRetailExpenses("CREATE TABLE incomeRetailExpenses (key, type, desc, begin, end)", connection);
    
    QSqlQuery("INSERT INTO main VALUES(\"" + name + "\", \"" + yearEnd + "\")", connection);
    
    return true;
}

void Database::closeDb()
{
    if(connection->isOpen()) connection->close();

    report->header = QString::null;
    report->data = QString::null;
    report->string = QString::null;
    report->csv = QString::null;
    report->html = QString::null;

    journalTmpReport->header = QString::null;
    journalTmpReport->data = QString::null;

    if(curDb.right(4) == ".db_")
    {
        remove(QString(clientPath + "/" + curDb));
        remove(QString(clientPath + "/" + curDb.left(curDb.length() - 1)));
        curDb = curDb.left(curDb.length() - 1).append("x");
    }
}

void Database::copyDb(QString file) const
{
    QString origFile;
    bool encrypted;
    if(curDb.right(4) == ".db_")
    {
        encrypted = true;
        origFile = curDb.left(curDb.length() - 1).append("x");
    }
    else
    {
        encrypted = false;
        origFile = curDb;
    }

    QFile origDb(clientPath + "/" + origFile);

    if(file.right(3) == ".db" && encrypted)
        file = file.left(file.length() - 3).append(".dbx");
    if(file.right(4) == ".dbx" && !encrypted)
        file = file.left(file.length() - 4).append(".db");
    if(file.right(3) != ".db" && file.right(4) != ".dbx" && encrypted)
        file = file.append(".dbx");
    if(file.right(3) != ".db" && file.right(4) != ".dbx" && !encrypted)
        file = file.append(".db");

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
    
    while(!origDb.atEnd())
        newDb.putch(origDb.getch());
    
    newDb.close();
    origDb.close();
}

void Database::encryptDb(QString password)
{
    system(QString(CRYPT_PATH + "/aescrypt2 0 \"" + clientPath + "/" + curDb + "\" \""
            + clientPath + "/" + curDb + "x2\" \"" + password + "\"").ascii());

    QFile encDb2(clientPath + "/" + curDb + "x2");
    if(!encDb2.open( IO_ReadOnly ))
        return;
    QDataStream encStream2(&encDb2);

    QFile encDb(clientPath + "/" + curDb + "x");
    encDb.open( IO_WriteOnly );
    QDataStream encStream(&encDb);

    encStream << QString("enc:" + curClient).ascii();

    char *byte = new char;
    while(!encStream2.atEnd())
    {
        encStream2.readRawBytes(byte, 1);
        encStream.writeRawBytes(byte, 1);
    }
    delete byte;

    encStream.unsetDevice();
    encDb.close();
    encStream2.unsetDevice();
    encDb2.close();

    remove(QString(clientPath + "/" + curDb + "x2").ascii());

    QString oldCurDb = curDb;

    openNew(curClient, password);

    remove(QString(clientPath + "/" + oldCurDb).ascii());
}

void Database::decryptDb()
{
    QFile origDb(clientPath + "/" + curDb);

    curDb.truncate(curDb.length() - 4);

    QFile newDb(clientPath + "/" + curDb + ".db");

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
    
    while(!origDb.atEnd())
        newDb.putch(origDb.getch());
    
    newDb.close();
    origDb.close();

    remove(clientPath + "/" + curDb + ".db_");
    remove(clientPath + "/" + curDb + ".dbx");

    curDb.append(".db");
}

bool Database::isEncrypted() const
{
    if(curDb.right(4) == ".db_")
        return true;
    else
        return false;
}

void Database::importDb(QString file) const
{
    QRegExp regexp("[\\/\\\\]([^\\/\\\\]*)(\\.dbx?)$");
    regexp.search(file);

    QString ext = regexp.cap(2);

    QString newFile = clientPath + "/" + regexp.cap(1);

    QFile origDb(file);

    QFile newDb(newFile + ext);

    int i = 0;
    while(newDb.exists())
        newDb.setName(newFile + QString::number(i++) + ext);

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
    
    while(!origDb.atEnd())
        newDb.putch(origDb.getch());
    
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

bool Database::curClientExists() const
{
    QFile file(clientPath + "/" + curDb);
    if(file.exists())
        return true;
    else
        return false;
}

QStringList Database::getClientList() const
{
    QStringList list;
    
#ifdef Q_WS_WIN
        QSqlDatabase *connectionTemp = QSqlDatabase::addDatabase("QSQLITEX", "temp");
#else
        QSqlDatabase *connectionTemp = QSqlDatabase::addDatabase("QSQLITE", "temp");
#endif 
    
    QDir dir(clientPath);
    QStringList::Iterator it;
    QStringList files = dir.entryList("*.db", QDir::Files);
    it = files.begin();
    while(it != files.end())
    {
        connectionTemp->setDatabaseName(clientPath + "/" + *it);
        if(connectionTemp->open())
        {
            QSqlQuery query("SELECT name FROM main", connectionTemp);
            if(query.next())
                list << (query.value(0).toString() + "    (" + *it + ")");
            connectionTemp->close();
        }
        ++it;
    }

    QFile encDb;
    QDataStream encStream;

    files = dir.entryList("*.dbx", QDir::Files);
    it = files.begin();
    while(it != files.end())
    {
        encDb.setName(clientPath + "/" + *it);
        encDb.open( IO_ReadOnly );
        encStream.setDevice(&encDb);

        char *string;
        encStream >> string;
        QString qstring(string);

        if(qstring.startsWith("enc:"))
            list << (qstring.mid(4) + "    (" + *it + ")");

        delete string;
        encStream.unsetDevice();
        encDb.close();
        ++it;
    }
    
    return list;
}

void Database::exportCSV(QString table, QString file) const
{
    if(file.right(4) != ".csv")
        file.append(".csv");

    QFile destFile(file);
    
    if(destFile.open(IO_WriteOnly))
    {
        QTextStream destStream(&destFile);
        
        QSqlQuery query("SELECT * FROM " + table);
        
        if(table == "accounts")
        {
            destStream << "Account ID,Account Description\n";
        
            while(query.next())
                destStream << query.value(1).toString() << "," << query.value(2).toString() << "\n";
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
        
            QPushButton okButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/okButton.png")),
                                 "OK", &okCancelFrame);
            connect(&okButton, SIGNAL(clicked()), &dialog, SLOT(accept()));
            QPushButton cancelButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/cancelButton.png")),
                                     "Cancel", &okCancelFrame);
            connect(&cancelButton, SIGNAL(clicked()), &dialog, SLOT(reject()));
            
            okCancelLayout.addStretch();
            okCancelLayout.addWidget(&okButton);
            okCancelLayout.addWidget(&cancelButton);
        
            vBoxLayout.addWidget(&label);
            vBoxLayout.addWidget(&dataTable);
            vBoxLayout.addWidget(&buttonGroup);
            vBoxLayout.addSpacing(10);
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

                        emit accountsChanged();
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

    if(table == "journalTmp")
        emit journalTmpChanged();
    
    origFile.close();
}

QStringList Database::getPeriodBegin() const
{
    QStringList range;
    QString yearEnd;
    
    QDate date = QDate::currentDate();
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
        
        range << ("Period " + periodString.setNum(period++) + " (" + monthString + "/01/" + curYear + ")");
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
        
        range << ("Period " + periodString.setNum(period++) + " (" + monthString + "/01/" + nextYear + ")");
    }
    
    return range;
}

QStringList Database::getPeriodEnd() const
{
    QStringList periodEnd;
    QStringList periodBegin = getPeriodBegin();
    
    QDate date = QDate::currentDate();
    
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

QPtrList<EditorElement> Database::getElements(editorCategories category)
{
    QPtrList<EditorElement> tmpList;

    if(!connection->open())
    {
        connection->lastError().showMessage();
        return tmpList;
    }
    
    QString table;
    switch(category)
    {
        case balanceAssets:
            table = "balanceAssets";
            break;
        case balanceLiabilities:
            table = "balanceLiabilities";
            break;
        case balanceEquities:
            table = "balanceEquities";
            break;
        case incomeSvcIncome:
            table = "incomeSvcIncome";
            break;
        case incomeSvcExpenses:
            table = "incomeSvcExpenses";
            break;
        case incomeRetailIncome:
            table = "incomeRetailIncome";
            break;
        case incomeRetailCOS:
            table = "incomeRetailCOS";
            break;
        case incomeRetailExpenses:
            table = "incomeRetailExpenses";
            break;
    }

    QSqlQuery query("SELECT * FROM " + table + " ORDER BY key", connection);
    
    while(query.next())
    {
        EditorElement *element = new EditorElement(
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

void Database::createElement(editorCategories category, QString type,
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
        case balanceAssets:
            table = "balanceAssets";
            break;
        case balanceLiabilities:
            table = "balanceLiabilities";
            break;
        case balanceEquities:
            table = "balanceEquities";
            break;
        case incomeSvcIncome:
            table = "incomeSvcIncome";
            break;
        case incomeSvcExpenses:
            table = "incomeSvcExpenses";
            break;
        case incomeRetailIncome:
            table = "incomeRetailIncome";
            break;
        case incomeRetailCOS:
            table = "incomeRetailCOS";
            break;
        case incomeRetailExpenses:
            table = "incomeRetailExpenses";
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

void Database::removeElement(editorCategories category, QString key)
{
    if(!connection->open())
    {
        connection->lastError().showMessage();
        return;
    }
    
    QString table;
    switch(category)
    {
        case balanceAssets:
            table = "balanceAssets";
            break;
        case balanceLiabilities:
            table = "balanceLiabilities";
            break;
        case balanceEquities:
            table = "balanceEquities";
            break;
        case incomeSvcIncome:
            table = "incomeSvcIncome";
            break;
        case incomeSvcExpenses:
            table = "incomeSvcExpenses";
            break;
        case incomeRetailIncome:
            table = "incomeRetailIncome";
            break;
        case incomeRetailCOS:
            table = "incomeRetailCOS";
            break;
        case incomeRetailExpenses:
            table = "incomeRetailExpenses";
            break;
    }

    QSqlQuery query("DELETE FROM " + table + " WHERE key = " + key, connection);
}

void Database::moveElement(editorCategories category, QString key, bool up)
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
        case balanceAssets:
            table = "balanceAssets";
            break;
        case balanceLiabilities:
            table = "balanceLiabilities";
            break;
        case balanceEquities:
            table = "balanceEquities";
            break;
        case incomeSvcIncome:
            table = "incomeSvcIncome";
            break;
        case incomeSvcExpenses:
            table = "incomeSvcExpenses";
            break;
        case incomeRetailIncome:
            table = "incomeRetailIncome";
            break;
        case incomeRetailCOS:
            table = "incomeRetailCOS";
            break;
        case incomeRetailExpenses:
            table = "incomeRetailExpenses";
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
    QPtrList<QStringList> accounts;
    
    bool allAccounts = false;
    if(accountBegin == "" && accountEnd == "")
        allAccounts = true;
    
    QSqlQuery query(connection);
    if(allAccounts)
        query.prepare("SELECT * FROM accounts ORDER BY id");
    else
        query.prepare("SELECT * FROM accounts WHERE id >= " + accountBegin
                      + " AND id <= " + accountEnd + " ORDER BY id");
    query.exec();

    while(query.next())
    {
        QStringList* accountsList = new QStringList;
        *accountsList << query.value(1).toString() << query.value(2).toString();
        accounts.append(accountsList);
    }
    
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
    
    report->header += "<table width=\"100%\"><tr>"
            "<td width=\"19%\"><b>Account ID<br>Account Description</b></td>"
            "<td width=\"10%\"><b>Date</b></td>"
            "<td width=\"10%\"><b>Reference</b></td>"
            "<td width=\"31%\"><b>Description</b></td>"
            "<td align=\"right\" width=\"10%\"><b>Debit Amt</b></td>"
            "<td align=\"right\" width=\"10%\"><b>Credit Amt</b></td>"
            "<td align=\"right\" width=\"10%\"><b>Balance</b></td></tr></table>";

    report->data = "<table width=\"100%\">";
    
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
    
    QDate curDate = QDate::currentDate();
    
    for(QPtrList<QStringList>::Iterator it = accounts.begin(); it != accounts.end(); ++it)
    {
        query.prepare("SELECT date, reference, desc, debit, credit FROM journal WHERE account = " + (**it)[1]);
        query.exec();
        
        report->data += "<tr><td width=\"19%\">" + (**it)[0] + "</td><td width=\"10%\">"
                + periodBeginDate.toString("MM/dd/yy") +
                "</td><td width=\"10%\"> </td><td width=\"31%\">Beginning Balance</td><td width=\"10%\"> </td>"
                "<td width=\"10%\"> </td><td align=\"right\" width=\"10%\">(BALANCE)</td></tr>";
        
        bool accountDescDisplayed = false;
        
        report->csv += (**it)[0] + "," + (**it)[1] + "," + periodBeginDate.toString("MM/dd/yy") +
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
                        report->data += "<tr><td width=\"19%\">" + (**it)[1] + "</td>";
                        accountDescDisplayed = true;
                    }
                    else
                        report->data += "<tr><td width=\"19%\"> </td>";
                    
                    report->data += "<td width=\"10%\">" + query.value(0).toString() + "</td><td width=\"10%\">"
                            + query.value(1).toString() + "</td><td width=\"31%\">"
                            + query.value(2).toString() + "</td><td align=\"right\" width=\"10%\">"
                            + addCommas(query.value(3).toString()) + "</td><td align=\"right\" width=\"10%\">"
                            + addCommas(query.value(4).toString()) + "</td><td width=\"10%\"> </td></tr>\n";
                    
                    report->csv += (**it)[0] + "," + (**it)[1] + ","
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
                    beginningBalance = addCurrency(beginningBalance, negCurrency(query.value(4).toString()));
                }
            }
            
            if(inDateRange)
            {   
                report->data += "<tr><td width=\"19%\"> </td><td width=\"10%\"> </td><td width=\"10%\"> </td>"
                        "<td width=\"31%\">Current Period Change</td><td align=\"right\" width=\"10%\">"
                        + addCommas(debitBalance) + "</td><td align=\"right\" width=\"10%\">"
                        + addCommas(creditBalance) + "</td><td align=\"right\" width=\"10%\">"
                        + addCommas(periodChangeBalance) + "</td></tr>\n";
                
                report->csv += ",,,Current Period Change," + debitBalance + "," + creditBalance
                        + "," + periodChangeBalance + "\n";
            }
        }
        else
        {
            report->data += "<tr><td width=\"19%\">" + (**it)[1] + "</td></tr>\n";
            accountDescDisplayed = true;
        }
        
        if(!accountDescDisplayed)
            report->data += "<tr><td width=\"19%\">" + (**it)[1] + "</td></tr>";
        
        if(beginningBalance == "0.00" && debitBalance == "0.00" && creditBalance == "0.00")
            endingBalance = "";
        else
            endingBalance = addCurrency(beginningBalance, periodChangeBalance);
        
        if(beginningBalance == "0.00")
            beginningBalance = "";
        
        report->data = (report->data).replace("(BALANCE)", addCommas(beginningBalance));
        report->csv = (report->csv).replace("(BALANCE)", beginningBalance);
        
        report->data += "<tr><td width=\"19%\"> </td><td width=\"10%\"><b>"
                + periodEndDate.toString("MM/dd/yy") + "</b></td>"
                "<td width=\"10%\"> </td><td width=\"31%\"><b>Ending Balance</b></td>"
                "<td width=\"10%\"> </td><td width=\"10%\"> </td><td align=\"right\" width=\"10%\"><b>"
                + addCommas(endingBalance) + "</b></td></tr><tr><td> </td></tr><tr><td> </td></tr>\n";
        
        report->csv += "," + periodEndDate.toString("MM/dd/yy") + ",,Ending Balance,,,"
                + endingBalance + "\n";

        delete *it;
        
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
    QPtrList<QStringList> accounts;
    
    bool allAccounts = false;
    if(accountBegin == "" && accountEnd == "")
        allAccounts = true;
   
    QSqlQuery query(connection);
    if(allAccounts)
        query.prepare("SELECT * FROM accounts ORDER BY id");
    else
        query.prepare("SELECT * FROM accounts WHERE id >= " + accountBegin
                      + " AND id <= " + accountEnd + " ORDER BY id");
    query.exec();

    while(query.next())
    {
        QStringList* accountsList = new QStringList;
        *accountsList << query.value(1).toString() << query.value(2).toString();
        accounts.append(accountsList);
    }
    
    QDate curDate = QDate::currentDate();
    
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
    
    report->header += "<table width=\"100%\"><tr><td width=\"19%\"><b>Account ID</b></td>"
            "<td width=\"31%\"><b>Account Description</b></td>"
            "<td align=\"right\" width=\"10%\"><b>Debit Amt</b></td>"
            "<td align=\"right\" width=\"10%\"><b>Credit Amt</b></td><td width=\"30%\"> </td></tr></table>";

    report->data = "<table width=\"100%\">";
    
    QString debit = "0.00";
    QString credit = "0.00";
    QString totalDebit = "0.00";
    QString totalCredit = "0.00";
    
    QString entryYear;
    QString entryMonth;
    QString entryDay;
    QDate entryDate;
    
    for(QPtrList<QStringList>::Iterator it = accounts.begin(); it != accounts.end(); ++it)
    {
        query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + (**it)[0]);
        query.exec();
        
        report->data += "<tr><td width=\"19%\">" + (**it)[0] + "</td><td width=\"31%\">" + (**it)[1]
                + "</td><td align=\"right\" width=\"10%\">";
        report->csv += (**it)[0] + "," + (**it)[1] + ",";
        
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
                report->data += addCommas(debit);
            report->data += "</td><td align=\"right\" width=\"10%\">";
            if(credit != "0.00")
                report->data += addCommas(credit);
            report->data += "</td><td width=\"30%\"> </td></tr>\n";
            
            report->csv += debit + "," + credit + "\n";
        }
        else
        {
            report->data += "</td><td width=\"10%\"> </td><td width=\"30%\"> </td></tr>\n";
            report->csv += ",\n";
        }

        delete *it;
    }
    
    report->data += "<tr><td> </td><td> </td><td> </td><td> </td>"
            "</tr><tr><td width=\"19%\"> </td><td width=\"31%\"><b>Total:</b></td>"
            "<td align=\"right\" width=\"10%\"><b>" + addCommas(totalDebit)
            + "</b></td><td align=\"right\" width=\"10%\"><b>" + addCommas(totalCredit) + "</b></td>"
            "<td width=\"30%\"> </td></tr>\n</table>";

    report->csv += ",,,,,Total:," + totalDebit + "," + totalCredit + ",\n";
    
    
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
    
    QDate curDate = QDate::currentDate();
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
    report->header += "<center><h3>" + periodDate.toString("MMM d, yyyy") + "</h3>\n";

    report->header += "<table width=\"60%\"><tr>"
            "<td width=\"70%\"><b>Account Description</b></td>"
            "<td align=\"right\" width=\"30%\"><b>Balance</b></td></tr></table></center>";

    report->data = "<center><table width=\"60%\">";

    QStringList assetsList;
    QStringList liabilitiesList;
    QStringList equitiesList;

    QSqlQuery query("SELECT * FROM balanceAssets", connection);
    while(query.next())
    {
        assetsList << (query.value(0).toString() + "\n" + query.value(1).toString()
                + "\n" + query.value(2).toString() + "\n" + query.value(3).toString()
                + "\n" + query.value(4).toString());
    }
    assetsList.sort();

    query.prepare("SELECT * FROM balanceLiabilities");
    query.exec();
    while(query.next())
    {
        liabilitiesList << (query.value(0).toString() + "\n" + query.value(1).toString()
                + "\n" + query.value(2).toString() + "\n" + query.value(3).toString()
                + "\n" + query.value(4).toString());
    }
    liabilitiesList.sort();

    query.prepare("SELECT * FROM balanceEquities");
    query.exec();
    while(query.next())
    {
        equitiesList << (query.value(0).toString() + "\n" + query.value(1).toString()
                + "\n" + query.value(2).toString() + "\n" + query.value(3).toString()
                + "\n" + query.value(4).toString());
    }
    equitiesList.sort();


    QString sectionBalance;
    QString accountBalance;

    QString type;
    QString desc;
    QString accountBegin;
    QString accountEnd;
    
    QRegExp regexp(".*\n(.*)\n(.*)\n(.*)\n(.*)");

    sectionBalance = "0.00";
    for(QStringList::Iterator listIt = assetsList.begin(); listIt != assetsList.end(); ++listIt)
    {
        regexp.search(*listIt);
        type = regexp.cap(1);
        desc = regexp.cap(2);
        accountBegin = regexp.cap(3);
        accountEnd = regexp.cap(4);

        accountBalance = "0.00";
        if(type == "0")  // one account
        {
            query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + accountBegin);
            query.exec();
            while(query.next())
            {
                entryYear = query.value(0).toString().right(2);
                
                if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                    entryYear.insert(0, "20");
                else
                    entryYear.insert(0, "19");
                
                entryMonth = query.value(0).toString().left(2);
                entryDay = query.value(0).toString().mid(3,2);
                
                entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                if(entryDate > periodDate)
                    continue;

                if(query.value(1).toString() != "")
                    accountBalance = addCurrency(accountBalance, query.value(1).toString());
                if(query.value(2).toString() != "")
                    accountBalance = addCurrency(accountBalance, negCurrency(query.value(2).toString()));
            }
        }
        else  // begin and end accounts
        {
            QStringList accounts;

            query.prepare("SELECT id FROM accounts");
            query.exec();
            while(query.next())
            {
                if(query.value(0).toInt() >= accountBegin.toInt()
                   && query.value(0).toInt() <= accountEnd.toInt())
                    accounts << query.value(0).toString();
            }

            for(QStringList::Iterator it = accounts.begin(); it != accounts.end(); ++it)
            {
                query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + *it);
                query.exec();
                while(query.next())
                {
                    entryYear = query.value(0).toString().right(2);
                
                    if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                        entryYear.insert(0, "20");
                    else
                        entryYear.insert(0, "19");
                
                    entryMonth = query.value(0).toString().left(2);
                    entryDay = query.value(0).toString().mid(3,2);
                
                    entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                    if(entryDate > periodDate)
                        continue;

                    if(query.value(1).toString() != "")
                        accountBalance = addCurrency(accountBalance, query.value(1).toString());
                    if(query.value(2).toString() != "")
                        accountBalance = addCurrency(accountBalance, negCurrency(query.value(2).toString()));
                }
            }
        }

        sectionBalance = addCurrency(sectionBalance, accountBalance);

        report->data += "<tr><td width=\"70%\">" + desc + "</td>"
                "<td align=\"right\" width=\"30%\">" + formatCurrency(accountBalance) + "</td></tr>\n";
        report->csv += desc + "," + accountBalance + "\n";
    }

    report->data += "<tr><td> </td></tr>\n";
    report->data += "<tr><td width=\"70%\"><b>Total Assets</b></td>"
            "<td align=\"right\" width=\"30%\"><b>" + formatCurrency(sectionBalance) + "</b></td></tr>\n";
    report->data += "<tr><td> </td></tr><tr><td> </td></tr>\n";
    report->csv += "Total Assets," + sectionBalance + "\n";


    sectionBalance = "0.00";
    for(QStringList::Iterator listIt = liabilitiesList.begin(); listIt != liabilitiesList.end(); ++listIt)
    {
        regexp.search(*listIt);
        type = regexp.cap(1);
        desc = regexp.cap(2);
        accountBegin = regexp.cap(3);
        accountEnd = regexp.cap(4);

        accountBalance = "0.00";
        if(type == "0")  // one account
        {
            query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + accountBegin);
            query.exec();
            while(query.next())
            {
                entryYear = query.value(0).toString().right(2);
                
                if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                    entryYear.insert(0, "20");
                else
                    entryYear.insert(0, "19");
                
                entryMonth = query.value(0).toString().left(2);
                entryDay = query.value(0).toString().mid(3,2);

                entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                if(entryDate > periodDate)
                    continue;

                if(query.value(1).toString() != "")
                {
                    accountBalance = addCurrency(accountBalance, negCurrency(query.value(1).toString()));
                }
                if(query.value(2).toString() != "")
                {
                    accountBalance = addCurrency(accountBalance, query.value(2).toString());
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
                if(query.value(0).toInt() >= accountBegin.toInt()
                   && query.value(0).toInt() <= accountEnd.toInt())
                    accounts << query.value(0).toString();
            }

            for(QStringList::Iterator it = accounts.begin(); it != accounts.end(); ++it)
            {
                query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + *it);
                query.exec();
                while(query.next())
                {
                    entryYear = query.value(0).toString().right(2);
                
                    if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                        entryYear.insert(0, "20");
                    else
                        entryYear.insert(0, "19");
                
                    entryMonth = query.value(0).toString().left(2);
                    entryDay = query.value(0).toString().mid(3,2);

                    entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                    if(entryDate > periodDate)
                        continue;

                    if(query.value(1).toString() != "")
                        accountBalance = addCurrency(accountBalance, negCurrency(query.value(1).toString()));
                    if(query.value(2).toString() != "")
                        accountBalance = addCurrency(accountBalance, query.value(2).toString());
                }
            }
        }

        sectionBalance = addCurrency(sectionBalance, accountBalance);

        report->data += "<tr><td width=\"70%\">" + desc + "</td>"
                "<td align=\"right\" width=\"30%\">" + formatCurrency(accountBalance) + "</td></tr>\n";
        report->csv += desc + "," + accountBalance + "\n";
    }

    report->data += "<tr><td> </td></tr>\n";
    report->data += "<tr><td width=\"70%\"><b>Total Liabilities</b></td>"
            "<td align=\"right\" width=\"30%\"><b>" + formatCurrency(sectionBalance) + "</b></td></tr>\n";
    report->data += "<tr><td> </td></tr><tr><td> </td></tr>\n";
    report->csv += "Total Liabilities," + sectionBalance + "\n";


    sectionBalance = "0.00";
    for(QStringList::Iterator listIt = equitiesList.begin(); listIt != equitiesList.end(); ++listIt)
    {
        regexp.search(*listIt);
        type = regexp.cap(1);
        desc = regexp.cap(2);
        accountBegin = regexp.cap(3);
        accountEnd = regexp.cap(4);

        accountBalance = "0.00";
        if(type == "0")  // one account
        {
            query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + accountBegin);
            query.exec();
            while(query.next())
            {
                entryYear = query.value(0).toString().right(2);
                
                if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                    entryYear.insert(0, "20");
                else
                    entryYear.insert(0, "19");
                
                entryMonth = query.value(0).toString().left(2);
                entryDay = query.value(0).toString().mid(3,2);
                
                entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                if(entryDate > periodDate)
                    continue;

                if(query.value(1).toString() != "")
                {
                    accountBalance = addCurrency(accountBalance, negCurrency(query.value(1).toString()));
                }
                if(query.value(2).toString() != "")
                {
                    accountBalance = addCurrency(accountBalance, query.value(2).toString());
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
                if(query.value(0).toInt() >= accountBegin.toInt()
                   && query.value(0).toInt() <= accountEnd.toInt())
                    accounts << query.value(0).toString();
            }

            for(QStringList::Iterator it = accounts.begin(); it != accounts.end(); ++it)
            {
                query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + *it);
                query.exec();
                while(query.next())
                {
                    entryYear = query.value(0).toString().right(2);
                
                    if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                        entryYear.insert(0, "20");
                    else
                        entryYear.insert(0, "19");
                
                    entryMonth = query.value(0).toString().left(2);
                    entryDay = query.value(0).toString().mid(3,2);
                
                    entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                    if(entryDate > periodDate)
                        continue;

                    if(query.value(1).toString() != "")
                        accountBalance = addCurrency(accountBalance, negCurrency(query.value(1).toString()));
                    if(query.value(2).toString() != "")
                        accountBalance = addCurrency(accountBalance, query.value(2).toString());
                }
            }
        }

        sectionBalance = addCurrency(sectionBalance, accountBalance);

        report->data += "<tr><td width=\"70%\">" + desc + "</td>"
                "<td align=\"right\" width=\"30%\">" + formatCurrency(accountBalance) + "</td></tr>\n";
        report->csv += desc + "," + accountBalance + "\n";
    }
    
    report->data += "<tr><td> </td></tr>\n";
    report->data += "<tr><td width=\"70%\"><b>Total Equities</b></td>"
            "<td align=\"right\" width=\"30%\"><b>" + formatCurrency(sectionBalance) + "</b></td></tr>\n";
    report->data += "<tr><td> </td></tr><tr><td> </td></tr>\n";
    report->csv += "Total Equities," + sectionBalance + "\n";

    report->data += "</table></center>";

    report->string = "<qt>" + report->header + report->data + "</qt>";
    
    report->html = report->string.replace(
            "<qt>", "<html><head><title>" + curClient +
            " - Balance Sheet</title></head><body>\n").replace(
                    "</qt>", "</body></html>");

    return this;
}

Database* Database::getChartAccounts()
{
    QSqlQuery query("SELECT * FROM accounts ORDER BY id", connection);
    
    QDate date = QDate::currentDate();
    
    report->csv = "Account ID,Account Description\n";
    report->header = "<center><h1>";
    report->header += curClient;
    report->header += "</h1></center>\n";
    report->header += "<center><h2>Chart of Accounts</h2></center>\n";
    report->header += "<center><h3>As of " + date.toString("MMM d, yyyy") + "</h3></center>\n";

    report->header += "<table width=\"100%\"><tr><td width=\"15%\"><b>Account ID</b></td>"
            "<td width=\"40%\"><b>Account Description</b></td><td width=\"45%\"> </td></tr>\n";

    report->data = "<table width=\"100%\">";
    
    QStringList reportString;
    QStringList reportCSV;
    
    while(query.next())
    {
        reportCSV << (query.value(1).toString() + "," + query.value(2).toString() + "\n");
        reportString << ("<tr><td width=\"15%\">" + query.value(1).toString() + "</td><td width=\"40%\">" +
                query.value(2).toString() + "</td><td width=\"45%\"> </td></tr>\n");
    }
    
    
    report->data += reportString.join("") + "</table>";
    report->csv += reportCSV.join("");
    
    report->string = "<qt>" + report->header + report->data + "</qt>";
    
    report->html = report->string.replace(
            "<qt>", "<html><head><title>" + curClient + " - Chart of Accounts</title></head><body>\n").replace(
            "</qt>", "</body></html>");
    
    return this;
}

Database* Database::getJournalTmpReport()
{
    QSqlQuery query("SELECT * FROM journalTmp ORDER BY key", connection);
    
    QDate date = QDate::currentDate();

    journalTmpReport->header = "<center><h1>";
    journalTmpReport->header += curClient;
    journalTmpReport->header += "</h1></center>\n";
    journalTmpReport->header += "<center><h2>Journal Entries</h2></center>\n";
    journalTmpReport->header += "<center><h3>Entered " + date.toString("MMM d, yyyy") + "</h3></center>\n";

    journalTmpReport->header += "<table width=\"100%\"><tr><td width=\"10%\"><b>Date</b></td>"
            "<td width=\"10%\"><b>Account</b></td><td width=\"10%\"><b>Reference</b></td>"
            "<td width=\"50%\"><b>Description</b></td><td width\"10%\" align=\"right\"><b>Debit Amt</b></td>"
            "<td width=\"10%\" align=\"right\"><b>Credit Amt</b></td></tr>\n";

    journalTmpReport->data = "<table width=\"100%\">";
    
    QStringList reportString;
    
    while(query.next())
    {
        journalTmpReport->data += "<tr><td width=\"10%\">" + query.value(1).toString() + "</td><td width=\"10%\">" +
                query.value(2).toString() + "</td><td width=\"10%\">" + query.value(3).toString() +
                "</td><td width=\"50%\">" + query.value(4).toString() + "</td><td width=\"10%\" align=\"right\">" +
                addCommas(query.value(5).toString()) + "</td><td width=\"10%\" align=\"right\">"
                + addCommas(query.value(6).toString()) + "</td></tr>\n";
    }

    journalTmpReport->data += "</table>";
    
    return this;
}

Database* Database::getIncomeSvcReport(QString periodEnd)
{
    QRegExp dateRegexp("Period \\d+ \\((\\d\\d)/(\\d\\d)/(\\d\\d)\\)");

    dateRegexp.search(getPeriodBegin()[0]);
    int periodBeginMonth = dateRegexp.cap(1).toInt();
    QDate periodBeginDate(dateRegexp.cap(3).insert(0, "20").toInt(), dateRegexp.cap(1).toInt(), dateRegexp.cap(2).toInt());

    dateRegexp.search(periodEnd);
    QDate periodBeginSingleDate(dateRegexp.cap(3).insert(0, "20").toInt(), dateRegexp.cap(1).toInt(), 1);

    dateRegexp.search(periodEnd);
    int periodEndMonth = dateRegexp.cap(1).toInt();
    QDate periodEndDate(dateRegexp.cap(3).insert(0, "20").toInt(), dateRegexp.cap(1).toInt(), dateRegexp.cap(2).toInt());

    QDate curDate = QDate::currentDate();

    QString monthRange;
    int monthRangeInt = periodEndMonth - periodBeginMonth;
    if(monthRangeInt < 0)
        monthRangeInt += 12;
    monthRangeInt++;

    if(monthRangeInt == 1)
        monthRange = "One";
    if(monthRangeInt == 2)
        monthRange = "Two";
    if(monthRangeInt == 3)
        monthRange = "Three";
    if(monthRangeInt == 4)
        monthRange = "Four";
    if(monthRangeInt == 5)
        monthRange = "Five";
    if(monthRangeInt == 6)
        monthRange = "Six";
    if(monthRangeInt == 7)
        monthRange = "Seven";
    if(monthRangeInt == 8)
        monthRange = "Eight";
    if(monthRangeInt == 9)
        monthRange = "Nine";
    if(monthRangeInt == 10)
        monthRange = "Ten";
    if(monthRangeInt == 11)
        monthRange = "Eleven";
    if(monthRangeInt == 12)
        monthRange = "Twelve";

    QString entryYear;
    QString entryMonth;
    QString entryDay;
    QDate entryDate;

    report->csv = "Account Description,One Month Balance,";
    if(monthRange == "One")
        report->csv += "One Month Balance\n";
    else
        report->csv += monthRange + " Months Balance\n";

    report->header = "<center><h1>";
    report->header += curClient;
    report->header += "</h1></center>\n";
    report->header += "<center><h2>Income Statement Service Company</h2></center>\n";
    report->header += "<center><h3>For One Month and ";
    if(monthRange == "One")
        report->header += "One Month";
    else
        report->header += monthRange + " Months";
    report->header += " Ended " + periodEndDate.toString("MMM d, yyyy") + "</h3>\n";

    report->header += "<table width=\"80%\"><tr>"
            "<td width=\"50%\"><b>Account Description</b></td>"
            "<td align=\"right\" width=\"25%\"><b>One Month</b></td>"
            "<td align=\"right\" width=\"25%\"><b>";
    if(monthRange == "One")
        report->header += "One Month";
    else
        report->header += monthRange + " Months";
    report->header += "</b></td></tr></table></center>";

    report->data = "<center><table width=\"80%\">";

    QStringList incomeList;
    QStringList expensesList;

    QSqlQuery query("SELECT * FROM incomeSvcIncome", connection);
    while(query.next())
    {
        incomeList << (query.value(0).toString() + "\n" + query.value(1).toString()
        + "\n" + query.value(2).toString() + "\n" + query.value(3).toString()
        + "\n" + query.value(4).toString());
    }
    incomeList.sort();

    query.prepare("SELECT * FROM incomeSvcExpenses");
    query.exec();
    while(query.next())
    {
        expensesList << (query.value(0).toString() + "\n" + query.value(1).toString()
        + "\n" + query.value(2).toString() + "\n" + query.value(3).toString()
        + "\n" + query.value(4).toString());
    }
    expensesList.sort();

    QString incomeBalance;
    QString incomeBalanceSingle;
    QString expensesBalance;
    QString expensesBalanceSingle;
    QString accountBalance;
    QString accountBalanceSingle;

    QString type;
    QString desc;
    QString accountBegin;
    QString accountEnd;

    QRegExp regexp(".*\n(.*)\n(.*)\n(.*)\n(.*)");

    incomeBalance = "0.00";
    incomeBalanceSingle = "0.00";
    for(QStringList::Iterator listIt = incomeList.begin(); listIt != incomeList.end(); ++listIt)
    {
        regexp.search(*listIt);
        type = regexp.cap(1);
        desc = regexp.cap(2);
        accountBegin = regexp.cap(3);
        accountEnd = regexp.cap(4);

        accountBalance = "0.00";
        accountBalanceSingle = "0.00";
        if(type == "0")
        {
            query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + accountBegin);
            query.exec();
            while(query.next())
            {
                entryYear = query.value(0).toString().right(2);

                if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                    entryYear.insert(0, "20");
                else
                    entryYear.insert(0, "19");

                entryMonth = query.value(0).toString().left(2);
                entryDay = query.value(0).toString().mid(3,2);

                entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                if(entryDate > periodEndDate || entryDate < periodBeginDate)
                    continue;

                if(entryDate >= periodBeginDate)
                {
                    if(query.value(1).toString() != "")
                        accountBalance = addCurrency(accountBalance, query.value(1).toString());
                    if(query.value(2).toString() != "")
                        accountBalance = addCurrency(accountBalance, negCurrency(query.value(2).toString()));
                }
                if(entryDate >= periodBeginSingleDate)
                {
                    if(query.value(1).toString() != "")
                        accountBalanceSingle = addCurrency(accountBalanceSingle, query.value(1).toString());
                    if(query.value(2).toString() != "")
                        accountBalanceSingle = addCurrency(accountBalanceSingle, negCurrency(query.value(2).toString()));
                }
            }
        }
        else
        {
            QStringList accounts;

            query.prepare("SELECT id FROM accounts");
            query.exec();
            while(query.next())
            {
                if(query.value(0).toInt() >= accountBegin.toInt()
                   && query.value(0).toInt() <= accountEnd.toInt())
                    accounts << query.value(0).toString();
            }

            for(QStringList::Iterator it = accounts.begin(); it != accounts.end(); ++it)
            {
                query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + *it);
                query.exec();
                while(query.next())
                {
                    entryYear = query.value(0).toString().right(2);

                    if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                        entryYear.insert(0, "20");
                    else
                        entryYear.insert(0, "19");

                    entryMonth = query.value(0).toString().left(2);
                    entryDay = query.value(0).toString().mid(3,2);

                    entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                    if(entryDate > periodEndDate || entryDate < periodBeginDate)
                        continue;

                    if(entryDate >= periodBeginDate)
                    {
                        if(query.value(1).toString() != "")
                            accountBalance = addCurrency(accountBalance, query.value(1).toString());
                        if(query.value(2).toString() != "")
                            accountBalance = addCurrency(accountBalance, negCurrency(query.value(2).toString()));
                    }
                    if(entryDate >= periodBeginSingleDate)
                    {
                        if(query.value(1).toString() != "")
                            accountBalanceSingle = addCurrency(accountBalanceSingle, query.value(1).toString());
                        if(query.value(2).toString() != "")
                            accountBalanceSingle = addCurrency(accountBalanceSingle, negCurrency(query.value(2).toString()));
                    }
                }
            }
        }

        incomeBalance = addCurrency(incomeBalance, negCurrency(accountBalance));
        incomeBalanceSingle = addCurrency(incomeBalanceSingle, negCurrency(accountBalanceSingle));

        report->data += "<tr><td width=\"50%\">" + desc + "</td>"
                "<td align=\"right\" width=\"25%\">" + formatCurrency(accountBalanceSingle) + "</td>"
                "<td align=\"right\" width=\"25%\">" + formatCurrency(accountBalance) + "</td></tr>\n";
        report->csv += desc + "," + accountBalanceSingle + "," + accountBalance + "\n";
    }

    report->data += "<tr><td> </td></tr>\n";
    report->data += "<tr><td align=\"right\" width=\"50%\"><b>Total Income</b></td>"
            "<td align=\"right\" width=\"25%\"><b>" + formatCurrency(incomeBalanceSingle) + "</b></td>"
            "<td align=\"right\" width=\"25%\"><b>" + formatCurrency(incomeBalance) + "</b></td></tr>\n";
    report->data += "<tr><td> </td></tr><tr><td> </td></tr>\n";
    report->csv += "Total Income," + incomeBalanceSingle + "," + incomeBalance + "\n\n";


    expensesBalance = "0.00";
    expensesBalanceSingle = "0.00";
    for(QStringList::Iterator listIt = expensesList.begin(); listIt != expensesList.end(); ++listIt)
    {
        regexp.search(*listIt);
        type = regexp.cap(1);
        desc = regexp.cap(2);
        accountBegin = regexp.cap(3);
        accountEnd = regexp.cap(4);

        accountBalance = "0.00";
        accountBalanceSingle = "0.00";
        if(type == "0")
        {
            query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + accountBegin);
            query.exec();
            while(query.next())
            {
                entryYear = query.value(0).toString().right(2);

                if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                    entryYear.insert(0, "20");
                else
                    entryYear.insert(0, "19");

                entryMonth = query.value(0).toString().left(2);
                entryDay = query.value(0).toString().mid(3,2);

                entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                if(entryDate > periodEndDate || entryDate < periodBeginDate)
                    continue;

                if(entryDate >= periodBeginDate)
                {
                    if(query.value(1).toString() != "")
                        accountBalance = addCurrency(accountBalance, query.value(1).toString());
                    if(query.value(2).toString() != "")
                        accountBalance = addCurrency(accountBalance, negCurrency(query.value(2).toString()));
                }
                if(entryDate >= periodBeginSingleDate)
                {
                    if(query.value(1).toString() != "")
                        accountBalanceSingle = addCurrency(accountBalanceSingle, query.value(1).toString());
                    if(query.value(2).toString() != "")
                        accountBalanceSingle = addCurrency(accountBalanceSingle, negCurrency(query.value(2).toString()));
                }
            }
        }
        else
        {
            QStringList accounts;

            query.prepare("SELECT id FROM accounts");
            query.exec();
            while(query.next())
            {
                if(query.value(0).toInt() >= accountBegin.toInt()
                   && query.value(0).toInt() <= accountEnd.toInt())
                    accounts << query.value(0).toString();
            }

            for(QStringList::Iterator it = accounts.begin(); it != accounts.end(); ++it)
            {
                query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + *it);
                query.exec();
                while(query.next())
                {
                    entryYear = query.value(0).toString().right(2);

                    if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                        entryYear.insert(0, "20");
                    else
                        entryYear.insert(0, "19");

                    entryMonth = query.value(0).toString().left(2);
                    entryDay = query.value(0).toString().mid(3,2);

                    entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                    if(entryDate > periodEndDate || entryDate < periodBeginDate)
                        continue;

                    if(entryDate >= periodBeginDate)
                    {
                        if(query.value(1).toString() != "")
                            accountBalance = addCurrency(accountBalance, query.value(1).toString());
                        if(query.value(2).toString() != "")
                            accountBalance = addCurrency(accountBalance, negCurrency(query.value(2).toString()));
                    }
                    if(entryDate >= periodBeginSingleDate)
                    {
                        if(query.value(1).toString() != "")
                            accountBalanceSingle = addCurrency(accountBalanceSingle, query.value(1).toString());
                        if(query.value(2).toString() != "")
                            accountBalanceSingle = addCurrency(accountBalanceSingle, negCurrency(query.value(2).toString()));
                    }
                }
            }
        }

        expensesBalance = addCurrency(expensesBalance, accountBalance);
        expensesBalanceSingle = addCurrency(expensesBalanceSingle, accountBalanceSingle);

        report->data += "<tr><td width=\"50%\">" + desc + "</td>"
                "<td align=\"right\" width=\"25%\">" + formatCurrency(accountBalanceSingle) + "</td>"
                "<td align=\"right\" width=\"25%\">" + formatCurrency(accountBalance) + "</td></tr>\n";
        report->csv += desc + "," + accountBalanceSingle + "," + accountBalance + "\n";
    }

    report->data += "<tr><td> </td></tr>\n";
    report->data += "<tr><td align=\"right\" width=\"50%\"><b>Total Expenses</b></td>"
            "<td align=\"right\" width=\"25%\"><b>" + formatCurrency(expensesBalanceSingle) + "</b></td>"
            "<td align=\"right\" width=\"25%\"><b>" + formatCurrency(expensesBalance) + "</b></td></tr>\n";
    report->data += "<tr><td> </td></tr><tr><td> </td></tr>\n";
    report->csv += "Total Expenses," + expensesBalanceSingle + "," + expensesBalance + "\n";

    report->data += "<tr><td align=\"right\" width=\"50%\"><b>Net Income (Loss)</b></td>"
            "<td align=\"right\" width=\"25%\"><b>" + formatCurrency(addCurrency(incomeBalanceSingle, expensesBalanceSingle))
            + "</b></td>"
            "<td align=\"right\" width=\"25%\"><b>" + formatCurrency(addCurrency(incomeBalance, expensesBalance))
            + "</b></td></tr>\n";
    report->csv += "Net Income (Loss)," + addCurrency(incomeBalanceSingle, expensesBalanceSingle)
            + "," + addCurrency(incomeBalance, expensesBalance) + "\n";

    report->data += "</table></center>";

    report->string = "<qt>" + report->header + report->data + "</qt>";

    report->html = report->string.replace(
            "<qt>", "<html><head><title>" + curClient +
            " - Income Statement Service Company</title></head><body>\n").replace(
                    "</qt>", "</body></html>");

    return this;
}

Database* Database::getIncomeRetailReport(QString periodEnd)
{
    QRegExp dateRegexp("Period \\d+ \\((\\d\\d)/(\\d\\d)/(\\d\\d)\\)");

    dateRegexp.search(getPeriodBegin()[0]);
    int periodBeginMonth = dateRegexp.cap(1).toInt();
    QDate periodBeginDate(dateRegexp.cap(3).insert(0, "20").toInt(), dateRegexp.cap(1).toInt(), dateRegexp.cap(2).toInt());

    dateRegexp.search(periodEnd);
    QDate periodBeginSingleDate(dateRegexp.cap(3).insert(0, "20").toInt(), dateRegexp.cap(1).toInt(), 1);

    dateRegexp.search(periodEnd);
    int periodEndMonth = dateRegexp.cap(1).toInt();
    QDate periodEndDate(dateRegexp.cap(3).insert(0, "20").toInt(), dateRegexp.cap(1).toInt(), dateRegexp.cap(2).toInt());

    QDate curDate = QDate::currentDate();

    QString monthRange;
    int monthRangeInt = periodEndMonth - periodBeginMonth;
    if(monthRangeInt < 0)
        monthRangeInt += 12;
    monthRangeInt++;

    if(monthRangeInt == 1)
        monthRange = "One";
    if(monthRangeInt == 2)
        monthRange = "Two";
    if(monthRangeInt == 3)
        monthRange = "Three";
    if(monthRangeInt == 4)
        monthRange = "Four";
    if(monthRangeInt == 5)
        monthRange = "Five";
    if(monthRangeInt == 6)
        monthRange = "Six";
    if(monthRangeInt == 7)
        monthRange = "Seven";
    if(monthRangeInt == 8)
        monthRange = "Eight";
    if(monthRangeInt == 9)
        monthRange = "Nine";
    if(monthRangeInt == 10)
        monthRange = "Ten";
    if(monthRangeInt == 11)
        monthRange = "Eleven";
    if(monthRangeInt == 12)
        monthRange = "Twelve";

    QString entryYear;
    QString entryMonth;
    QString entryDay;
    QDate entryDate;

    report->csv = "Account Description,One Month Balance,";
    if(monthRange == "One")
        report->csv += "One Month Balance\n";
    else
        report->csv += monthRange + " Months Balance\n";

    report->header = "<center><h1>";
    report->header += curClient;
    report->header += "</h1></center>\n";
    report->header += "<center><h2>Income Statement Retail</h2></center>\n";
    report->header += "<center><h3>For One Month and ";
    if(monthRange == "One")
        report->header += "One Month";
    else
        report->header += monthRange + " Months";
    report->header += " Ended " + periodEndDate.toString("MMM d, yyyy") + "</h3>\n";

    report->header += "<table width=\"80%\"><tr>"
            "<td width=\"50%\"><b>Account Description</b></td>"
            "<td align=\"right\" width=\"25%\"><b>One Month</b></td>"
            "<td align=\"right\" width=\"25%\"><b>";
    if(monthRange == "One")
        report->header += "One Month";
    else
        report->header += monthRange + " Months";
    report->header += "</b></td></tr></table></center>";

    report->data = "<center><table width=\"80%\">";

    QStringList incomeList;
    QStringList cosList;
    QStringList expensesList;

    QSqlQuery query("SELECT * FROM incomeRetailIncome", connection);
    while(query.next())
    {
        incomeList << (query.value(0).toString() + "\n" + query.value(1).toString()
        + "\n" + query.value(2).toString() + "\n" + query.value(3).toString()
        + "\n" + query.value(4).toString());
    }
    incomeList.sort();

    query.prepare("SELECT * FROM incomeRetailCOS");
    query.exec();
    while(query.next())
    {
        cosList << (query.value(0).toString() + "\n" + query.value(1).toString()
        + "\n" + query.value(2).toString() + "\n" + query.value(3).toString()
        + "\n" + query.value(4).toString());
    }
    cosList.sort();

    query.prepare("SELECT * FROM incomeRetailExpenses");
    query.exec();
    while(query.next())
    {
        expensesList << (query.value(0).toString() + "\n" + query.value(1).toString()
        + "\n" + query.value(2).toString() + "\n" + query.value(3).toString()
        + "\n" + query.value(4).toString());
    }
    expensesList.sort();

    QString incomeBalance;
    QString incomeBalanceSingle;
    QString cosBalance;
    QString cosBalanceSingle;
    QString expensesBalance;
    QString expensesBalanceSingle;
    QString accountBalance;
    QString accountBalanceSingle;

    QString type;
    QString desc;
    QString accountBegin;
    QString accountEnd;

    QRegExp regexp(".*\n(.*)\n(.*)\n(.*)\n(.*)");

    incomeBalance = "0.00";
    incomeBalanceSingle = "0.00";
    for(QStringList::Iterator listIt = incomeList.begin(); listIt != incomeList.end(); ++listIt)
    {
        regexp.search(*listIt);
        type = regexp.cap(1);
        desc = regexp.cap(2);
        accountBegin = regexp.cap(3);
        accountEnd = regexp.cap(4);

        accountBalance = "0.00";
        accountBalanceSingle = "0.00";
        if(type == "0")
        {
            query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + accountBegin);
            query.exec();
            while(query.next())
            {
                entryYear = query.value(0).toString().right(2);

                if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                    entryYear.insert(0, "20");
                else
                    entryYear.insert(0, "19");

                entryMonth = query.value(0).toString().left(2);
                entryDay = query.value(0).toString().mid(3,2);

                entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                if(entryDate > periodEndDate || entryDate < periodBeginDate)
                    continue;

                if(entryDate >= periodBeginDate)
                {
                    if(query.value(1).toString() != "")
                        accountBalance = addCurrency(accountBalance, query.value(1).toString());
                    if(query.value(2).toString() != "")
                        accountBalance = addCurrency(accountBalance, negCurrency(query.value(2).toString()));
                }
                if(entryDate >= periodBeginSingleDate)
                {
                    if(query.value(1).toString() != "")
                        accountBalanceSingle = addCurrency(accountBalanceSingle, query.value(1).toString());
                    if(query.value(2).toString() != "")
                        accountBalanceSingle = addCurrency(accountBalanceSingle, negCurrency(query.value(2).toString()));
                }
            }
        }
        else
        {
            QStringList accounts;

            query.prepare("SELECT id FROM accounts");
            query.exec();
            while(query.next())
            {
                if(query.value(0).toInt() >= accountBegin.toInt()
                   && query.value(0).toInt() <= accountEnd.toInt())
                    accounts << query.value(0).toString();
            }

            for(QStringList::Iterator it = accounts.begin(); it != accounts.end(); ++it)
            {
                query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + *it);
                query.exec();
                while(query.next())
                {
                    entryYear = query.value(0).toString().right(2);

                    if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                        entryYear.insert(0, "20");
                    else
                        entryYear.insert(0, "19");

                    entryMonth = query.value(0).toString().left(2);
                    entryDay = query.value(0).toString().mid(3,2);

                    entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                    if(entryDate > periodEndDate || entryDate < periodBeginDate)
                        continue;

                    if(entryDate >= periodBeginDate)
                    {
                        if(query.value(1).toString() != "")
                            accountBalance = addCurrency(accountBalance, query.value(1).toString());
                        if(query.value(2).toString() != "")
                            accountBalance = addCurrency(accountBalance, negCurrency(query.value(2).toString()));
                    }
                    if(entryDate >= periodBeginSingleDate)
                    {
                        if(query.value(1).toString() != "")
                            accountBalanceSingle = addCurrency(accountBalanceSingle, query.value(1).toString());
                        if(query.value(2).toString() != "")
                            accountBalanceSingle = addCurrency(accountBalanceSingle, negCurrency(query.value(2).toString()));
                    }
                }
            }
        }

        incomeBalance = addCurrency(incomeBalance, negCurrency(accountBalance));
        incomeBalanceSingle = addCurrency(incomeBalanceSingle, negCurrency(accountBalanceSingle));

        report->data += "<tr><td width=\"50%\">" + desc + "</td>"
                "<td align=\"right\" width=\"25%\">" + formatCurrency(accountBalanceSingle) + "</td>"
                "<td align=\"right\" width=\"25%\">" + formatCurrency(accountBalance) + "</td></tr>\n";
        report->csv += desc + "," + accountBalanceSingle + "," + accountBalance + "\n";
    }

    report->data += "<tr><td> </td></tr>\n";
    report->data += "<tr><td align=\"right\" width=\"50%\"><b>Total Income</b></td>"
            "<td align=\"right\" width=\"25%\"><b>" + formatCurrency(incomeBalanceSingle) + "</b>"
                "<br><img src=\"" + ICON_PATH + "/underline_single.gif\" width=\"80\" height=\"1\"></td>"
            "<td align=\"right\" width=\"25%\"><b>" + formatCurrency(incomeBalance) + "</b>"
                "<br><img src=\"" + ICON_PATH + "/underline_single.gif\" width=\"80\" height=\"1\"></td></tr>\n";
    report->data += "<tr><td> </td></tr><tr><td> </td></tr>\n";
    report->csv += "Total Income," + incomeBalanceSingle + "," + incomeBalance + "\n\n";


    cosBalance = "0.00";
    cosBalanceSingle = "0.00";
    for(QStringList::Iterator listIt = cosList.begin(); listIt != cosList.end(); ++listIt)
    {
        regexp.search(*listIt);
        type = regexp.cap(1);
        desc = regexp.cap(2);
        accountBegin = regexp.cap(3);
        accountEnd = regexp.cap(4);

        accountBalance = "0.00";
        accountBalanceSingle = "0.00";
        if(type == "0")
        {
            query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + accountBegin);
            query.exec();
            while(query.next())
            {
                entryYear = query.value(0).toString().right(2);

                if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                    entryYear.insert(0, "20");
                else
                    entryYear.insert(0, "19");

                entryMonth = query.value(0).toString().left(2);
                entryDay = query.value(0).toString().mid(3,2);

                entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                if(entryDate > periodEndDate || entryDate < periodBeginDate)
                    continue;

                if(entryDate >= periodBeginDate)
                {
                    if(query.value(1).toString() != "")
                        accountBalance = addCurrency(accountBalance, query.value(1).toString());
                    if(query.value(2).toString() != "")
                        accountBalance = addCurrency(accountBalance, negCurrency(query.value(2).toString()));
                }
                if(entryDate >= periodBeginSingleDate)
                {
                    if(query.value(1).toString() != "")
                        accountBalanceSingle = addCurrency(accountBalanceSingle, query.value(1).toString());
                    if(query.value(2).toString() != "")
                        accountBalanceSingle = addCurrency(accountBalanceSingle, negCurrency(query.value(2).toString()));
                }
            }
        }
        else
        {
            QStringList accounts;

            query.prepare("SELECT id FROM accounts");
            query.exec();
            while(query.next())
            {
                if(query.value(0).toInt() >= accountBegin.toInt()
                   && query.value(0).toInt() <= accountEnd.toInt())
                    accounts << query.value(0).toString();
            }

            for(QStringList::Iterator it = accounts.begin(); it != accounts.end(); ++it)
            {
                query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + *it);
                query.exec();
                while(query.next())
                {
                    entryYear = query.value(0).toString().right(2);

                    if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                        entryYear.insert(0, "20");
                    else
                        entryYear.insert(0, "19");

                    entryMonth = query.value(0).toString().left(2);
                    entryDay = query.value(0).toString().mid(3,2);

                    entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                    if(entryDate > periodEndDate || entryDate < periodBeginDate)
                        continue;

                    if(entryDate >= periodBeginDate)
                    {
                        if(query.value(1).toString() != "")
                            accountBalance = addCurrency(accountBalance, query.value(1).toString());
                        if(query.value(2).toString() != "")
                            accountBalance = addCurrency(accountBalance, negCurrency(query.value(2).toString()));
                    }
                    if(entryDate >= periodBeginSingleDate)
                    {
                        if(query.value(1).toString() != "")
                            accountBalanceSingle = addCurrency(accountBalanceSingle, query.value(1).toString());
                        if(query.value(2).toString() != "")
                            accountBalanceSingle = addCurrency(accountBalanceSingle, negCurrency(query.value(2).toString()));
                    }
                }
            }
        }

        cosBalance = addCurrency(cosBalance, accountBalance);
        cosBalanceSingle = addCurrency(cosBalanceSingle, accountBalanceSingle);

        report->data += "<tr><td width=\"50%\">" + desc + "</td>"
                "<td align=\"right\" width=\"25%\">" + formatCurrency(accountBalanceSingle) + "</td>"
                "<td align=\"right\" width=\"25%\">" + formatCurrency(accountBalance) + "</td></tr>\n";
        report->csv += desc + "," + accountBalanceSingle + "," + accountBalance + "\n";
    }

    report->data += "<tr><td> </td></tr>\n";
    report->data += "<tr><td align=\"right\" width=\"50%\"><b>Total Cost of Sales</b></td>"
            "<td align=\"right\" width=\"25%\"><b>" + formatCurrency(cosBalanceSingle) + "</b>"
                "<br><img src=\"" + ICON_PATH + "/underline_single.gif\" width=\"80\" height=\"1\"></td>"
            "<td align=\"right\" width=\"25%\"><b>" + formatCurrency(cosBalance) + "</b>"
                "<br><img src=\"" + ICON_PATH + "/underline_single.gif\" width=\"80\" height=\"1\"></td></tr>\n";
    report->data += "<tr><td align=\"right\" width=\"50%\"><b>Gross Profit</b></td>"
            "<td align=\"right\" width=\"25%\"><b>" + formatCurrency(
                                                      addCurrency(incomeBalanceSingle, negCurrency(cosBalanceSingle)))
                                                    + "</b>"
                                                        "<br><img src=\"" + ICON_PATH + 
                                                        "/underline_single.gif\" width=\"80\" height=\"1\"></td>"
            "<td align=\"right\" width=\"25%\"><b>" + formatCurrency(
                                                      addCurrency(incomeBalance, negCurrency(cosBalance)))
                                                    + "</b>"
                                                        "<br><img src=\"" + ICON_PATH +
                                                        "/underline_single.gif\" width=\"80\" height=\"1\"></td></tr>\n";
    report->data += "<tr><td> </td></tr><tr><td> </td></tr>\n";
    report->csv += "Total Cost of Sales," + cosBalanceSingle + "," + cosBalance + "\n";
    report->csv += "Gross Profit," + addCurrency(incomeBalanceSingle, negCurrency(cosBalanceSingle))
                + "," + addCurrency(incomeBalance, negCurrency(cosBalance)) + "\n\n";


    expensesBalance = "0.00";
    expensesBalanceSingle = "0.00";
    for(QStringList::Iterator listIt = expensesList.begin(); listIt != expensesList.end(); ++listIt)
    {
        regexp.search(*listIt);
        type = regexp.cap(1);
        desc = regexp.cap(2);
        accountBegin = regexp.cap(3);
        accountEnd = regexp.cap(4);

        accountBalance = "0.00";
        accountBalanceSingle = "0.00";
        if(type == "0")
        {
            query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + accountBegin);
            query.exec();
            while(query.next())
            {
                entryYear = query.value(0).toString().right(2);

                if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                    entryYear.insert(0, "20");
                else
                    entryYear.insert(0, "19");

                entryMonth = query.value(0).toString().left(2);
                entryDay = query.value(0).toString().mid(3,2);

                entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                if(entryDate > periodEndDate || entryDate < periodBeginDate)
                    continue;

                if(entryDate >= periodBeginDate)
                {
                    if(query.value(1).toString() != "")
                        accountBalance = addCurrency(accountBalance, query.value(1).toString());
                    if(query.value(2).toString() != "")
                        accountBalance = addCurrency(accountBalance, negCurrency(query.value(2).toString()));
                }
                if(entryDate >= periodBeginSingleDate)
                {
                    if(query.value(1).toString() != "")
                        accountBalanceSingle = addCurrency(accountBalanceSingle, query.value(1).toString());
                    if(query.value(2).toString() != "")
                        accountBalanceSingle = addCurrency(accountBalanceSingle, negCurrency(query.value(2).toString()));
                }
            }
        }
        else
        {
            QStringList accounts;

            query.prepare("SELECT id FROM accounts");
            query.exec();
            while(query.next())
            {
                if(query.value(0).toInt() >= accountBegin.toInt()
                   && query.value(0).toInt() <= accountEnd.toInt())
                    accounts << query.value(0).toString();
            }

            for(QStringList::Iterator it = accounts.begin(); it != accounts.end(); ++it)
            {
                query.prepare("SELECT date, debit, credit FROM journal WHERE account = " + *it);
                query.exec();
                while(query.next())
                {
                    entryYear = query.value(0).toString().right(2);

                    if(entryYear.toInt() < (curDate.toString("yy").toInt() + 2))
                        entryYear.insert(0, "20");
                    else
                        entryYear.insert(0, "19");

                    entryMonth = query.value(0).toString().left(2);
                    entryDay = query.value(0).toString().mid(3,2);

                    entryDate.setYMD(entryYear.toInt(), entryMonth.toInt(), entryDay.toInt());
                    if(entryDate > periodEndDate || entryDate < periodBeginDate)
                        continue;

                    if(entryDate >= periodBeginDate)
                    {
                        if(query.value(1).toString() != "")
                            accountBalance = addCurrency(accountBalance, query.value(1).toString());
                        if(query.value(2).toString() != "")
                            accountBalance = addCurrency(accountBalance, negCurrency(query.value(2).toString()));
                    }
                    if(entryDate >= periodBeginSingleDate)
                    {
                        if(query.value(1).toString() != "")
                            accountBalanceSingle = addCurrency(accountBalanceSingle, query.value(1).toString());
                        if(query.value(2).toString() != "")
                            accountBalanceSingle = addCurrency(accountBalanceSingle, negCurrency(query.value(2).toString()));
                    }
                }
            }
        }

        expensesBalance = addCurrency(expensesBalance, accountBalance);
        expensesBalanceSingle = addCurrency(expensesBalanceSingle, accountBalanceSingle);

        report->data += "<tr><td width=\"50%\">" + desc + "</td>"
                "<td align=\"right\" width=\"25%\">" + formatCurrency(accountBalanceSingle) + "</td>"
                "<td align=\"right\" width=\"25%\">" + formatCurrency(accountBalance) + "</td></tr>\n";
        report->csv += desc + "," + accountBalanceSingle + "," + accountBalance + "\n";
    }

    report->data += "<tr><td> </td></tr>\n";
    report->data += "<tr><td align=\"right\" width=\"50%\"><b>Total Expenses</b></td>"
            "<td align=\"right\" width=\"25%\"><b>" + formatCurrency(expensesBalanceSingle) + "</b>"
                "<br><img src=\"" + ICON_PATH + "/underline_single.gif\" width=\"80\" height=\"1\"></td>"
            "<td align=\"right\" width=\"25%\"><b>" + formatCurrency(expensesBalance) + "</b>"
                "<br><img src=\"" + ICON_PATH + "/underline_single.gif\" width=\"80\" height=\"1\"></td></tr>\n";
    report->data += "<tr><td align=\"right\" width=\"50%\"><b>Net Income (Loss)</b></td>"
            "<td align=\"right\" width=\"25%\"><b>" + formatCurrency(
                                                      addCurrency(
                                                          addCurrency(incomeBalanceSingle, negCurrency(cosBalanceSingle)),
                                                          negCurrency(expensesBalanceSingle)))
                                                    + "</b><br><img src=\""
                                                        + ICON_PATH +
                                                        "/underline_double.gif\" width=\"80\" height=\"3\">"
                                                    "</td>"
            "<td align=\"right\" width=\"25%\"><b>" + formatCurrency(
                                                      addCurrency(
                                                          addCurrency(incomeBalance, negCurrency(cosBalance)),
                                                          negCurrency(expensesBalance)))
                                                    + "</b><br><img src=\""
                                                        + ICON_PATH +
                                                        "/underline_double.gif\" width=\"80\" height=\"3\">"
                                                    "</td></tr>\n";
    report->data += "<tr><td> </td></tr><tr><td> </td></tr>\n";
    report->csv += "Total Expenses," + expensesBalanceSingle + "," + expensesBalance + "\n";
    report->csv += "Net Income (Loss)," + addCurrency(
                                              addCurrency(incomeBalanceSingle, negCurrency(cosBalanceSingle)),
                                              negCurrency(expensesBalanceSingle))
                + "," + addCurrency(
                            addCurrency(incomeBalance, negCurrency(cosBalance)),
                            negCurrency(expensesBalanceSingle))
                + "\n\n";


    report->data += "</table></center>";

    report->string = "<qt>" + report->header + report->data + "</qt>";

    report->html = report->string.replace(
            "<qt>", "<html><head><title>" + curClient +
            " - Income Statement Retail</title></head></body>\n").replace(
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

QString& Database::journalTmpReportHeader()
{
    return journalTmpReport->header;
}

QString& Database::journalTmpReportData()
{
    return journalTmpReport->data;
}

QStringList& Database::getAccountsList()
{
    accountsList->clear();
    
    QSqlQuery query("SELECT * FROM accounts ORDER BY id", connection);
    while(query.next())
        accountsList->append(query.value(1).toString() + " - " + query.value(2).toString());

    return *accountsList;
    
}

QPtrList<QStringList>* Database::getAccountsData()
{
    QPtrList<QStringList>* data = new QPtrList<QStringList>;
    
    QSqlQuery query("SELECT * FROM accounts ORDER BY id", connection);
    while(query.next())
    {
        QStringList *list = new QStringList;
        *list << query.value(0).toString() << query.value(1).toString()
                << query.value(2).toString() << query.value(3).toString();
        data->append(list);
    }
    return data;
}

QPtrList<QStringList>* Database::getJournalTmpData()
{
    QPtrList<QStringList>* data = new QPtrList<QStringList>;
    
    QSqlQuery query("SELECT * FROM journalTmp ORDER BY key", connection);
    while(query.next())
    {
        QStringList *list = new QStringList;
        *list << query.value(0).toString() << query.value(1).toString() << query.value(2).toString()
                << query.value(3).toString() << query.value(4).toString() << query.value(5).toString()
                << query.value(6).toString();
        data->append(list);
    }
    return data;
}

void Database::updateAccount(QString key, QString id, QString desc)
{
    QSqlQuery query("SELECT key FROM accounts WHERE key = " + key, connection);
    
    QString queryString;
    
    if(query.first())
        queryString = "UPDATE accounts SET id = " + id + ", desc = \"" + desc + "\" WHERE key = " + key;
    else
        queryString = "INSERT INTO accounts VALUES(\"" + key + "\", \"" + id + "\", \"" + desc + "\")";
    
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
    QSqlQuery query("SELECT key FROM journalTmp WHERE key = " + key, connection);
    
    QString queryString;
    
    if(query.first())
        queryString = "UPDATE journalTmp SET date = \"" + date + "\", account = \"" + account +
                "\", reference = \"" + reference + "\", desc = \"" + desc + "\", debit = \"" +
                debit + "\", credit = \"" + credit + "\" WHERE key =  " + key;
    else
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

QString Database::addCurrency(QString base, QString add) const
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

QString Database::negCurrency(QString currency) const
{
    QString result(currency);

    if(result.left(1) == "-")
        result = result.mid(1);
    else
        result = "-" + result;

    return result;
}

QString Database::addCommas(QString currency) const
{
    bool neg = false;
    QString result;
    if(currency.left(1) == "-")
    {
        neg = true;
        result = currency.mid(1);
    }
    else
        result = currency;

    std::div_t divResult = div(result.length() - 3, 3);
    int start = divResult.rem;
    int times = divResult.quot;

    for(int i = 0; i < times; i++)
    {
        if(start == 0)
            continue;

        result.insert(start + i, ",");

        start += 3;
    }

    if(neg)
        return negCurrency(result);
    else
        return result;
}

QString Database::formatCurrency(QString currency) const
{
    QString result = addCommas(currency);

    if(result.left(1) == "-")
        result = "(" + result.mid(1) + ")";

    return result;
}

void Database::commitJournalTmp()
{
    QStringList data;
    
    QSqlQuery query("SELECT * FROM journalTmp", connection);
    while(query.next())
    {
        data << ("\"" + query.value(1).toString() + "\", \"" + query.value(2).toString()
                + "\", \"" + query.value(3).toString() + "\", \"" + query.value(4).toString()
                + "\", \"" + query.value(5).toString() + "\", \"" + query.value(6).toString() + "\"");
    }
    
    for(QStringList::Iterator it = data.begin(); it != data.end(); ++it)
    {
        query.prepare("INSERT INTO journal VALUES(" + *it + ")");
        query.exec();
    }
    
    query.prepare("DROP TABLE journalTmp");
    query.exec();
    
    query.prepare("CREATE TABLE journalTmp (key, date, account, reference, "
            "desc, debit, credit, PRIMARY KEY (key))");
    query.exec();
    
    emit journalTmpChanged();
}

bool Database::journalTmpEmpty()
{
    QSqlQuery query("SELECT * FROM journalTmp", connection);
    if(query.next())
        return false;
    else
        return true;
}

QString Database::nextAccountKey()
{
    QSqlQuery query("SELECT MAX(key) FROM accounts", connection);
    
    int key = 0;
    if(query.first())
        key = query.value(0).toInt();
    
    return QString::number(++key);
}

QString Database::nextJournalTmpKey()
{
    QSqlQuery query("SELECT MAX(key) FROM journalTmp", connection);
    
    int key = 0;
    if(query.first())
        key = query.value(0).toInt();
    
    return QString::number(++key);
}

QString Database::getAccountDesc(QString account)
{
    QSqlQuery query("SELECT desc FROM accounts WHERE id = " + account, connection);
    if(query.next())
        return query.value(0).toString();
    else
        return QString::null;
}

bool Database::accountExists(QString account)
{
    QSqlQuery query("SELECT * FROM accounts WHERE id = " + account, connection);
    if(query.first())
        return true;
    else
        return false;
}

bool Database::accountHasEntries(QString account)
{
    QSqlQuery query("SELECT * FROM journal WHERE account = " + account, connection);
    if(query.first())
        return true;
    else
        return false;
}

Database *Database::db = 0;

