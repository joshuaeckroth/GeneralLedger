#include <qtable.h>

#include "moneyEdit.h"

MoneyEdit::MoneyEdit(QTable *table, const QString &contents)
    : QTableItem(table, QTableItem::OnTyping, contents)
{}

