#ifndef DATABASE_H
#define DATABASE_H

#include <qobject.h>
#include <qptrlist.h>

class QString;
class QStringList;
class QSqlDatabase;

class EditorElement;
class Settings;

class Database : public QObject
{
    Q_OBJECT
    public:
        static Database* instance();

        void destroy();

        bool openNew(QString file, QString password);
        bool createNew(QString name, QString yearEnd);
        void closeDb();
        void copyDb(QString file) const;
        void encryptDb(QString password);
        void decryptDb();
        bool isEncrypted() const;
        void importDb(QString file) const;
        void editName(QString name);
        QString getCurDb() const;
        QString getCurClient() const;
        bool curClientExists() const;
        QStringList getClientList() const;
        void exportCSV(QString table, QString file) const;
        void importCSV(QString table, QString file);
        QStringList getPeriodBegin() const;
        QStringList getPeriodEnd() const;
  
        enum editorCategories {balanceAssets, balanceLiabilities, balanceEquities,
                               incomeSvcIncome, incomeSvcExpenses,
                               incomeRetailIncome, incomeRetailCOS, incomeRetailExpenses};
  
        QPtrList<EditorElement> getElements(editorCategories category);
  
        void createElement(editorCategories category, QString type, QString desc, QString accountBegin,
                           QString accountEnd = "");
        void removeElement(editorCategories category, QString key);
        void moveElement(editorCategories category, QString key, bool up);
  
        Database* getGeneralDetail(QString periodBegin, QString periodEnd, QString accountBegin, QString accountEnd);
        Database* getGeneralTrialBalance(QString periodEnd, QString accountBegin, QString accountEnd);
        Database* getBalanceReport(QString periodEnd);
        Database* getChartAccounts();
        Database* getJournalTmpReport();
        Database* getIncomeSvcReport(QString periodEnd);
        Database* getIncomeRetailReport(QString periodEnd);
  
        QString& reportHeader();
        QString& reportData();

        QString& reportString();
        QString& reportCSV();
        QString& reportHTML();

        QString& journalTmpReportHeader();
        QString& journalTmpReportData();
  
        QStringList& getAccountsList();
        QPtrList<QStringList>* getAccountsData();
        QPtrList<QStringList>* getJournalTmpData();
  
        void updateAccount(QString key, QString id, QString desc);
        void deleteAccount(QString key);
        void updateJournalTmp(QString key, QString date, QString account,
                              QString reference, QString desc, QString debit, QString credit);
        void deleteJournalTmp(QString key);
  
        QString journalTmpDebit();
        QString journalTmpCredit();
        QString journalTmpBalance();

        QString addCurrency(QString base, QString add) const;
        QString negCurrency(QString currency) const;
        QString addCommas(QString currency) const;
        QString formatCurrency(QString currency) const;
  
        void commitJournalTmp();

        bool journalTmpEmpty();
  
        QString nextAccountKey();
        QString nextJournalTmpKey();

        QString getAccountDesc(QString account);
        bool accountExists(QString account);
        bool accountHasEntries(QString account);
  
    signals:
        void accountsChanged();
        void journalTmpChanged();

    private:
        Database();
        ~Database();
  
        static Database *db;

        Settings *settings;

        QString clientPath;
     
        QSqlDatabase *connection;
        QString curClient;
        QString curDb;
  
        struct ReportStruct {
            QString header;
            QString data;
            QString string;
            QString csv;
            QString html;
        };
        ReportStruct *report;

        struct JournalTmpReport {
            QString header;
            QString data;
        };
        JournalTmpReport *journalTmpReport;
  
        QStringList *accountsList;
  
};

#endif
