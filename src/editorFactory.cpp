
#include <qsqleditorfactory.h>
#include <qwidget.h>
#include <qsqlfield.h>

#include "editorFactory.h"
#include "dateEditor.h"
#include "moneyEditor.h"
#include "accountEditor.h"

EditorFactory::EditorFactory(QObject *parent, const char *name)
    : QSqlEditorFactory(parent,name)
{} 

QWidget* EditorFactory::createEditor(QWidget *parent, const QSqlField *field)
{
    if(field->name() == "date")
    {
        DateEditor *dateEditor = new DateEditor(parent);
//         connect(dateEditor, SIGNAL(advance()), this, SIGNAL(advance()));
        return dateEditor;
    }
    else if(field->name() == "credit" || field->name() == "debit")
    {
        MoneyEditor *moneyEditor = new MoneyEditor(parent);
//         connect(moneyEditor, SIGNAL(advance()), this, SIGNAL(advance()));
        return moneyEditor;
    }
    else if(field->name() == "account")
    {
        AccountEditor *accountEditor = new AccountEditor(parent);
//         connect(accountEditor, SIGNAL(advance()), this, SIGNAL(advance()));
        return accountEditor;
    }
    else
    {
        QLineEdit *lineEditor = new QLineEdit(parent);
        return lineEditor;
    }
}

