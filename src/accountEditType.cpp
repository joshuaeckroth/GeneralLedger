#include <qtable.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qcombobox.h>

#include "accountEditType.h"

AccountEditType::AccountEditType(QTable *table, const QString &defaultContent)
    : QTableItem(table, QTableItem::OnTyping), content(defaultContent)
{}

QWidget* AccountEditType::createEditor() const
{
    QComboBox *editor = new QComboBox(false, table()->viewport());
    editor->insertStringList(QStringList("Normal") << "Type 2" << "Type 3" << "Type 4");
    if(content == "Normal")
        editor->setCurrentItem(0);
    if(content == "Type 2")
        editor->setCurrentItem(1);
    if(content == "Type 3")
        editor->setCurrentItem(2);
    if(content == "Type 4")
        editor->setCurrentItem(3);
    return editor;
}

void AccountEditType::setContentFromEditor(QWidget *widget)
{
    content = ((QComboBox*)(widget))->currentText();
}

QString AccountEditType::text() const
{
    return content;
}

