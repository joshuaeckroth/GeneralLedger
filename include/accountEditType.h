#ifndef ACCOUNT_EDIT_TYPE_H
#define ACCOUNT_EDIT_TYPE_H

#include <qtable.h>

class QString;

class AccountEditType : public QTableItem
{
    public:
        AccountEditType(QTable *table, const QString &defaultContent);
        QWidget* createEditor() const;
        void setContentFromEditor(QWidget *widget);
        QString text() const;
       
    private:
        QString content;
};

#endif

