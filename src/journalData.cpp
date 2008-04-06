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

#include <qstring.h>
#include <qiodevice.h>
#include <qdatastream.h>
#include <qdatetime.h>
#include <qtextstream.h>
#include <qregexp.h>

#include "journalData.h"
#include "main.h"

JournalData *JournalData::thisInstance = 0;

JournalData* JournalData::instance()
{
    if(thisInstance == 0)
        thisInstance = new JournalData;
    return thisInstance;
}

void JournalData::destroy()
{
    if(thisInstance != 0)
        delete thisInstance;
}

void JournalData::initiate()
{}

void JournalData::readStream(QIODevice *device, Q_UINT16 journalTmpCount,
                Q_UINT32 journalCount)
{
    QDataStream in(device);

    Q_UINT16 i = 0;
    while(i++ != journalTmpCount)
    {
        JournalRecord *journalTmpRecord = new JournalRecord;
        in >> journalTmpRecord->date
           >> journalTmpRecord->account
           >> journalTmpRecord->reference
           >> journalTmpRecord->desc
           >> journalTmpRecord->amount;
        journalTmpVec.push_back(journalTmpRecord);
    }

    Q_UINT32 j = 0;
    while(j++ != journalCount)
    {
        JournalRecord *journalRecord = new JournalRecord;
        in >> journalRecord->date
           >> journalRecord->account
           >> journalRecord->reference
           >> journalRecord->desc
           >> journalRecord->amount;
        journalVec.push_back(journalRecord);
    }
}

void JournalData::writeStream(QIODevice *device)
{
    QDataStream out(device);

    for(vector<JournalRecord*>::const_iterator it = journalTmpVec.begin();
                    it != journalTmpVec.end(); ++it)
    {
        if((*it) != 0)
        {
            out << (*it)->date
                << (*it)->account
                << (*it)->reference
                << (*it)->desc
                << (*it)->amount;
        }
    }

    for(vector<JournalRecord*>::const_iterator it = journalVec.begin();
                    it != journalVec.end(); ++it)
    {
        if((*it) != 0)
        {
            out << (*it)->date
                << (*it)->account
                << (*it)->reference
                << (*it)->desc
                << (*it)->amount;
        }
    }
}

void JournalData::clearData()
{
    for(vector<JournalRecord*>::iterator it = journalTmpVec.begin();
        it != journalTmpVec.end(); ++it)
    {
        if((*it) != 0)
            delete (*it);
    }

    journalTmpVec.clear();

    for(vector<JournalRecord*>::iterator it = journalVec.begin();
        it != journalVec.end(); ++it)
    {
        if((*it) != 0)
            delete (*it);
    }

    journalVec.clear();
}

Q_UINT16 JournalData::getJournalTmpCount() const
{
    Q_UINT16 count = 0;

    for(vector<JournalRecord*>::const_iterator it = journalTmpVec.begin();
                    it != journalTmpVec.end(); ++it)
    {
        if((*it) != 0)
            count++;
    }

    return count;
}

Q_UINT32 JournalData::getJournalCount() const
{
    Q_UINT32 count = 0;

    for(vector<JournalRecord*>::const_iterator it = journalVec.begin();
                    it != journalVec.end(); ++it)
    {
        if((*it) != 0)
            count++;
    }

    return count;
}

vector<JournalData::JournalRecord*> JournalData::getJournalTmpRecords() const
{
    return journalTmpVec;
}

vector<JournalData::JournalRecord*> JournalData::getJournalRecords(
                QString accountBegin, QString accountEnd,
                QDate periodBegin, QDate periodEnd, SortField sort) const
{
    vector<JournalData::JournalRecord*> journal;

    for(vector<JournalData::JournalRecord*>::const_iterator it = journalVec.begin();
                    it != journalVec.end(); ++it)
    {
        if(accountBegin != "" && QString::localeAwareCompare(
            (*it)->account, accountBegin) < 0)
            continue;
        if(accountEnd != "" && QString::localeAwareCompare(
            (*it)->account, accountEnd) > 0)
            continue;
        if(periodBegin.isValid() && (*it)->date < periodBegin)
            continue;
        if(periodEnd.isValid() && (*it)->date > periodEnd)
            continue;
        
        if(sort == SortDate)
        {
            bool inserted = false;

            for(vector<JournalData::JournalRecord*>::iterator innerIt = journal.begin();
                            innerIt != journal.end(); ++innerIt)
            {
                if((*it)->date < (*innerIt)->date)
                {
                    journal.insert(innerIt, *it);
                    
                    inserted = true;
                    break;
                }
            }

            if(!inserted)
                journal.push_back(*it);
        }
        if(sort == NoSort)
            journal.push_back(*it);
    }

    return journal;
}


void JournalData::updateTmpRecord(const Q_UINT16 key, const QDate date,
                const QString account, const QString reference,
                const QString desc, const Q_INT64 amount)
{
    if((Q_UINT16)journalTmpVec.size() < key)  // add a new entry
    {
        JournalRecord *journalTmpRecord = new JournalRecord;
        journalTmpRecord->date = date;
        journalTmpRecord->account = account;
        journalTmpRecord->reference = reference;
        journalTmpRecord->desc = desc;
        journalTmpRecord->amount = amount;
        journalTmpVec.push_back(journalTmpRecord);
    }
    else  // update existing entry
    {
        JournalRecord *journalTmpRecord = journalTmpVec[key-1];
        journalTmpRecord->date = date;
        journalTmpRecord->account = account;
        journalTmpRecord->reference = reference;
        journalTmpRecord->desc = desc;
        journalTmpRecord->amount = amount;
    }

    emit saveToDisk();

    emit journalTmpChanged();
}

void JournalData::deleteTmpRecord(const Q_UINT16 key)
{
    delete journalTmpVec[key-1];
    journalTmpVec[key-1] = 0;

    emit saveToDisk();

    emit journalTmpChanged();
}

Q_UINT16 JournalData::getNextTmpRecordKey() const
{
    return (journalTmpVec.size() + 1);
}

void JournalData::saveAndClear()
{
    for(vector<JournalRecord*>::const_iterator it = journalTmpVec.begin();
                    it != journalTmpVec.end(); ++it)
    {
        if((*it) != 0)
            journalVec.push_back(*it);
    }

    journalTmpVec.clear();

    emit saveToDisk();

    emit journalTmpChanged();
}

QString JournalData::convertCurrency(const Q_INT64 amount) const
{
    bool neg = false;
    if(amount < 0)
	neg = true;

    QString sumString = QString::number(std::abs(amount));
    int length = sumString.length();

    if(length == 1)
        sumString = sumString.insert(0, "0.0");
    else if(length == 2)
        sumString = sumString.insert(0, "0.");
    else
        sumString = sumString.insert(length - 2, '.');

    if(neg)
	sumString = sumString.insert(0, "-");

    return sumString;
}

QString JournalData::journalTmpDebit() const
{
    Q_INT64 sum = 0;
    for(vector<JournalRecord*>::const_iterator it = journalTmpVec.begin();
                    it != journalTmpVec.end(); ++it)
    {
        if((*it) != 0)
        {
            if((*it)->amount > 0)
                sum += (*it)->amount;
        }
    }
    
    return convertCurrency(sum);
}

QString JournalData::journalTmpCredit() const
{
    Q_INT64 sum = 0;
    for(vector<JournalRecord*>::const_iterator it = journalTmpVec.begin();
                    it != journalTmpVec.end(); ++it)
    {
        if((*it) != 0)
        {
            if((*it)->amount < 0)
                sum += abs((*it)->amount);
        }
    }

    return convertCurrency(sum);
}

QString JournalData::journalTmpBalance() const
{
    Q_INT64 sum = 0;
    for(vector<JournalRecord*>::const_iterator it = journalTmpVec.begin(); it != journalTmpVec.end(); ++it)
    {
        if((*it) != 0)
            sum += (*it)->amount;
    }

    return convertCurrency(sum);
}

bool JournalData::journalTmpEmpty() const
{
    bool empty = true;
    for(vector<JournalRecord*>::const_iterator it = journalTmpVec.begin(); it != journalTmpVec.end(); ++it)
    {
        if((*it) != 0)
        {
            empty = false;
            break;
        }
    }

    return empty;
}

bool JournalData::accountHasEntries(const QString id) const
{
    // decision: only check journal records, not journalTmp

    bool hasEntries = false;
    for(vector<JournalRecord*>::const_iterator it = journalVec.begin(); it != journalVec.end(); ++it)
    {
        if((*it) != 0)
        {
            if((*it)->account == id)
            {
                hasEntries = true;
                break;
            }
        }
    }

    return hasEntries;
}

void JournalData::importCSV(QTextStream &in)
{
    QString line;
    QString dateString;
    QString account;
    QString reference;
    QString desc;
    QString debit;
    QString credit;

    QRegExp regExp("^\"?(\\d\\d\\d\\d-\\d\\d-\\d\\d)\"?,"
                   "\"?(?\\d+-?\\d*)\"?,\"?(.*)\"?,\"?(.*)\"?,"
                   "\"?(\\(?-?\\d*\\.?\\d?\\d?\\)?)\"?,"
                   "\"?(\\(?-?\\d*\\.?\\d?\\d?\\)?)\"?");

    while(line = in.readLine())
    {
        if(regExp.search(line) == -1)
            continue;

        dateString = regExp.cap(1);
        account = regExp.cap(2);
        reference = regExp.cap(3);
        desc = regExp.cap(4);
        debit = regExp.cap(5);
        credit = regExp.cap(6);

        QDate date = QDate::fromString(dateString, Qt::ISODate);

        Q_INT64 amount = 0;

        QRegExp creditRegExp("^\\(?-?(\\d*)\\.?(\\d?\\d?)\\)?$");
        creditRegExp.search(credit);

        amount = 0 - (creditRegExp.cap(1) + creditRegExp.cap(2)).toULongLong();

        QRegExp debitRegExp("^(\\d+)\\.?(\\d?\\d?)$");
        debitRegExp.search(debit);

        amount += (debitRegExp.cap(1) + debitRegExp.cap(2)).toULongLong();

 
        JournalRecord *journalRecord = new JournalRecord;

        journalRecord->date = date;
        journalRecord->account = account;
        journalRecord->reference = reference;
        journalRecord->desc = desc;
        journalRecord->amount = amount;

        journalTmpVec.push_back(journalRecord);
    }

    emit saveToDisk();

    emit journalTmpChanged();
}

void JournalData::exportCSV(QTextStream &out)
{
    out << "\"Date\",\"Account\",\"Reference\","
           "\"Description\",\"Debit\",\"Credit\"\n";

    for(vector<JournalRecord*>::const_iterator it = journalTmpVec.begin();
        it != journalTmpVec.end(); ++it)
    {
        out << "\"" << (*it)->date.toString(Qt::ISODate) << "\",\""
            << (*it)->account << "\",\"" << (*it)->reference << "\",\""
            << (*it)->desc << "\",";

        if((*it)->amount < 0)
        {
            out << "0.00,"
                << formatCurrency((*it)->amount, false);
        }
        else if((*it)->amount > 0)
        {
            out << formatCurrency((*it)->amount, false)
                << ",0.00";
        }
        else
        {
            out << "0.00,0.00";
        }
        out << "\n";
    }
}

// Private

JournalData::JournalData() : QObject(0, "JournalData")
{}

JournalData::~JournalData()
{}


