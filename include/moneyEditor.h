#ifndef MONEY_EDITOR_H
#define MONEY_EDITOR_H

#include <qlineedit.h>

class MoneyEditor : public QLineEdit
{
    public:
        MoneyEditor(QWidget *parent = 0, const char *name = 0);
};

#endif
 