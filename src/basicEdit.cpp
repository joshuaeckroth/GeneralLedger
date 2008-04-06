#include <qtable.h>
#include <qlineedit.h>
#include <qevent.h>

#include "basicEdit.h"

BasicEdit::BasicEdit(QTable *table, const QString &contents)
    : QTableItem(table, QTableItem::OnTyping, contents)
{}
