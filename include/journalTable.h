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

#ifndef JOURNAL_TABLE_H
#define JOURNAL_TABLE_H

#include <qtable.h>

class QStringList;
class QKeyEvent;

class JournalData;
class AccountsData;
class Client;

class JournalTable : public QTable
{
    Q_OBJECT
    public:
        JournalTable(QWidget *parent, const char *name);
        bool isInserting() const;
        int debitColWidth();
        int creditColWidth();
        void clearTable();
        
    signals:
        void switchToHome();
        void switchToAccounts();
        void switchToReports();
        void switchToHelp();

    public slots:
        void populate();
        void unpopulate();
    
    private slots:
        void updateAccounts();
        
    private:
        void updateDb();
        void insert();
        void remove(int row);
        void keyPressEvent(QKeyEvent *event);
        
        JournalData *journalData;
        AccountsData *accountsData;
        Client *client;

        bool editing;
        bool inserting;
        
        QStringList accounts;
};

#endif
