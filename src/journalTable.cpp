#include <qtable.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qevent.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include "config.h"
#include "database.h"
#include "journalTable.h"
#include "journalDescEdit.h"
#include "accountEditItem.h"
#include "dateEdit.h"
#include "moneyEditItem.h"

JournalTable::JournalTable(QWidget *parent, const char *name)
    : QTable(parent,name)  
{
    db = Database::instance();
    
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
    
    inserting = false;
    editing = false;
    
    connect(this, SIGNAL(currentChanged(int, int)), this, SLOT(editingChanged(int, int)));
    connect(this, SIGNAL(valueChanged(int, int)), this, SLOT(updateDb(int, int)));    
}

JournalTable::~JournalTable()
{}

void JournalTable::populate()
{
    for(int row = 0; row <= numRows(); row++)
        removeRow(0);
    
    QPtrList<QStringList>* data = db->getJournalTmpData();
    QPtrListIterator<QStringList> it(*data);
    
    QStringList *stringList;
    while((stringList = it.current()) != 0)
    {
        ++it;
        insertRows(0);
        setItem(0, 0, new DateEdit(this, stringList->operator[](1)));
        setItem(0, 1, new AccountEditItem(this, accounts, stringList->operator[](2), stringList->operator[](0)));
        setItem(0, 2, new QTableItem(this, QTableItem::OnTyping, stringList->operator[](3)));
        setItem(0, 3, new JournalDescEdit(this, stringList->operator[](4)));
        setItem(0, 4, new MoneyEditItem(this, stringList->operator[](5)));
        setItem(0, 5, new MoneyEditItem(this, stringList->operator[](6)));
        delete stringList;
    }

    delete data;

    if(numRows())
    {
        setCurrentCell(0,0);
        editingRow = 0;
        editingCol = 0;
    }
}

void JournalTable::insert()
{
    inserting = true;

    int newRow = numRows();
    insertRows(newRow);

    editing = true;
    editingRow = newRow;
    editingCol = 0;

    ensureCellVisible(newRow, 0);
    setItem(newRow, 0, new DateEdit(this, ""));
    setItem(newRow, 1, new AccountEditItem(this, accounts, "", db->nextJournalTmpKey()));
    setItem(newRow, 2, new QTableItem(this, QTableItem::OnTyping));
    setItem(newRow, 3, new JournalDescEdit(this, ""));
    setItem(newRow, 4, new MoneyEditItem(this, ""));
    setItem(newRow, 5, new MoneyEditItem(this, ""));
    setCurrentCell(newRow,0);

    beginEdit(newRow, 0, false);
}

bool JournalTable::isInserting() const
{
    return inserting;
}

void JournalTable::remove(int row)
{
    QString key = ((AccountEditItem*)(item(row, 1)))->getKey();
    
    QDialog dialog;
    dialog.setCaption("Delete Record");

    QVBoxLayout vBoxLayout(&dialog);
    vBoxLayout.setMargin(5);
    vBoxLayout.setSpacing(5);

    QLabel label("<nobr>Are you sure you want to delete the selected record?</nobr>", &dialog);

    vBoxLayout.addWidget(&label);

    vBoxLayout.addSpacing(10);

    QHBoxLayout hBoxLayout(&vBoxLayout);
    hBoxLayout.setSpacing(5);

    QPushButton buttonDelete(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/deleteButton.png")),
                             "Delete", &dialog);
    connect(&buttonDelete, SIGNAL(clicked()), &dialog, SLOT(accept()));

    QPushButton buttonDontDelete(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/okButton.png")),
                                 "Don't Delete", &dialog);
    connect(&buttonDontDelete, SIGNAL(clicked()), &dialog, SLOT(reject()));

    buttonDontDelete.setDefault(true);

    hBoxLayout.addStretch();
    hBoxLayout.addWidget(&buttonDelete);
    hBoxLayout.addWidget(&buttonDontDelete);

    if(dialog.exec())
    {
        db->deleteJournalTmp(key);
        removeRow(row);
    }
    if(row > 0)
        setCurrentCell(row - 1, 0);
}

void JournalTable::updateDb(int row, int col)
{
    QString key = ((AccountEditItem*)(item(row, 1)))->getKey();
    QString date = text(row, 0);
    QString account = text(row, 1);
    QString reference = text(row, 2);
    QString desc = text(row, 3);
    QString debit = text(row, 4);
    QString credit = text(row, 5);
    db->updateJournalTmp(key, date, account, reference, desc, debit, credit);

    if(inserting && col == 1 && text(row, 3) == "")
        setText(row, 3, db->getAccountDesc(account));
}

void JournalTable::clearTable()
{
    int rows = numRows();
    for(int row = 0; row <= rows; row++)
        removeRow(0);
}

void JournalTable::editingChanged(int newRow, int newCol)
{
    editingRow = newRow;
    editingCol = newCol;
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
        case Key_Enter:
        case Key_Return:
        case Key_Tab:
            if(editing)
            {
                endEdit(editingRow, editingCol, true, true);
                if(editingCol == 5)
                {
                    editingCol = 0;
                    if(editingRow == (numRows() - 1))
                    {
                        inserting = false;
                        editing = false;
                        setCurrentCell(editingRow, 0);
                        break;
                    }
                    editingRow++;
                }
                else
                {
                    editingCol++;
                }
            }
            setCurrentCell(editingRow, editingCol);

            beginEdit(editingRow, editingCol, false);

            editing = true;
            break;
        case Key_Escape:
            if(editing)
            {
                endEdit(editingRow, editingCol, true, true);
                editing = false;
            }
            else
                emit goToMain();
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

