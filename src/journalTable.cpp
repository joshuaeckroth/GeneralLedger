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

#include <cmath>
using std::abs;

#include <qtable.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>

#include "config.h"
#include "journalTable.h"
#include "journalData.h"
#include "journalEditDesc.h"
#include "journalEditDate.h"
#include "journalEditCurrency.h"
#include "journalEditCurrencyValidator.h"
#include "journalEditAccount.h"
#include "accountsData.h"
#include "client.h"

JournalTable::JournalTable(QWidget *parent, const char *name)
    : QTable(parent,name)  
{
    journalData = JournalData::instance();
    accountsData = AccountsData::instance();
    client = Client::instance();

    connect(accountsData, SIGNAL(accountsChanged()), this, SLOT(updateAccounts()));

    connect(client, SIGNAL(clientOpened(QString)), this, SLOT(populate()));
    connect(client, SIGNAL(clientClosed(QString)), this, SLOT(unpopulate()));

    setLeftMargin(0);
    setVScrollBarMode(QScrollView::AlwaysOn);
    setSelectionMode(QTable::NoSelection);
    setFocusStyle(QTable::SpreadSheet);
    
    insertColumns(0,6);
    
    QStringList labels;
    labels << "Date" << "Account" << "Reference" << "Description" << "Debit" << "Credit";
    setColumnLabels(labels);
              
    setColumnStretchable(3, true);

    setColumnWidth(1, columnWidth(1) + 40);
    
    inserting = false;
    editing = false;
}

void JournalTable::populate()
{
    unpopulate();

    accounts = accountsData->getFormattedList();

    vector<JournalData::JournalRecord*> records = journalData->getJournalTmpRecords();
    Q_UINT16 key = 0;
    for(vector<JournalData::JournalRecord*>::const_iterator it = records.begin();
                    it != records.end(); ++it)
    {
        key++;
        if((*it) != 0)
        {
            insertRows(0);
            setItem(0, 0, new JournalEditDate(this, (*it)->date));
            setItem(0, 1, new JournalEditAccount(this, accounts, (*it)->account, key));
            setItem(0, 2, new QTableItem(this, QTableItem::OnTyping, (*it)->reference));
            setItem(0, 3, new JournalEditDesc(this, (*it)->desc));
            
            if((*it)->amount > 0)
            {
                setItem(0, 4, new JournalEditCurrency(this, (*it)->amount));
                setItem(0, 5, new JournalEditCurrency(this, 0));
            }
            else if((*it)->amount < 0)
            {
                setItem(0, 4, new JournalEditCurrency(this, 0));
                setItem(0, 5, new JournalEditCurrency(this, abs((*it)->amount)));
            }
            else
            {
                setItem(0, 4, new JournalEditCurrency(this, 0));
                setItem(0, 5, new JournalEditCurrency(this, 0));
            }
        }
    }

    if(numRows())
        setCurrentCell(0,0);
}

void JournalTable::unpopulate()
{
    int rows = numRows();
    for(int row = 0; row <= rows; row++)
        removeRow(0);
}

void JournalTable::insert()
{
    inserting = true;
    editing = true;

    int newRow = numRows();
    insertRows(newRow);

    ensureCellVisible(newRow, 0);
    setItem(newRow, 0, new JournalEditDate(this, QDate()));
    setItem(newRow, 1, new JournalEditAccount(this, accounts, "",
                            journalData->getNextTmpRecordKey()));
    setItem(newRow, 2, new QTableItem(this, QTableItem::OnTyping));
    setItem(newRow, 3, new JournalEditDesc(this, ""));
    setItem(newRow, 4, new JournalEditCurrency(this, 0));
    setItem(newRow, 5, new JournalEditCurrency(this, 0));
    setCurrentCell(newRow,0);

    beginEdit(newRow, 0, false);
}

bool JournalTable::isInserting() const
{
    return inserting;
}

void JournalTable::updateAccounts()
{
    accounts = accountsData->getFormattedList();

    for(int i = 0; i < numRows(); i++)
        ((JournalEditAccount*)(item(i, 1)))->updateAccounts(accounts);
}

void JournalTable::remove(int row)
{
    Q_UINT16 key = ((JournalEditAccount*)(item(row, 1)))->getKey();
    
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

    QPushButton *buttonDelete = new QPushButton(QIconSet(
                             QPixmap::fromMimeSource(ICON_PATH + "/deleteButton.png")),
                             "Delete", dialog);
    connect(buttonDelete, SIGNAL(clicked()), dialog, SLOT(accept()));

    QPushButton *buttonDontDelete = new QPushButton(QIconSet(
                             QPixmap::fromMimeSource(ICON_PATH + "/okButton.png")),
                             "Don't Delete", dialog);
    connect(buttonDontDelete, SIGNAL(clicked()), dialog, SLOT(reject()));

    buttonDontDelete->setDefault(true);

    hBoxLayout->addStretch();
    hBoxLayout->addWidget(buttonDelete);
    hBoxLayout->addWidget(buttonDontDelete);

    if(dialog->exec())
    {
        journalData->deleteTmpRecord(key);
        removeRow(row);
    }
    if(row > 0)
        setCurrentCell(row - 1, 0);

    delete dialog;
}

void JournalTable::updateDb()
{
    int row = currentRow();

    Q_UINT16 key = ((JournalEditAccount*)(item(row, 1)))->getKey();
    QDate date = ((JournalEditDate*)(item(row, 0)))->date();
    QString account = text(row, 1);
    QString reference = text(row, 2);
    QString desc = text(row, 3);
    Q_INT64 amount = ((JournalEditCurrency*)(item(row, 4)))->value();
    amount -= ((JournalEditCurrency*)(item(row, 5)))->value();
    journalData->updateTmpRecord(key, date, account, reference, desc, amount);

    if(inserting && currentColumn() == 1 && text(row, 3) == "")
        setText(row, 3, accountsData->getDescription(account));
}

void JournalTable::keyPressEvent(QKeyEvent *event)
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
                if(currentColumn() == 4)
                {
                    endEdit(currentRow(), 4, true, true);
                    updateDb();

                    if(text(currentRow(), 5) == "" && text(currentRow(), 4) != "")
                    {
                        insert();
                        break;
                    }
                    else
                    {
                        setCurrentCell(currentRow(), 5);
                        beginEdit(currentRow(), 5, false);
                        break;
                    }
                }
                
                endEdit(currentRow(), currentColumn(), true, true);
                updateDb();

                if(currentColumn() == 5)
                {
                    insert();
                    break;
                }

                if(currentColumn() == 1)
                    setText(currentRow(), 3, accountsData->getDescription(text(currentRow(), 1)));

                int nextCol = (currentColumn() + 1);
                setCurrentCell(currentRow(), nextCol);
                beginEdit(currentRow(), nextCol, false);
                break;
            }
            else
            {
                if(editing)
                {
                    endEdit(currentRow(), currentColumn(), true, true);
                    editing = false;
                    updateDb();
                    break;
                }
                else
                {
                    editing = true;
                    beginEdit(currentRow(), currentColumn(), false);
                    break;
                }
            }
        case Key_Escape:
            if(inserting)
            {
                if(currentColumn() == 0)
                {
                    endEdit(currentRow(), 0, false, false);
                    removeRow(currentRow());
                    editing = false;
                    inserting = false;
                    setCurrentCell(numRows() - 1, 0);
                    break;
                }
                else
                {
                    endEdit(currentRow(), currentColumn(), false, false);
                    inserting = false;
                    editing = false;
                    updateDb();
                    break;
                }
            }
            if(editing)
            {
                endEdit(currentRow(), currentColumn(), false, false);
                editing = false;
                updateDb();
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

                if(curCol == 1)
                    break;

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
                QTable::keyPressEvent(event);
            }
            break;
        case Key_F1:
            emit switchToHome();
            break;
        case Key_F2:
            emit switchToAccounts();
            break;
        case Key_F3:
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

