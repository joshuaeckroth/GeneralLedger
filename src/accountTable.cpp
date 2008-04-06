#include <qtable.h>
#include <qptrlist.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

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

    iconPath = settings->getIconPath();
    
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
    
    inserting = false;
    editting = false;
    
    connect(this, SIGNAL(currentChanged(int, int)), this, SLOT(edittingChanged(int, int)));
    connect(this, SIGNAL(valueChanged(int, int)), this, SLOT(updateDb(int, int)));
    
}

AccountTable::~AccountTable()
{}

void AccountTable::populate()
{
    int rows = numRows();
    for(int row = 0; row < rows; row++)
        removeRow(0);
    
    QPtrList<QStringList> data = db->getAccountsData();
    QPtrListIterator<QStringList> it(data);
    
    QStringList *stringList;
    while((stringList = it.current()) != 0)
    {
        ++it;
        insertRows(0);
        setItem(0, 0, new AccountEditItem(this, stringList->operator[](1), stringList->operator[](0)));
        setItem(0, 1, new QTableItem(this, QTableItem::OnTyping, stringList->operator[](2)));
        setItem(0, 2, new AccountEditType(this, stringList->operator[](3)));
        delete stringList;
    }
    
    if(numRows())
    {
        sortColumn(0, true, true);

        setCurrentCell(0,0);
        edittingRow = 0;
        edittingCol = 0;
    }
}

void AccountTable::insert()
{
    inserting = true;

    int newRow = numRows();
    insertRows(newRow);

    editting = true;
    edittingRow = newRow;
    edittingCol = 0;

    ensureCellVisible(newRow, 0);
    setItem(newRow, 0, new AccountEditItem(this, "", db->nextAccountKey()));
    setItem(newRow, 1, new QTableItem(this, QTableItem::OnTyping, ""));
    setItem(newRow, 2, new AccountEditType(this, "Normal"));
    setCurrentCell(newRow,0);

    beginEdit(newRow, 0, false);
}

void AccountTable::remove(int row)
{
    QString key = ((AccountEditItem*)(item(row, 0)))->getKey();
    
    QDialog dialog;
    dialog.setCaption("Delete Record");

    QVBoxLayout vBoxLayout(&dialog);
    vBoxLayout.setMargin(5);
    vBoxLayout.setSpacing(5);

    QLabel label("Are you sure you want to delete the selected record?", &dialog);

    vBoxLayout.addWidget(&label);

    vBoxLayout.addSpacing(10);

    QHBoxLayout hBoxLayout(&vBoxLayout);
    hBoxLayout.setSpacing(5);

    QPushButton buttonDelete(QIconSet(QPixmap::fromMimeSource(iconPath + "/deleteButton.png")),
                             "Delete", &dialog);
    connect(&buttonDelete, SIGNAL(clicked()), &dialog, SLOT(accept()));

    QPushButton buttonDontDelete(QIconSet(QPixmap::fromMimeSource(iconPath + "/okButton.png")),
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
            setCurrentCell(row - 1, 0);
    }
}

void AccountTable::updateDb(int row, int)
{
    QString id = text(row, 0);
    QString key = ((AccountEditItem*)(item(row, 0)))->getKey();
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
            if(editting)
            {
                endEdit(edittingRow, edittingCol, true, true);

                if(edittingCol == 2)
                {
                    edittingCol = 0;
                    if(edittingRow == (numRows() - 1))
                    {
                        inserting = false;
                        editting = false;
                        sortColumn(0, true, true);

                        setCurrentCell(currentRow(), 0);
                        break;
                    }
                    edittingRow++;
                }
                else
                {
                    edittingCol++;
                }
            }
            editting = true;

            setCurrentCell(edittingRow, edittingCol);
            beginEdit(edittingRow, edittingCol, false);
            break;
        case Key_Escape:
            if(editting)
            {
                endEdit(edittingRow, edittingCol, true, true);
                editting = false;
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
