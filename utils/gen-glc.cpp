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
#include <cstdlib>
#include <iostream>
using namespace std;

#include <qfile.h>
#include <qdatastream.h>
#include <qdatetime.h>
#include <qstring.h>

#define ACCOUNT_COUNT 200
#define JOURNAL_TMP_COUNT 50
#define JOURNAL_COUNT 10000
#define BALANCE_SHEET_EDITOR_COUNT 10
#define INCOME_SVC_EDITOR_COUNT 10
#define INCOME_RETAIL_EDITOR_COUNT 10

int main(int argc, char *argv[])
{
    QFile file(argv[1]);

    if(!file.open(IO_WriteOnly))
        return 0;

    QDataStream out(&file);

    out << (Q_UINT32)0x4AB298BC
        << (Q_UINT8)0x00
        << QString("Stress-Test Client")
        << (Q_UINT8)0
        << QDate::currentDate()
        << QDate::currentDate()
        << (Q_UINT16)ACCOUNT_COUNT
        << QString("")
        << (Q_UINT16)JOURNAL_TMP_COUNT
        << (Q_UINT32)JOURNAL_COUNT
        << (Q_UINT16)BALANCE_SHEET_EDITOR_COUNT
        << (Q_UINT16)INCOME_SVC_EDITOR_COUNT
        << (Q_UINT16)INCOME_RETAIL_EDITOR_COUNT;

    QString str;

    cout << "Creating " << ACCOUNT_COUNT << " accounts..." << endl;

    for(Q_UINT16 i = 0; i < ACCOUNT_COUNT; i++)
    {
        out << QString::number(int(10000 * (((float)rand())/(RAND_MAX + 1.0f))))
            << str.fill('x', 250);
    }

    cout << "Creating " << JOURNAL_TMP_COUNT << " journalTmp records and "
         << JOURNAL_COUNT << " journal records..." << endl;

    for(Q_UINT32 i = 0; i < (JOURNAL_TMP_COUNT + JOURNAL_COUNT); i++)
    {
        out << QDate::currentDate()
            << str.fill('1', 50)
            << str.fill('x', 50)
            << str.fill('x', 250)
            << (Q_INT64)int(1000000 * (((float)rand())/(RAND_MAX + 1.0f)));
    }

    cout << "Creating " << BALANCE_SHEET_EDITOR_COUNT << " balanceSheetEditors, "
         << INCOME_SVC_EDITOR_COUNT << " incomeSvcEditors, and "
         << INCOME_RETAIL_EDITOR_COUNT << " incomeRetailEditors..." << endl;

    for(Q_UINT32 i = 0; i < (BALANCE_SHEET_EDITOR_COUNT
                             + INCOME_SVC_EDITOR_COUNT
                             + INCOME_RETAIL_EDITOR_COUNT); i++)
    {
        out << (Q_UINT8)1
            << (Q_UINT16)100
            << str.fill('x', 50)
            << str.fill('x', 50)
            << str.fill('x', 250);
    }

    cout << "Done!" << endl;

    file.close();

    return 1;

}

