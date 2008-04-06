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

#ifndef ACCOUNTS_DATA_H
#define ACCOUNTS_DATA_H

#include <vector>

#include <qobject.h>
#include <qptrlist.h>

class QIODevice;
class QString;
class QStringList;
class QTextStream;

class JournalData;

/// Interface to all Accounts data
class AccountsData : public QObject
{
    Q_OBJECT
    public:
        /// Holds all data for one account
        struct Account {
            /// Account id
            QString id;
            /// Account description
            QString desc;
        };

        /// Retrieve the single instance of this class
        ///
        /// Client should start this instance, and use AccountsData::destroy()
        /// inside its destructor.
        static AccountsData* instance();

        /// Destroy the single instance of this class
        ///
        /// Should only be used by Client.
        void destroy();

        /// Initiate the class
        void initiate();

        /// Reads data from client file
        ///
        /// Called by Client when a client is opened
        void readStream(QIODevice *device, Q_UINT16 accountCount,
                        QString newREAccountId, QString newREAccountDesc);

        /// Write data to client file
        ///
        /// Called by Client when a client is closed,
        /// and after changes (periodic saving)
        void writeStream(QIODevice *device);

        /// Deletes all data from memory
        ///
        /// Called by Client when a client is closed, after
        /// AccountsData::writeStream()
        void clearData();
 
        /// Retrieve all data for all accounts
        std::vector<AccountsData::Account*> getAccounts(
                        QString accountBegin = "", QString accountEnd = "",
                        bool sorted = false, bool addREAccount = false) const;

        /// Update an account
        ///
        /// This function updates an existing account or inserts a new account
        /// if the "key" is not found in the account database.
        /// Requires an identifying key, and the new data.
        /// \note
        /// This update does not work on the retained earnings account.
        /// Use AccountsData::updateREAccount() instead.
        void updateAccount(const Q_UINT16 key, const QString id,
                        const QString desc);

        /// Delete an account
        ///
        /// Requires an identifying key
        /// \note
        /// If the account contains associated data in the journal
        /// or journal entry it will not be deleted.  Use
        /// AccountsData::accountHasEntries() to check
        /// for this condition (and tell the user).
        void deleteAccount(const Q_UINT16 key);

        /// Retrieve a new account key
        ///
        /// To create a new account, grab this key, then use it in an update.
        Q_UINT16 getNextAccountKey() const;

        /// Sets the Retained Earnings account id
        ///
        /// Called by Client when a new client is created
        void setREAccountId(const QString newREAccountId);

        /// Changes the Retained Earnings account description
        ///
        /// The emitSave property is only false when called by Client
        /// when a client is created; the other caller, AccountsTable,
        /// should keep emitSave true.
        void setREAccountDesc(const QString newREAccountDesc,
                              bool emitSave = true);

        /// Returns the Retained Earnings account id
        QString getREAccountId() const;

        /// Returns the Retained Earnings account description
        QString getREAccountDesc() const;

        /// Retrieve just a list of accounts
        ///
        /// The strings are in the format: "id - desc"
        QStringList getFormattedList(bool withREAccount = false) const;

        /// Retrieve just the description for a particluar account
        QString getDescription(const QString id) const;

        /// Determine if an account has data associated with it in
        /// the journal (not the unsaved journal entries)
        bool accountHasEntries(const QString id) const;

        /// Determine if a string is already an account id
        bool accountIdExists(const QString id) const;

        /// Returns the current number of accounts
        Q_UINT16 getAccountCount() const;

        /// Imports account information from CSV data
        ///
        /// This import only takes unique/new data.
        void importCSV(QTextStream &in);

        /// Export all account information in CSV format
        void exportCSV(QTextStream &out);

    signals:
        /// Emitted when the "accounts" table has changed
        void accountsChanged();

        /// Emitted when account data changes
        void saveToDisk();

    private:
        AccountsData();
        ~AccountsData();

        static AccountsData *thisInstance;
        JournalData *journalData;

        std::vector<AccountsData::Account*> accountsVec;

        Account* REAccount;

};

#endif


