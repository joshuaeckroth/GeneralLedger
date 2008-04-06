#include <qtable.h>
#include <qlineedit.h>

#include "moneyEditItem.h"
#include "moneyValidator.h"

MoneyEditItem::MoneyEditItem(QTable *table, const QString &contents)
    : QTableItem(table, QTableItem::OnTyping, contents)
{}

QWidget* MoneyEditItem::createEditor() const
{
    QLineEdit *lineEdit = new QLineEdit(text(), table()->viewport());
    lineEdit->setValidator(new MoneyValidator(lineEdit));
    return lineEdit;
}
