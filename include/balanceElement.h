#ifndef BALANCE_ELEMENT_H
#define BALANCE_ELEMENT_H

#include <qobject.h>

#include "database.h"

class QFrame;
class QString;
class QHBoxLayout;
class QLabel;
class QPushButton;

class Database;

class BalanceElement : public QObject
{
    Q_OBJECT
 public:
     BalanceElement(Database::balanceCategory newCategory, QString newId, QString newType,
                    QString newDesc, QString newAccountBegin, QString newAccountEnd);
     
     ~BalanceElement();
     
     QFrame* getFrame(QWidget *parent, void *section);
     
    signals:
        void changed(void*);
        
    private slots:
        void elementUp();
        void elementDown();
        void removeElement();
     
    private:
        Database::balanceCategory category;        
        void *section;
        
        Database *db;
        
        QString id;
     QString type;
     QString desc;
     QString accountBegin;
     QString accountEnd;
     
     QFrame *frame;
     QHBoxLayout *layout;
     QLabel *descLabel;
     QLabel *accountsLabel;
     QPushButton *upButton;
     QPushButton *downButton;
     QPushButton *removeButton;
};

#endif
