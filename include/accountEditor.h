#ifndef ACCOUNT_EDITOR_H
#define ACCOUNT_EDITOR_H

#include <qlineedit.h>

class AccountEditor : public QLineEdit
{
    public:
        AccountEditor(QWidget *parent = 0, const char *name = 0);
};

#endif

