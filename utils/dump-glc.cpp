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
#include <iostream>
using std::cout;
using std::endl;

#include <qfile.h>
#include <qdatastream.h>
#include <qdatetime.h>
#include <qstring.h>

int main(int argc, char *argv[])
{
    QFile file(argv[1]);
    if(!file.open(IO_ReadOnly))
        return 0;

    QDataStream in(&file);

    Q_UINT32 MagicNumber;
    Q_UINT8 Version;
    QString clientName;
    Q_UINT8 yearEndMonth;
    QDate creationTime;
    QDate modifiedTime;
    Q_UINT16 accountCount;
    QString REAccountId;
    QString REAccountDesc;
    Q_UINT16 journalTmpCount;
    Q_UINT32 journalCount;
    Q_UINT16 editorCount;

    in >> MagicNumber
       >> Version
       >> clientName
       >> yearEndMonth
       >> creationTime
       >> modifiedTime
       >> accountCount
       >> REAccountId
       >> REAccountDesc
       >> journalTmpCount
       >> journalCount
       >> editorCount;
    
    cout << "Magic Number: " << MagicNumber << endl;
    cout << "Version: " << Version << endl;
    cout << "Client Name: " << clientName.ascii() << endl;
    cout << "Year End Month: " << yearEndMonth << endl;
    cout << "Creation Time: " << creationTime.toString().ascii() << endl;
    cout << "Modified Time: " << modifiedTime.toString().ascii() << endl;

    cout << "Account Count: " << accountCount << endl;
    cout << "Retained Earnings Account Id: " << REAccountId.ascii() << endl;
    cout << "Retained Earnings Account Desc: " << REAccountDesc.ascii() << endl;

    cout << "For each account:" << endl;

    QString accountId;
    QString accountDesc;

    for(Q_UINT16 i = 0; i < accountCount; i++)
    {
        in >> accountId
           >> accountDesc;

        cout << "\tId: " << accountId.ascii() << "\tDesc: "
             << accountDesc.ascii() << endl;
    }

    cout << "JournalTmp Count: " << journalTmpCount << endl;
    cout << "Journal Count: " << journalCount << endl;

    cout << "For each journalTmp:" << endl;

    QDate journalDate;
    QString journalAccount;
    QString journalReference;
    QString journalDesc;
    Q_INT64 journalAmount;

    for(Q_UINT16 i = 0; i < journalTmpCount; i++)
    {
        in >> journalDate
           >> journalAccount
           >> journalReference
           >> journalDesc
           >> journalAmount;

        cout << "\tDate: " << journalDate.toString().ascii()
             << "\tAccount: " << journalAccount.ascii()
             << "\tReference: " << journalReference.ascii()
             << "\tDesc: " << journalDesc.ascii()
             << "\tAmount: " << journalAmount << endl;
    }

    cout << "For each journal:" << endl;

    for(Q_UINT32 i = 0; i < journalCount; i++)
    {
        in >> journalDate
           >> journalAccount
           >> journalReference
           >> journalDesc
           >> journalAmount;

        cout << "\tDate: " << journalDate.toString().ascii()
             << "\tAccount: " << journalAccount.ascii()
             << "\tReference: " << journalReference.ascii()
             << "\tDesc: " << journalDesc.ascii()
             << "\tAmount: " << journalAmount << endl;
    }


}

