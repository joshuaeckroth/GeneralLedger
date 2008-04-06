#include <qtable.h>

#include "dateEdit.h"

DateEdit::DateEdit(QTable *table, const QString &contents)
    : QTableItem(table, QTableItem::OnTyping, contents)
{}

