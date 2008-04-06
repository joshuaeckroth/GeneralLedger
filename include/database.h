#ifndef DATABASE_H
#define DATABASE_H

#include <qobject.h>
#include <qptrlist.h>

class QString;
class QStringList;
class QSqlDatabase;

class BalanceElement;

class Database : public QObject
{
    Q_OBJECT
 public:
  Database();
  ~Database();
  
  bool openDefault();
  bool openNew(QString name);
  bool createNew(QString name, QString yearEnd);
  void closeDb();
  void copyDb(QString file) const;
  void editName(QString name);
  QString getCurDb() const;
  QString getCurClient() const;
  QStringList getClientList() const;
  void exportCSV(QString table, QString file) const;
  void importCSV(QString table, QString file);
  QStringList getPeriodBegin() const;
  QStringList getPeriodEnd() const;
  
  enum balanceCategory {assets, liabilities, equities};
  
  QPtrList<BalanceElement> getBalanceElements(balanceCategory category);
  
  void createBalanceElement(balanceCategory category, QString type, QString desc, QString accountBegin,
                            QString accountEnd = "");
  void removeBalanceElement(balanceCategory category, QString key);
  void moveBalanceElement(balanceCategory category, QString key, bool up);
  
  Database* getChartAccounts();
  
  QString& reportString();
  QString& reportCSV();
  QString& reportHTML();
  
  QStringList& getAccountsList();
  QPtrList<QStringList> getAccountsData();
  QPtrList<QStringList> getJournalTmpData();
  
  void updateAccount(QString key, QString id, QString desc, QString type);
  void deleteAccount(QString key);
  void updateJournalTmp(QString key, QString date, QString account,
                        QString reference, QString desc, QString debit, QString credit);
  void deleteJournalTmp(QString key);
  QString nextAccountKey();
  QString nextJournalTmpKey();
  
    signals:
        void accountsChanged();

 private:
  static int instances;
  static QSqlDatabase *db;
  static QString curClient;
  static QString curDb;
  
  struct ReportStruct {
      QString string;
      QString csv;
      QString html;
  };
  static ReportStruct *report;
  
  static QStringList *accountsList;
  
};

#endif
