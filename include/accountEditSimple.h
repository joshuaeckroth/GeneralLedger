#ifndef ACCOUNT_EDIT_SIMPLE_H
#define ACCOUNT_EDIT_SIMPLE_H

#include <qlineedit.h>

class AccountEditSimple : public QLineEdit
{
    public:
        AccountEditSimple(QWidget *parent, const char *name, const QString &contents);
       
};

#endif

