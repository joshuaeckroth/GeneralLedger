#include <cstdlib>
using namespace std;

#include <qapplication.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qstring.h>

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    
    QSqlDatabase *connection = QSqlDatabase::addDatabase("QSQLITE");
    connection->setDatabaseName("test.db");
    if(!connection->open())
    {
        qDebug("Could not open database");
        return -1;
    }

    // must be same as in database.cpp
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

    // now add junk
    QSqlQuery("INSERT INTO main VALUES(\"Stress-Test Client\", \"December 31\")");

    qDebug("1000 accounts...");
    int accountId = 0;
    for(int accountKey = 1; accountKey <= 1000; accountKey++)
    {
        QSqlQuery("INSERT INTO accounts VALUES(\"" + QString::number(accountKey)
                  + "\", \"" + QString::number(accountId++) + "\", \"blah\")");
    }

    qDebug("10000 journal entries...");
    accountId = 0;
    for(int i = 1; i <= 1000; i++)
    {
        for(int j = 1; j <= 10; j++)
        {
            QString sql("INSERT INTO journal VALUES(\""
                        + QString::number(10 + int(2 * (((float)rand())/(RAND_MAX + 1.0f))))
                        + "/" + QString::number(10 + int(18 * (((float)rand())/(RAND_MAX + 1.0f))))
                        + "/04"
                        + "\", \"" + QString::number(accountId) + "\", \"REF\", "
                        + "\"Blah\", ");
            QString currency("\"" + QString::number(int(1000000000 * (((float)rand())/(RAND_MAX + 1.0f))))
                             + "." + QString::number(10 + int(89 * (((float)rand())/(RAND_MAX + 1.0f))))
                             + "\"");
            if(i % 2 == 0)
                sql += currency + ", \"\")";
            else
                sql += "\"\", " + currency + ")";

            QSqlQuery query(sql);
        }
        accountId++;
    }

    qDebug("1000 journal tmp entries");
    accountId = 0;
    for(int i = 1; i <= 1000; i++)
    {
        for(int j = 1; j <= 1; j++)
        {
            QString sql("INSERT INTO journalTmp VALUES(" + QString::number(i*j) + ", \""
                        + QString::number(10 + int(2 * (((float)rand())/(RAND_MAX + 1.0f))))
                        + "/" + QString::number(10 + int(18 * (((float)rand())/(RAND_MAX + 1.0f))))
                        + "/04"
                        + "\", \"" + QString::number(accountId) + "\", \"REF\", "
                        + "\"Blah2\", ");
            QString currency("\"" + QString::number(int(1000000000 * (((float)rand())/(RAND_MAX + 1.0f))))
                             + "." + QString::number(10 + int(89 * (((float)rand())/(RAND_MAX + 1.0f))))
                             + "\"");
            if(i % 2 == 0)
                sql += currency + ", \"\")";
            else
                sql += "\"\", " + currency + ")";

            QSqlQuery query(sql);
        }
        accountId++;
    }

    connection->close();
   
}

