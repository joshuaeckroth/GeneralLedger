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

#include <cstdlib>
#include <cmath>

#include <qapplication.h>
#include <qstring.h>
#include <qfont.h>

#include "main.h"
#include "globalTabs.h"
#include "client.h"
#include "settings.h"
#include "accountsData.h"
#include "journalData.h"
#include "reportsEditor.h"

QString formatCurrency(Q_INT64 amount, bool commas)
{
    QString formatted = QString::number(std::abs(amount));
    if(formatted.length() == 1)
        formatted = "0.0" + formatted;
    else if(formatted.length() == 2)
        formatted = "0." + formatted;
    else
        formatted.insert(formatted.length() - 2, '.');

    if(commas)
    {
        std::div_t divResult = std::div(formatted.length() - 3, 3);
        int start = divResult.rem;
        int times = divResult.quot;

        for(int i = 0; i < times; i++)
        {
            if(start == 0)
                continue;

            formatted.insert(start + i, ',');

            start += 3;
        }
    }

    if(amount < 0)
    {
        formatted = "(" + formatted + ")";
    }
    if(amount >= 0 && commas)
    {
        formatted = formatted + "&nbsp;";
    }

    return formatted;
}

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);

    app.setFont(QFont("Verdana", 9));
    
    Settings *settings = Settings::instance();
    Client *client = Client::instance();
    AccountsData *accountsData = AccountsData::instance();
    JournalData *journalData = JournalData::instance();
    ReportsEditor *reportsEditor = ReportsEditor::instance();

    client->initiate();
    accountsData->initiate();
    journalData->initiate();
    reportsEditor->initiate();

    GlobalTabs globalTabs("globalTabs");
    globalTabs.show();
    app.setMainWidget(&globalTabs);
    int retVal = app.exec();

    accountsData->destroy();
    journalData->destroy();
    reportsEditor->destroy();
    
    client->destroy();
    settings->destroy();

    return retVal;
}

