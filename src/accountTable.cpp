#include <qtable.h>
#include <qptrlist.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include "config.h"
#include "database.h"
#include "settings.h"
#include "accountTable.h"
#include "accountEditItem.h"
#include "accountEditType.h"

AccountTable::AccountTable(QWidget *parent, const char *name)
    : QTable(parent,name)  
{
    db = Database::instance();
    settings = Settings::instance();

    setLeftMargin(0);
    setVScrollBarMode(QScrollView::AlwaysOn);
    setSelectionMode(QTable::SingleRow);
    setFocusStyle(QTable::SpreadSheet);
    
    insertColumns(0,2);
    
    QStringList labels;
    labels << "Account ID" << "Account Description";
    setColumnLabels(labels);
    
    setColumnStretchable(1, true);
    
    populate();

    setCurrentCell(0,0);
    
    inserting = false;
    editing = false;
    
    connect(this, SIGNAL(currentChanged(int, int)), this, SLOT(editingChanged(int, int)));
    connect(this, SIGNAL(valueChanged(int, int)), this, SLOT(updateDb(int, int)));
    
}

AccountTable::~AccountTable()
{}

void AccountTable::populate()
{
    int rows = numRows();
    for(int row = 0; row < rows; row++)
        removeRow(0);
    
    QPtrList<QStringList>* data = db->getAccountsData();
    QPtrListIterator<QStringList> it(*data);
    
    QStringList *stringList;
    while((stringList = it.current()) != 0)
    {
        ++it;
        insertRows(0);
        setItem(0, 0, new AccountEditItem(this, stringList->operator[](1), stringList->operator[](0)));
        setItem(0, 1, new QTableItem(this, QTableItem::OnTyping, stringList->operator[](2)));
        delete stringList;
    }

    delete data;

    setColumnReadOnly(0, true);
    
    sortColumn(0, true, true);

    editingRow = 0;
    editingCol = 1;
}

void AccountTable::insert()
{
    inserting = true;

    int newRow = numRows();
    insertRows(newRow);

    editing = true;
    editingRow = newRow;
    editingCol = 0;

    ensureCellVisible(newRow, 0);
    setItem(newRow, 0, new AccountEditItem(this, "", db->nextAccountKey()));
    setColumnReadOnly(0, false);
    setItem(newRow, 1, new QTableItem(this, QTableItem::OnTyping, ""));
    setCurrentCell(newRow,0);

    beginEdit(newRow, 0, false);
}

void AccountTable::remove(int row)
{
    QString key = ((AccountEditItem*)(item(row, 0)))->getKey();

    if(db->accountHasEntries(text(row, 0)))
    {
        QDialog dialog;
        dialog.setCaption("Delete Record");

        QVBoxLayout vBoxLayout(&dialog);
        vBoxLayout.setMargin(5);
        vBoxLayout.setSpacing(5);

        QLabel label("<nobr>Cannot delete account " + text(row, 0) + "</nobr>;<br>"
                "<nobr>there is data associated with this account.</nobr>", &dialog);
        
        vBoxLayout.addWidget(&label);

        vBoxLayout.addSpacing(10);

        QHBoxLayout hBoxLayout(&vBoxLayout);
        hBoxLayout.setSpacing(5);

        QPushButton okButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/okButton.png")),
                             "OK", &dialog);
        connect(&okButton, SIGNAL(clicked()), &dialog, SLOT(accept()));

        hBoxLayout.addStretch();
        hBoxLayout.addWidget(&okButton);

        dialog.exec();

        return;
    }
    
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
        db->deleteAccount(key);
        removeRow(row);

        if(row > 0)
            setCurrentCell(row - 1, 1);
    }
}

void AccountTable::updateDb(int row, int)
{
    QString id = text(row, 0);
    QString key = ((AccountEditItem*)(item(row, 0)))->getKey();
    QString desc = text(row, 1);
    db->updateAccount(key, id, desc);

    setColumnReadOnly(0, true);
    setCurrentCell(row, 1);
}

void AccountTable::editingChanged(int newRow, int newCol)
{
    editingRow = newRow;
    editingCol = newCol;
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
            if(editing)
            {
                if(inserting)
                {
                    if(editingCol == 1)
                    {
                        endEdit(editingRow, editingCol, true, true);
                        editing = false;
                        inserting = false;
                        break;
                    }

                    endEdit(editingRow, editingCol, true, true);
                    sortColumn(0, true, true);

                    editingRow = currentRow();
                }
                else if(editingRow != (numRows() - 1))
                {
                    endEdit(editingRow, editingCol, true, true);
                    editingRow++;
                }
                else
                {
                    endEdit(editingRow, editingCol, true, true);
                    editing = false;
                    break;
                }
            }
            editing = true;
            editingCol = 1;

            setCurrentCell(editingRow, editingCol);
            beginEdit(editingRow, editingCol, false);
            break;
        case Key_Escape:
            if(editing)
            {
                endEdit(editingRow, editingCol, true, true);
                editing = false;
                inserting = false;
            }
            else
                emit goToMain();
            break;
        case Key_Up:
        case Key_Left:
        case Key_Down:
        case Key_Right:
            if(inserting)
                sortColumn(0, true, true);
            inserting = false;

            QTable::keyPressEvent(event);
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
