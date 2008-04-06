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

#ifndef ACCOUNTS_EDIT_ID_H
#define ACCOUNTS_EDIT_ID_H

#include <qtable.h>

class QString;

class AccountsEditId : public QTableItem
{
    public:
        AccountsEditId(QTable *table, const QString newText, const Q_UINT16 newKey);
        QWidget* createEditor() const;
        void setContentFromEditor(QWidget *widget);
        QString text() const;
        Q_UINT16 getKey() const;

    private:
        QString itemText;
        Q_UINT16 itemKey;

};


#endif
