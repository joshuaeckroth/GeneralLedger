/* General Ledger, Copyright (C) 2004  Joshua Eckroth <josh@eckroth.net>
 * http://www.eckroth.net/software/genledg
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  US
*/

#include <vector>
using std::vector;

#include <qtable.h>
#include <qptrlist.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include "config.h"
#include "accountsTable.h"
#include "accountsEditId.h"
#include "accountsData.h"
#include "client.h"

AccountsTable::AccountsTable(QWidget *parent, const char *name)
    : QTable(parent,name)  
{
    accountsData = AccountsData::instance();
    client = Client::instance();

    connect(client, SIGNAL(clientOpened(QString)), this, SLOT(populate()));
    connect(client, SIGNAL(clientClosed(QString)), this, SLOT(unpopulate()));

    setLeftMargin(0);
    setVScrollBarMode(QScrollView::AlwaysOn);
    setSelectionMode(QTable::NoSelection);
    setFocusStyle(QTable::SpreadSheet);
    
    insertColumns(0,2);
    
    QStringList labels;
    labels << "Account ID" << "Account Description";
    setColumnLabels(labels);
    
    setColumnStretchable(1, true);
    
    inserting = false;
    editing = false;
}

void AccountsTable::populate()
{
    unpopulate();
    
    vector<AccountsData::Account*> accountsVec = accountsData->getAccounts();
    Q_UINT16 key = 0;
    for(vector<AccountsData::Account*>::const_iterator it = accountsVec.begin();
                    it != accountsVec.end(); ++it)
    {
        key++;
        if((*it) != 0)
        {
            insertRows(0);
            setItem(0, 0, new AccountsEditId(this, (*it)->id, key));
            setItem(0, 1, new QTableItem(this, QTableItem::OnTyping,
                (*it)->desc));
        }
    }

    insertRows(0);
    setItem(0, 0, new QTableItem(this, QTableItem::Never,
        accountsData->getREAccountId()));
    setItem(0, 1, new QTableItem(this, QTableItem::OnTyping,
        accountsData->getREAccountDesc()));

    setColumnReadOnly(0, true);
    
    sortColumn(0, true, true);

    setCurrentCell(0,1);
}

void AccountsTable::unpopulate()
{
    int rows = numRows();
    for(int row = 0; row < rows; row++)
        removeRow(0);
}

void AccountsTable::insert()
{
    inserting = true;

    int newRow = numRows();
    insertRows(newRow);

    editing = true;

    ensureCellVisible(newRow, 0);
    setItem(newRow, 0, new AccountsEditId(this, "",
                            accountsData->getNextAccountKey()));
    setColumnReadOnly(0, false);
    setItem(newRow, 1, new QTableItem(this, QTableItem::Never, ""));
    setCurrentCell(newRow,0);

    beginEdit(newRow, 0, false);
}

void AccountsTable::remove(int row)
{
    if(text(row, 0) == accountsData->getREAccountId())
        return;

    Q_UINT16 key = ((AccountsEditId*)(item(row, 0)))->getKey();

    if(accountsData->accountHasEntries(text(row, 0)))
    {
        QDialog *dialog = new QDialog;
        dialog->setCaption("Delete Record");

        QVBoxLayout *vBoxLayout = new QVBoxLayout(dialog);
        vBoxLayout->setMargin(5);
        vBoxLayout->setSpacing(5);

        QLabel *label = new QLabel("<nobr>Cannot delete account " + text(row, 0)
                + "</nobr>;<br>"
                "<nobr>there is data associated with this account.</nobr>",
                dialog);
        
        vBoxLayout->addWidget(label);

        vBoxLayout->addSpacing(10);

        QHBoxLayout *hBoxLayout = new QHBoxLayout(vBoxLayout);
        hBoxLayout->setSpacing(5);

        QPushButton *okButton = new QPushButton(QIconSet(
                                QPixmap::fromMimeSource(ICON_PATH
                                + "/okButton.png")), "OK", dialog);
        connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));

        hBoxLayout->addStretch();
        hBoxLayout->addWidget(okButton);

        dialog->exec();

        delete dialog;
    }
    else
    {    
        QDialog *dialog = new QDialog;
        dialog->setCaption("Delete Record");

        QVBoxLayout *vBoxLayout = new QVBoxLayout(dialog);
        vBoxLayout->setMargin(5);
        vBoxLayout->setSpacing(5);

        QLabel *label = new QLabel("<nobr>Are you sure you want to delete the selected record?</nobr>", dialog);

        vBoxLayout->addWidget(label);

        vBoxLayout->addSpacing(10);

        QHBoxLayout *hBoxLayout = new QHBoxLayout(vBoxLayout);
        hBoxLayout->setSpacing(5);

        QPushButton *buttonDelete = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/deleteButton.png")),
                                 "Delete", dialog);
        connect(buttonDelete, SIGNAL(clicked()), dialog, SLOT(accept()));

        QPushButton *buttonDontDelete = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/okButton.png")),
                                     "Don't Delete", dialog);
        connect(buttonDontDelete, SIGNAL(clicked()), dialog, SLOT(reject()));

        buttonDontDelete->setDefault(true);

        hBoxLayout->addStretch();
        hBoxLayout->addWidget(buttonDelete);
        hBoxLayout->addWidget(buttonDontDelete);

        if(dialog->exec())
        {
            accountsData->deleteAccount(key);
            removeRow(row);

            if(row > 0)
                setCurrentCell(row - 1, 1);
            else
                setCurrentCell(0, 1);
        }

        delete dialog;
    }
}

void AccountsTable::updateDb()
{
    int row = currentRow();

    QString id = text(row, 0);

    if(id == accountsData->getREAccountId())
    {
        accountsData->setREAccountDesc(text(row,1));
        return;
    }

    Q_UINT16 key = ((AccountsEditId*)(item(row, 0)))->getKey();
    QString desc = text(row, 1);
    
    if(desc == "")
    {
        desc = "No Description";
        setText(currentRow(), 1, desc);
    }

    accountsData->updateAccount(key, id, desc);
}

void AccountsTable::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Key_Insert:
            if(!inserting)
                insert();
            break;
        case Key_Delete:
            if(!editing)
                remove(currentRow());
            else
                ((QLineEdit*)cellWidget(currentRow(), currentColumn()))->del();
            break;
        case Key_Tab:
            break;
        case Key_Enter:
        case Key_Return:
            if(inserting)
            {
                if(currentColumn() == 0)
                {
                    endEdit(currentRow(), 0, true, true);

                    if(accountsData->accountIdExists(text(currentRow(), 0)))
                    {
                        beginEdit(currentRow(), 0, true);
                        break;
                    }
                    else
                    {
                        setCurrentCell(currentRow(), 1);
                        beginEdit(currentRow(), 1, true);
                        break;
                    }
                }
                else
                {
                    editing = false;
                    inserting = false;
                    endEdit(currentRow(), 1, true, true);
                    updateDb();
                    sortColumn(0, true, true);
                    break;
                }
            }
            else
            {
                if(editing)
                {
                    editing = false;
                    endEdit(currentRow(), 1, true, true);
                    updateDb();
                    setCurrentCell(currentRow(), 1);
                    sortColumn(0, true, true);
                    break;
                }
                else
                {
                    editing = true;
                    QString oldText = text(currentRow(), 1);
                    QLineEdit *editor = (QLineEdit*)beginEdit(currentRow(), 1, true);
                    editor->insert(oldText);
                    editor->selectAll();
                    break;
                }
            }
        case Key_Escape:
            if(inserting)
            {
                editing = false;
                inserting = false;
                endEdit(currentRow(), currentColumn(), false, false);
                removeRow(currentRow());
                setCurrentCell(numRows(), 1);
                break;
            }
            if(editing)
            {
                editing = false;
                endEdit(currentRow(), currentColumn(), false, false);
                break;
            }

            emit switchToHome();
            break;
        case Key_Up:
        case Key_Left:
        case Key_Down:
        case Key_Right:
        case Key_Home:
        case Key_End:
        case Key_PageUp:
        case Key_PageDown:
            if(editing)
            {
                int curRow = currentRow();
                int curCol = currentColumn();

                switch(event->key())
                {
                    case Key_Right:
                        ((QLineEdit*)cellWidget(curRow, curCol))->cursorForward(false);
                        break;
                    case Key_Left:
                        ((QLineEdit*)cellWidget(curRow, curCol))->cursorBackward(false);
                        break;
                    case Key_Home:
                        ((QLineEdit*)cellWidget(curRow, curCol))->home(false);
                        break;
                    case Key_End:
                        ((QLineEdit*)cellWidget(curRow, curCol))->end(false);
                        break;
                    default:
                        break;
                }
            }
            else
            {
                if(event->key() == Key_Left)
                    break;
                
                QTable::keyPressEvent(event);
                break;
            }
        case Key_F1:
            emit switchToHome();
            break;
        case Key_F2:
            break;
        case Key_F3:
            emit switchToJournal();
            break;
        case Key_F4:
            emit switchToReports();
            break;
        case Key_F5:
            emit switchToHelp();
            break;
        default:
            break;
    }
}

