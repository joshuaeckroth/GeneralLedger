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

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qiodevice.h>
#include <qdatastream.h>
#include <qtextstream.h>
#include <qregexp.h>

#include "accountsData.h"
#include "journalData.h"

AccountsData *AccountsData::thisInstance = 0;

AccountsData* AccountsData::instance()
{
    if(thisInstance == 0)
        thisInstance = new AccountsData;
    return thisInstance;
}

void AccountsData::destroy()
{
    if(thisInstance != 0)
        delete thisInstance;
}

void AccountsData::initiate()
{
    journalData = JournalData::instance();
}

void AccountsData::readStream(QIODevice *device, Q_UINT16 accountCount,
                QString newREAccountId, QString newREAccountDesc)
{
    QDataStream in(device);

    REAccount->id = newREAccountId;
    REAccount->desc = newREAccountDesc;

    Q_UINT16 i = 0;
    while(i++ != accountCount)
    {
        Account *account = new Account;
        in >> account->id
           >> account->desc;
        accountsVec.push_back(account);
    }
}

void AccountsData::writeStream(QIODevice *device)
{
    QDataStream out(device);

    for(vector<Account*>::const_iterator it = accountsVec.begin();
                    it != accountsVec.end(); ++it)
    {
        if((*it) != 0)
        {
            out << (*it)->id
                << (*it)->desc;
        }
    }
}

void AccountsData::clearData()
{
    for(vector<Account*>::iterator it = accountsVec.begin();
        it != accountsVec.end(); ++it)
    {
        if((*it) != 0)
            delete (*it);
    }

    accountsVec.clear();
}

Q_UINT16 AccountsData::getAccountCount() const
{
    Q_UINT16 count = 0;
    for(vector<Account*>::const_iterator it = accountsVec.begin();
                    it != accountsVec.end(); ++it)
    {
        if((*it) != 0)
            count++;
    }

    return count;
}

vector<AccountsData::Account*> AccountsData::getAccounts(
                QString accountBegin, QString accountEnd,
                bool sorted, bool addREAccount) const
{
    // Prepare our new container
    vector<AccountsData::Account*> accounts;

    // Run through a very inefficient sorting process if requested
    if(sorted)
    {
        // Add the retained earnings account if needed, and in our account range
        if(addREAccount && ((accountBegin == "" && accountEnd == "") ||
                            ((QString::localeAwareCompare(REAccount->id,
                                             accountBegin) >= 0)
                             && (QString::localeAwareCompare(REAccount->id,
                                             accountEnd) <= 0))))
            accounts.push_back(REAccount);

        // Look at every account
        for(vector<AccountsData::Account*>::const_iterator it =
            accountsVec.begin(); it != accountsVec.end(); ++it)
        {
            // Go to the next account if the current one is not
            // in the accountBegin and accountEnd range; note that all
            // the following tests are in the form of negations
            if(accountBegin != "" && accountEnd != ""
               && ((QString::localeAwareCompare((*it)->id, accountBegin) < 0)
                   || (QString::localeAwareCompare((*it)->id, accountEnd) > 0)))
                continue;

            // Keep track of whether the account has been inserted
            bool inserted = false;

            // Iterate over the whole accounts vector
            for(vector<AccountsData::Account*>::iterator innerIt =
                accounts.begin(); innerIt != accounts.end(); ++innerIt)
            {
                // If the account id in the old vector is less than
                // the account id in the new (sorted) vector, insert it
                // at the present position
                if(QString::localeAwareCompare((*it)->id, (*innerIt)->id) < 0)
                {
                    accounts.insert(innerIt, *it);

                    // We've inserted it, so change the flag and quit
                    inserted = true;
                    break;
                }
            }
            // If it was never inserted (the id is greater than
            // all the id's in the sorted accounts vector)
            if(!inserted)
                accounts.push_back(*it);
        }
    }
    // If we don't need to sort, just throw back the old accounts vector
    else
    {
        accounts = accountsVec;

        // Add the retained earnings account if needed
        if(addREAccount)
            accounts.push_back(REAccount);
    }

    return accounts;
}

bool AccountsData::accountHasEntries(const QString id) const
{
    return journalData->accountHasEntries(id);
}

bool AccountsData::accountIdExists(const QString id) const
{
    if(id == REAccount->id)
        return true;

    for(vector<Account*>::const_iterator it = accountsVec.begin();
                    it != accountsVec.end(); ++it)
    {
        if((*it) != 0)
        {
            if((*it)->id == id)
                return true;
        }
    }

    return false;
}

void AccountsData::updateAccount(Q_UINT16 key, const QString id,
                const QString desc)
{
    if((Q_UINT16)accountsVec.size() < key)  // add a new entry
    {
        Account *account = new Account;
        account->id = id;
        account->desc = desc;
        accountsVec.push_back(account);
    }
    else  // update existing entry
    {
        Account *account = accountsVec[key-1];
        account->id = id;
        account->desc = desc;
    }

    emit saveToDisk();

    emit accountsChanged();
}

void AccountsData::deleteAccount(const Q_UINT16 key)
{
    delete accountsVec[key-1];
    accountsVec[key-1] = 0;

    emit saveToDisk();

    emit accountsChanged();
}

Q_UINT16 AccountsData::getNextAccountKey() const
{
    return (accountsVec.size() + 1);
}

void AccountsData::setREAccountId(const QString newREAccountId)
{
    REAccount->id = newREAccountId;
}

void AccountsData::setREAccountDesc(const QString newREAccountDesc,
                                    bool emitSave)
{
    REAccount->desc = newREAccountDesc;

    if(emitSave)
        emit saveToDisk();

    emit accountsChanged();
}

QString AccountsData::getREAccountId() const
{
    return REAccount->id;
}

QString AccountsData::getREAccountDesc() const
{
    return REAccount->desc;
}

QStringList AccountsData::getFormattedList(bool withREAccount) const
{
    QStringList formattedList;

    for(vector<Account*>::const_iterator it = accountsVec.begin();
                    it != accountsVec.end(); ++it)
    {
        if((*it) != 0)
            formattedList << ((*it)->id + " - " + (*it)->desc);
    }

    if(withREAccount)
    {
        formattedList << (REAccount->id + " - " + REAccount->desc);
    }

    formattedList.sort();

    return formattedList;
}

QString AccountsData::getDescription(const QString id) const
{
    if(id == REAccount->id)
        return REAccount->desc;

    for(vector<Account*>::const_iterator it =
        accountsVec.begin(); it != accountsVec.end(); ++it)
    {
        if((*it) != 0)
        {
            if((*it)->id == id)
                return (*it)->desc;
        }
    }

    return QString::null;
}

void AccountsData::importCSV(QTextStream &in)
{
    QString line;
    QString id;
    QString desc;

    QRegExp regExp("^\\d+-?\\d*,.*");

    while(line = in.readLine())
    {
        if(!regExp.exactMatch(line))
            continue;

        id = line.left(line.find(","));
        if(id.left(1) == "\"" && id.right(1) == "\"")
            id = id.mid(1, id.length() - 2);

        desc = line.mid(line.find(",") + 1, line.length() - 1);
        if(desc.left(1) == "\"" && desc.right(1) == "\"")
            desc = desc.mid(1, desc.length() - 2);

        if(!accountIdExists(id))
        {
            Account *account = new Account;
            account->id = id;
            account->desc = desc;

            accountsVec.push_back(account);
        }
    }

    emit saveToDisk();

    emit accountsChanged();
}

void AccountsData::exportCSV(QTextStream &out)
{
    out << "\"Account ID\",\"Account Description\"\n";

    vector<Account*> accountsSorted = getAccounts("", "", true, true);

    for(vector<Account*>::const_iterator it = accountsSorted.begin();
        it != accountsSorted.end(); ++it)
    {
        out << "\"" << (*it)->id << "\",\"" << (*it)->desc << "\"\n";
    }
}

// Private

AccountsData::AccountsData() : QObject(0, "AccountsData")
{
    REAccount = new Account;
    REAccount->id = "";
    REAccount->desc = "";
}

AccountsData::~AccountsData()
{
    delete REAccount;
}

