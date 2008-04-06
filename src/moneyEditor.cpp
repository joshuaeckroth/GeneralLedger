
#include <qlineedit.h>

#include "moneyEditor.h"

MoneyEditor::MoneyEditor(QWidget *parent, const char *name)
    : QLineEdit(parent,name)
{
    setAlignment(Qt::AlignRight);
    setInputMask("000000000000000.99");
}


