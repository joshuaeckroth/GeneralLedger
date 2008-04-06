#include <qtable.h>
#include <qlineedit.h>

#include "dateEdit.h"

DateEdit::DateEdit(QTable *table, const QString &contents)
    : QTableItem(table, QTableItem::OnTyping, contents)
{}

QWidget* DateEdit::createEditor() const
{
    QLineEdit *lineEdit = new QLineEdit(table()->viewport());
    lineEdit->setInputMask("99/99/99");
    lineEdit->setText(text());
    lineEdit->selectAll();
    return lineEdit;
}
