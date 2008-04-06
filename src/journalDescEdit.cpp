#include <qtable.h>
#include <qlineedit.h>

#include "journalDescEdit.h"
#include "journalTable.h"

JournalDescEdit::JournalDescEdit(JournalTable *table, const QString &contents)
    : QTableItem(table, QTableItem::OnTyping, contents), journalTable(table)
{}

QWidget* JournalDescEdit::createEditor() const
{
    QLineEdit *lineEdit = new QLineEdit(table()->viewport());
    
    lineEdit->setText(text());
    
    if(journalTable->isInserting())
        lineEdit->selectAll();

    return lineEdit;
}
