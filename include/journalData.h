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

#ifndef JOURNAL_DATA_H
#define JOURNAL_DATA_H

#include <vector>

#include <qobject.h>
#include <qdatetime.h>

class QIODevice;
class QString;
class QTextStream;

/// Interface to Journal Entry data
///
/// \note
/// In this class, "journalTmp" data is accessed, which is the
/// unsaved data in the Journal Entry tab.  The other data,
/// that of saved journal entries, is accessed only by ReportsGenerator,
/// except for the function in this class that saves journalTmp data
/// to the journal (JournalData::saveAndClear()).
class JournalData : public QObject
{
    Q_OBJECT
    public:
        /// Holds all data for one journal record
        struct JournalRecord {
            /// Record date
            QDate date;
            /// Record account
            QString account;
            /// Record reference
            QString reference;
            /// Record description
            QString desc;
            /// Record (-debit + credit) amount (in cents)
            Q_INT64 amount;
        };

        /// Sorting types for getJournalRecords()
        enum SortField {
            NoSort,
            SortDate
        };

        /// Retrieve the single instance of this class
        ///
        /// Client should start this instance, and use JournalData::destroy()
        /// inside its destructor.
        static JournalData* instance();

        /// Destroy the single instance of this class
        ///
        /// Should only be used by Client.
        void destroy();

        /// Initiate the class
        void initiate();

        void readStream(QIODevice *device, Q_UINT16 journalTmpCount,
                        Q_UINT32 journalCount);

        void writeStream(QIODevice *device);

        void clearData();

        /// Retrieve all data for all tmp records
        std::vector<JournalRecord*> getJournalTmpRecords() const;

        /// Retrieve all data for all journal records
        std::vector<JournalRecord*> getJournalRecords(
                        QString accountBegin = "", QString accountEnd = "",
                        QDate periodBegin = QDate(), QDate periodEnd = QDate(),
                        SortField = NoSort) const;

        /// Update a record
        ///
        /// This function updates an existing record
        /// or inserts a new record if the "key" is not
        /// found in the database.  Requires an identifying
        /// key, and the new data.
        void updateTmpRecord(
            const Q_UINT16 key, const QDate date, const QString account,
            const QString reference, const QString desc, const Q_INT64 amount);

        /// Delete a record
        ///
        /// Requires an identifying key
        void deleteTmpRecord(const Q_UINT16 key);

        /// Retrieve a new record key
        ///
        /// To create a new record, grab this key, then use it in an update.
        Q_UINT16 getNextTmpRecordKey() const;

        /// Save and Clear all records to the "journal" database
        void saveAndClear();

        /// Retrieve the total debit of the journal records
        ///
        /// \note This is mostly for JournalSummary
        QString journalTmpDebit() const;

        /// Retrieve the total credit of the journal records
        ///
        /// \note This is mostly for JournalSummary
        QString journalTmpCredit() const;

        /// Retrieve the total balance of the journal records
        ///
        /// \note This is mostly for JournalSummary
        QString journalTmpBalance() const;

        /// Check if the journalTmp table is empty
        bool journalTmpEmpty() const;

        QString convertCurrency(const Q_INT64 amount) const;

        bool accountHasEntries(const QString id) const;

        Q_UINT16 getJournalTmpCount() const;

        Q_UINT32 getJournalCount() const;

        void importCSV(QTextStream &in);

        void exportCSV(QTextStream &out);

    signals:
        /// Emitted when something in the journalTmp table has been changed
        void journalTmpChanged();

        void saveToDisk();

    private:
        JournalData();
        ~JournalData();

        static JournalData *thisInstance;

        std::vector<JournalRecord*> journalTmpVec;
        std::vector<JournalRecord*> journalVec;

};

#endif

