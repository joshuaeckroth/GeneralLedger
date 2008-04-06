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
  bool createNew(QString name);
  void closeDb();
  void copyDb(QString file) const;
  void editName(QString name);
  QString getCurDb() const;
  QString getCurClient() const;
  QStringList getClientList() const;
  void exportCSV(QString table, QString file) const;
  void importCSV(QString table, QString file);
  
  enum balanceCategory {assets, liabilities, equities};
  
  QPtrList<BalanceElement> getBalanceElements(balanceCategory category);
  
  void createBalanceElement(balanceCategory category, QString type, QString desc, QString accountBegin,
                            QString accountEnd = "");
  
  void removeBalanceElement(balanceCategory category, QString id);
  
  void moveBalanceElement(balanceCategory category, QString id, bool up);
  
  Database* getChartAccounts();
  
  QString& reportString();
  QString& reportCSV();
  QString& reportHTML();
  
  QStringList& getAccountsList();

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
