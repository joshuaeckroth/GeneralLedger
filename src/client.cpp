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

#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qdir.h>

#include "client.h"
#include "accountsData.h"
#include "journalData.h"
#include "reportsEditor.h"
#include "settings.h"


Client *Client::thisInstance = 0;

Client* Client::instance()
{
    if(thisInstance == 0)
        thisInstance = new Client;
    return thisInstance;
}


void Client::destroy()
{
    if(thisInstance != 0)
        delete thisInstance;
}

void Client::initiate()
{
    accountsData = AccountsData::instance();
    journalData = JournalData::instance();
    reportsEditor = ReportsEditor::instance();

    connect(accountsData, SIGNAL(saveToDisk()), this, SLOT(saveToDisk()));
    connect(journalData, SIGNAL(saveToDisk()), this, SLOT(saveToDisk()));
    connect(reportsEditor, SIGNAL(saveToDisk()), this, SLOT(saveToDisk()));

    settings = Settings::instance();

    clientId = settings->getDefaultClientId();

    if(defaultClientExists())
        clientName = settings->getDefaultClientName();
    else
    {
        clientId = "";
        clientName = "";
    }

    clientOpen = false;
}

void Client::create(QString newClientName, int newYearEndMonth,
                QString REAccountId)
{
    QString filename(newClientName);
    filename.replace(QChar(' '), "_");
    filename.replace(QChar('/'), "-");
    filename.replace(QChar('\\'), "-");

    if(QFile::exists(settings->getClientPath() + "/" + filename + ".glc"))
    {
        int index = 0;
        while(QFile::exists(settings->getClientPath() + "/" + filename +
            QString::number(++index) + ".glc"));

        filename.append(QString::number(index));
    }

    clientId = filename;
    clientName = newClientName;
    yearEndMonth = newYearEndMonth;

    QString REAccountDesc = "Retained Earnings";

    QFile file(settings->getClientPath() + "/" + filename + ".glc");
    /// \todo throw exception if returns false
    if(!file.open(IO_WriteOnly))
    {
        return;
    }

    QDataStream out(&file);
    out.setVersion(5);

    out << (Q_UINT32)MagicNumber
        << (Q_UINT8)Version
        << clientName
        << yearEndMonth
        << QDate::currentDate(Qt::UTC)
        << QDate::currentDate(Qt::UTC)
        << (Q_UINT16)0
        << REAccountId
        << REAccountDesc
        << (Q_UINT16)0
        << (Q_UINT32)0
        << (Q_UINT16)0;

    out.unsetDevice();

    file.close();

    QFile lockFile(settings->getClientPath() + "/" + filename + ".lck");
    if(!lockFile.open(IO_WriteOnly))
        return;

    QTextStream lockOut(&lockFile);
    lockOut << "\n";
    lockOut.unsetDevice();
    lockFile.close();

    accountsData->setREAccountId(REAccountId);
    accountsData->setREAccountDesc(REAccountDesc, false);

    emit clientOpened(clientName);
}

/// \todo Add encryption
void Client::open(QString id)
{
    if(QFile::exists(settings->getClientPath() + "/" + id + ".lck"))
    {
        qDebug("locked file!");
        return;
    }

    QFile file(settings->getClientPath() + "/" + id + ".glc");
    /// \todo throw exception if failure
    if(!file.open(IO_ReadOnly))
    {
        return;
    }

    QDataStream in(&file);
    in.setVersion(5);

    Q_UINT32 magic;
    in >> magic;
    Q_UINT8 version;
    in >> version;
    /// \todo dialog box message for error
    if(magic != (Q_UINT32)MagicNumber || version != (Q_UINT8)Version)
    {
        return;
    }

    clientId = id;

    Q_UINT16 accountCount;
    QString REAccountId;
    QString REAccountDesc;
    Q_UINT16 journalTmpCount;
    Q_UINT32 journalCount;
    Q_UINT16 editorEntityCount;

    in >> clientName
       >> yearEndMonth
       >> creationDate
       >> modifiedDate
       >> accountCount
       >> REAccountId
       >> REAccountDesc
       >> journalTmpCount
       >> journalCount
       >> editorEntityCount;

    in.unsetDevice();

    accountsData->readStream(&file, accountCount, REAccountId, REAccountDesc);
    journalData->readStream(&file, journalTmpCount, journalCount);
    reportsEditor->readStream(&file, editorEntityCount);

    file.close();

    QFile lockFile(settings->getClientPath() + "/" + id + ".lck");
    if(!lockFile.open(IO_WriteOnly))
        return;

    QTextStream lockOut(&lockFile);
    lockOut << "\n";
    lockOut.unsetDevice();
    lockFile.close();

    clientOpen = true;

    emit clientOpened(clientName);
}

bool Client::isOpen()
{
    return clientOpen;
}

void Client::close()
{
    if(!clientOpen)
        return;

    clientOpen = false;

    saveToDisk();

    accountsData->clearData();
    journalData->clearData();
    reportsEditor->clearData();

    QFile::remove(settings->getClientPath() + "/" + clientId + ".lck");

    emit clientClosed(clientName);
}

void Client::saveToDisk()
{
    QFile file(settings->getClientPath() + "/" + clientId + ".glc");
    /// \todo fix error handling (dialog)
    if(!file.open(IO_WriteOnly))
        return;

    Q_UINT16 accountCount = accountsData->getAccountCount();
    QString REAccountId = accountsData->getREAccountId();
    QString REAccountDesc = accountsData->getREAccountDesc();
    Q_UINT16 journalTmpCount = journalData->getJournalTmpCount();
    Q_UINT32 journalCount = journalData->getJournalCount();
    Q_UINT16 editorEntityCount = reportsEditor->getEditorEntityCount();

    QDataStream out(&file);
    out << (Q_UINT32)MagicNumber
        << (Q_UINT8)Version
        << clientName
        << yearEndMonth
        << creationDate
        << QDate::currentDate(Qt::UTC)
        << accountCount
        << REAccountId
        << REAccountDesc
        << journalTmpCount
        << journalCount
        << editorEntityCount;

    out.unsetDevice();

    accountsData->writeStream(&file);
    journalData->writeStream(&file);
    reportsEditor->writeStream(&file);

    file.close();
}

void Client::setClientName(QString newClientName)
{
    clientName = newClientName;

    emit clientNameChanged(clientName);
}

QString Client::getClientName() const
{
    return clientName;
}

QStringList Client::getAllClients() const
{
    QStringList list;

    QDir dir(settings->getClientPath());
    QStringList::Iterator it;
    QStringList files = dir.entryList("*.glc", QDir::Files);
    it = files.begin();
    while(it != files.end())
    {
        QFile file(settings->getClientPath() + "/" + *it);
        if(file.open(IO_ReadOnly))
        {
            QDataStream in(&file);
            Q_UINT32 magic;
            in >> magic;
            Q_UINT8 version;
            in >> version;
            if(magic == (Q_UINT32)MagicNumber && version == (Q_UINT8)Version)
            {
                QString name;
                in >> name;
                list << (name + "   (" + *it + ")");
            }
            file.close();
        }
        ++it;
    }

    return list;
}

QString Client::defaultClientId() const
{
    return clientId;
}

bool Client::defaultClientExists() const
{
    QFile file(settings->getClientPath() + "/" + clientId);
    if(file.exists())
    {
        if(file.open(IO_ReadOnly))
        {
            QDataStream in(&file);
            Q_UINT32 magic;
            in >> magic;
            if(magic == (Q_UINT32)MagicNumber)
                return true;
            file.close();
        }
    }

    return false;
}

QStringList Client::getPeriodList(bool begin) const
{
    QStringList periods;

    QDate date;
    int curYear = QDate::currentDate().year();
    int curMonth = QDate::currentDate().month();

    for(int i = 1; i <= 12; i++)
    {
        if(yearEndMonth >= curMonth)
        {
            if((yearEndMonth + i) > 12)
                date.setYMD(curYear, (i + yearEndMonth - 12), 1);
            else
                date.setYMD((curYear - 1), (i + yearEndMonth), 1);
        }
        else
        {
            if((yearEndMonth + i) > 12)
                date.setYMD((curYear + 1), (i + yearEndMonth - 12), 1);
            else
                date.setYMD(curYear, (i + yearEndMonth), 1);
        }

        if(!begin)
            date = QDate(date.year(), date.month(), date.daysInMonth());

        periods << ("Period " + QString::number(i) + " (" +
                    date.toString(Qt::LocalDate) + ")");
    }

    return periods;
}

QDate Client::getPeriodDate(int period, bool endOfMonth) const
{
    QDate date;
    int curYear = QDate::currentDate().year();
    int curMonth = QDate::currentDate().month();

    if(yearEndMonth >= curMonth)
    {
        if((yearEndMonth + period) > 12)
            date.setYMD(curYear, (period + yearEndMonth - 12), 1);
        else
            date.setYMD((curYear - 1), (period + yearEndMonth), 1);
    }
    else
    {
        if((yearEndMonth + period) > 12)
            date.setYMD((curYear + 1), (period + yearEndMonth - 12), 1);
        else
            date.setYMD(curYear, (period + yearEndMonth), 1);
    }

    if(endOfMonth)
        date = QDate(date.year(), date.month(), date.daysInMonth());

    return date;
}

int Client::getCurrentPeriod() const
{
    QDate currentDate = QDate::currentDate();
    int i;
    
    for(i = 1; i <= 12; i++)
    {
        if(getPeriodDate(i, false) <= currentDate
           && getPeriodDate(i, true) >= currentDate)
            break;
    }

    return i;
}

// Private

Client::Client() : QObject(0, "Client")
{}

Client::~Client()
{
    close();
    
    settings->setDefaultClientId(clientId);
    settings->setDefaultClientName(clientName);
}


