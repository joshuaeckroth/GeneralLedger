#include <qtable.h>
#include <qptrlist.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qmessagebox.h>

#include "accountTable.h"
#include "database.h"
#include "accountEditItem.h"
#include "accountEditSimple.h"
#include "accountEditType.h"
#include "basicEdit.h"

AccountTable::AccountTable(QWidget *parent, const char *name)
    : QTable(parent,name)  
{
    db = new Database;
    
    setLeftMargin(0);
    setVScrollBarMode(QScrollView::AlwaysOn);
    setSelectionMode(QTable::SingleRow);
    setFocusStyle(QTable::SpreadSheet);
    
    insertColumns(0,3);
    
    QStringList labels;
    labels << "Account ID" << "Account Description" << "Account Type";
    setColumnLabels(labels);
    
    setColumnStretchable(1, true);
    
    populate();
    
    editting = false;
    
    sortColumn(0, FALSE, TRUE);
    
    connect(this, SIGNAL(currentChanged(int, int)), this, SLOT(edittingChanged(int, int)));
    connect(this, SIGNAL(valueChanged(int, int)), this, SLOT(updateDb(int, int)));
    
}

AccountTable::~AccountTable()
{
    delete db;
}

void AccountTable::populate()
{
    for(int row = 0; row < numRows(); row++)
        removeRow(0);
    
    QPtrList<QStringList> data = db->getAccountsData();
    QPtrListIterator<QStringList> it(data);
    
    QStringList *stringList;
    while((stringList = it.current()) != 0)
    {
        ++it;
        insertRows(0);
        setItem(0, 0, new AccountEditItem(this, stringList->operator[](1), stringList->operator[](0)));
        setItem(0, 1, new BasicEdit(this, stringList->operator[](2)));
        setItem(0, 2, new AccountEditType(this, stringList->operator[](3)));
        delete stringList;
    }
    
    if(numRows())
    {
        setCurrentCell(0,0);
        edittingRow = 0;
        edittingCol = 0;
    }
}

void AccountTable::insert()
{
    int newRow = numRows();
    insertRows(newRow);
    ensureCellVisible(newRow, 0);
    setItem(newRow, 0, new AccountEditItem(this, "", db->nextAccountKey()));
    setItem(newRow, 1, new BasicEdit(this, ""));
    setItem(newRow, 2, new AccountEditType(this, "Normal"));
    setCurrentCell(newRow,0);
    editting = true;
    edittingRow = newRow;
    edittingCol = 0;
    editCell(newRow, 0);
}

void AccountTable::remove(int row)
{
    QString key = ((AccountEditItem*)(item(row, 0)))->getKey();
    
    if(!QMessageBox::question(
        this,
        "Delete Record",
        "Are you sure you want to delete the selected record?",
        "Delete", "Don't Delete",
        QString::null, 1, 0))
    {
        db->deleteAccount(key);
        removeRow(row);
    }
    if(row > 0)
        setCurrentCell(row - 1, 0);
}

void AccountTable::updateDb(int row, int)
{
    QString key = ((AccountEditItem*)(item(row, 0)))->getKey();
    QString id = text(row, 0);
    QString desc = text(row, 1);
    QString type = text(row, 2);
    db->updateAccount(key, id, desc, type);
}

void AccountTable::edittingChanged(int newRow, int newCol)
{
    edittingRow = newRow;
    edittingCol = newCol;
}

void AccountTable::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Key_Insert:
            insert();
            break;
        case Key_Delete:
            remove(currentRow());
            break;
        case Key_Enter:
        case Key_Return:
        case Key_Tab:
            editting = true;
            if(editting)
            {
                endEdit(edittingRow, edittingCol, true, true);
                if(edittingCol == 2)
                {
                    edittingCol = 0;
                    if(edittingRow == numRows())
                    {
                        setCurrentCell(edittingRow, 0);
                        break;
                    }
                    edittingRow++;
                }
                else
                {
                    edittingCol++;
                }
            }
            beginEdit(edittingRow, edittingCol, false);
            break;
        case Key_F1:
            emit goToMain();
            break;
        case Key_F2:
            break;
        case Key_F3:
            emit goToJournal();
            break;
        case Key_F4:
            emit goToReports();
            break;
        case Key_F5:
            emit goToHelp();
            break;
        default:
            QTable::keyPressEvent(event);
    }
}
