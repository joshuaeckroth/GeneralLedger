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

#ifndef CLIENT_H
#define CLIENT_H

#include <qobject.h>
#include <qdatetime.h>

class QString;
class QStringList;

class AccountsData;
class JournalData;
class ReportsEditor;
class Settings;

/// Client database interface
///
/// All interactions with client information take place here; the database
/// backend is encapsulated in this class, and can be changed without change
/// to the API.

class Client : public QObject
{
    Q_OBJECT
    public:
        /// Returns the single instance of this class
        /// 
        /// If the instance doesn't exist, this function creates it.
        /// The instance should not yet exist only when called from main.cpp.
        static Client* instance();

        /// Destroys the single instance
        ///
        /// This should only occur in main.cpp
        void destroy();

        /// Performs all constructor operations
        ///
        /// This is not done in the constructor because circular requests for instance pointers results
        void initiate();

        /// Create a client.
        ///
        /// The caller must provide the new client name and year-end closing month.
        void create(QString newClientName, int newYearEndMonth, QString REAccountId);

        /// Open a client.
        ///
        /// The id is obtainable from Client::defaultClientId()
        /// or Client::getAllClients().
        void open(QString id);

        /// Check if a client is open
        bool isOpen();

        /// Close the client.
        ///
        /// No id is necessary since only one client is open at a time.
        void close();

        /// Sets the client's name
        ///
        /// Changes the client's name in the database, and emits a
        /// clientNameChanged(QString) signal.
        void setClientName(QString newClientName);

        /// Gets the client's name
        QString getClientName() const;

        /// Returns a list of all clients in the clientPath
        ///
        /// An element from this list is suitable as an id for Client::open(id)
        QStringList getAllClients() const;

        /// Returns the id of the default client
        QString defaultClientId() const;

        /// Returns 'true' if the default client exists
        ///
        /// The default client may not exist if the clientPath was changed outside
        /// of the program or the file or database was removed manually.
        /// This function will need to be rewritten if one changes the backend database.
        bool defaultClientExists() const;

        QStringList getPeriodList(bool begin = true) const;
      
        /// Get the date of the end of a particular period (1-12)
        QDate getPeriodDate(int period, bool endOfMonth) const;

        int getCurrentPeriod() const;

    public slots:
        void saveToDisk();

    signals:

        /// A client was opened
        ///
        /// The QString contains the client's name
        void clientOpened(QString);

        /// The client was closed
        ///
        /// The QString contains the client's name
        void clientClosed(QString);

        /// The client's name was changed
        ///
        /// The QString contains the client's new name
        void clientNameChanged(QString);


    private:
        Client();
        ~Client();

        static Client *thisInstance;

        AccountsData *accountsData;
        JournalData *journalData;
        ReportsEditor *reportsEditor;

        Settings *settings;

        enum { MagicNumber = 0x4AB298BC };
        enum { Version = 0x00 };

        QString clientId;
        QString clientName;
        bool clientOpen;
        Q_UINT8 yearEndMonth;
        QDate creationDate;
        QDate modifiedDate;
};

#endif

