#include <qtable.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qevent.h>
#include <qmessagebox.h>

#include "journalTable.h"
#include "accountEditItem.h"
#include "basicEdit.h"
#include "dateEdit.h"
#include "moneyEdit.h"
#include "database.h"

JournalTable::JournalTable(QWidget *parent, const char *name)
    : QTable(parent,name)  
{
    db = new Database;
    connect(db, SIGNAL(accountsChanged()), this, SLOT(updateAccounts()));
        
    setLeftMargin(0);
    setVScrollBarMode(QScrollView::AlwaysOn);
    setSelectionMode(QTable::SingleRow);
    setFocusStyle(QTable::SpreadSheet);
    
    insertColumns(0,6);
    
    QStringList labels;
    labels << "Date" << "Account" << "Reference" << "Description" << "Debit" << "Credit";
    setColumnLabels(labels);
              
    setColumnStretchable(3, true);
    
    accounts = db->getAccountsList();
    
    populate();
    
    sortColumn(0, FALSE, TRUE);
    
    connect(this, SIGNAL(valueChanged(int, int)), this, SLOT(updateDb(int, int)));    
}

JournalTable::~JournalTable()
{
    delete db;
}

void JournalTable::populate()
{
    for(int row = 0; row < numRows(); row++)
        removeRow(0);
    
    QPtrList<QStringList> data = db->getJournalTmpData();
    QPtrListIterator<QStringList> it(data);
    
    QStringList *stringList;
    while((stringList = it.current()) != 0)
    {
        ++it;
        insertRows(0);
        setItem(0, 0, new DateEdit(this, stringList->operator[](1)));
        setItem(0, 1, new AccountEditItem(this, accounts, stringList->operator[](2), stringList->operator[](0)));
        setItem(0, 2, new BasicEdit(this, stringList->operator[](3)));
        setItem(0, 3, new BasicEdit(this, stringList->operator[](4)));
        setItem(0, 4, new MoneyEdit(this, stringList->operator[](5)));
        setItem(0, 5, new MoneyEdit(this, stringList->operator[](6)));
        delete stringList;
    }
}

void JournalTable::insert()
{
    int newRow = numRows();
    insertRows(newRow);
    ensureCellVisible(newRow, 0);
    setItem(newRow, 0, new DateEdit(this, ""));
    setItem(newRow, 1, new AccountEditItem(this, accounts, "", db->nextJournalTmpKey()));
    setItem(newRow, 2, new BasicEdit(this, ""));
    setItem(newRow, 3, new BasicEdit(this, ""));
    setItem(newRow, 4, new MoneyEdit(this, ""));
    setItem(newRow, 5, new MoneyEdit(this, ""));
    editCell(newRow, 0);
}

void JournalTable::remove(int row)
{
    QString key = ((AccountEditItem*)(item(row, 1)))->getKey();
    
    if(!QMessageBox::question(
        this,
    "Delete Record",
    "Are you sure you want to delete the selected record?",
    "Delete", "Don't Delete",
    QString::null, 1, 0))
    {
        db->deleteJournalTmp(key);
        removeRow(row);
    }
}

void JournalTable::updateDb(int row, int)
{
    QString key = ((AccountEditItem*)(item(row, 1)))->getKey();
    QString date = text(row, 0);
    QString account = text(row, 1);
    QString reference = text(row, 2);
    QString desc = text(row, 3);
    QString debit = text(row, 4);
    QString credit = text(row, 5);
    db->updateJournalTmp(key, date, account, reference, desc, debit, credit);
}

void JournalTable::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Key_Insert:
            insert();
            break;
        case Key_Delete:
            remove(currentRow());
            break;
        case Key_F1:
            emit goToMain();
            break;
        case Key_F2:
            emit goToAccounts();
            break;
        case Key_F3:
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

int JournalTable::debitColWidth()
{
    return columnWidth(4);
}

int JournalTable::creditColWidth()
{
    return columnWidth(5);
}

void JournalTable::updateAccounts()
{
    accounts = db->getAccountsList();
}

