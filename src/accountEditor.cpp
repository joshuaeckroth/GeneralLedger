
#include <qlineedit.h>

#include "accountEditor.h"

AccountEditor::AccountEditor(QWidget *parent, const char *name)
    : QLineEdit(parent,name)
{
    setAlignment(Qt::AlignRight);
    setInputMask("000009");
}


