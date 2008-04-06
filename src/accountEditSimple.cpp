#include <qlineedit.h>

#include "accountEditSimple.h"

AccountEditSimple::AccountEditSimple(QWidget *parent, const char *name, const QString &contents)
    : QLineEdit(contents,parent,name)
{}

