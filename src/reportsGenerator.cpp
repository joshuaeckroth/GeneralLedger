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
#include <qdatetime.h>

#include "reportsGenerator.h"
#include "reportsEditor.h"
#include "reportsPrint.h"
#include "client.h"
#include "accountsData.h"
#include "journalData.h"
#include "main.h"
#include "config.h"

ReportsGenerator* ReportsGenerator::thisInstance = 0;

ReportsGenerator* ReportsGenerator::instance()
{
    if(thisInstance == 0)
        thisInstance = new ReportsGenerator;
    return thisInstance;
}

void ReportsGenerator::destroy()
{
    if(thisInstance != 0)
        delete thisInstance;
}

void ReportsGenerator::genGeneralDetailReport(
                QDate periodBegin, QDate periodEnd,
                QString accountBegin, QString accountEnd)
{
    setDocName("General Ledger Detail");
    csv = "\"Account ID\",\"Account Description\",\"Date\",\"Reference\","
          "\"Description\",\"Debit Amt\",\"Credit Amt\",\"Balance\"\n";
    header = "<html><body><center>" + client->getClientName() +
	     " - General Ledger Detail</center></body></html>";
    html = "<html><head><title>" + client->getClientName() +
	   "</title></head><body>\n"
           "<center><h1>" + client->getClientName() + "</h1></center>\n"
           "<center><h2>General Ledger Detail</h2></center>\n"
           "<center><h3>For the Period From " +
	   periodBegin.toString("MMM d, yyyy") +
           " to " + periodEnd.toString("MMM d, yyyy") + "</h3></center>\n";
    if(accountBegin == "" && accountEnd == "")
    {
        html += "<center><h4>All Accounts</h4></center>\n";
    }
    else
    {
        html += "<center><h4>Account Range: " + accountBegin +
                " to " + accountEnd + "</h4></center>\n";
    }

    html += "<table width=\"100%\"><tr><td> </td></tr><tr>\n"
            "<td width=\"19%\"><b>Account ID<br>Account Description</b></td>\n"
            "<td width=\"10%\"><b>Date</b></td>\n"
            "<td width=\"10%\"><b>Reference</b></td>\n"
            "<td width=\"31%\"><b>Description</b></td>\n"
            "<td align=\"right\" width=\"10%\"><b>Debit Amt</b></td>\n"
            "<td align=\"right\" width=\"10%\"><b>Credit Amt</b></td>\n"
            "<td align=\"right\" width=\"10%\"><b>Balance</b></td>\n"
            "</tr>\n";

    // Get all the accounts we need, sorted, with REAccount added
    vector<AccountsData::Account*> accounts = accountsData->getAccounts(
                    accountBegin, accountEnd, true, true);

    Q_INT64 beginningBalance;
    Q_INT64 debitBalance;
    Q_INT64 creditBalance;

    bool accountDisplayed;

    // Prepare the journal records container
    vector<JournalData::JournalRecord*> journalRecords;

    // Look at every account
    for(vector<AccountsData::Account*>::const_iterator accountIt =
	accounts.begin(); accountIt != accounts.end(); ++accountIt)
    {
        accountDisplayed = false;

        beginningBalance = 0;
        debitBalance = 0;
        creditBalance = 0;

        // Get the journal entries prior to our periodBegin date
        // to compute the Beginning Balance column
        journalRecords = journalData->getJournalRecords(
            (*accountIt)->id, (*accountIt)->id,
            QDate(), periodBegin.addDays(-1));

        for(vector<JournalData::JournalRecord*>::const_iterator journalIt =
	    journalRecords.begin(); journalIt != journalRecords.end();
            ++journalIt)
        {
            beginningBalance += (*journalIt)->amount;
        }

        html += "<tr><td width=\"19%\">" + (*accountIt)->id + "</td>"
                "<td width=\"10%\">" + periodBegin.toString("MM/dd/yyyy") + "</td>"
                "<td width=\"10%\"> </td>"
                "<td width=\"31%\">Beginning Balance</td>"
                "<td width=\"10%\"> </td>"
                "<td width=\"10%\"> </td>";

        if(beginningBalance == 0)
            html += "<td width=\"10%\"> </td>";
        else
            html += "<td align=\"right\" width=\"10%\">" +
                    formatCurrency(beginningBalance) + "</td>";

        html += "</tr>\n";

        csv += (*accountIt)->id + ",\"" + (*accountIt)->desc + "\"," +
               periodBegin.toString(Qt::ISODate) + ",,\"Beginning Balance\",,," +
               formatCurrency(beginningBalance, false) + "\n";

        // Get the journal records for this account, with max and min dates,
        // sorted by date
        journalRecords = journalData->getJournalRecords(
                        (*accountIt)->id, (*accountIt)->id,
                        periodBegin, periodEnd, JournalData::SortDate);

        // Look at every journal record
        for(vector<JournalData::JournalRecord*>::const_iterator journalIt =
            journalRecords.begin(); journalIt != journalRecords.end();
            ++journalIt)
        {
            if((*journalIt)->amount < 0)
                creditBalance += abs((*journalIt)->amount);
            else
                debitBalance += (*journalIt)->amount;


            if(!accountDisplayed)
            {
                html += "<tr><td width=\"19%\">" + (*accountIt)->desc + "</td>";
                accountDisplayed = true;
            }
            else
            {
                html += "<tr><td width=\"19%\"> </td>";
            }

            html += "<td width=\"10%\">" +
                    ((*journalIt)->date).toString("MM/dd/yyyy") + "</td>"
                    "<td width=\"10%\">" + (*journalIt)->reference + "</td>"
                    "<td width=\"31%\">" + (*journalIt)->desc + "</td>";

            csv += (*accountIt)->id + ",\"" + (*accountIt)->desc + "\"," +
                   ((*journalIt)->date).toString(Qt::ISODate) + "," +
                   "\"" + (*journalIt)->reference + "\"," +
                   "\"" + (*journalIt)->desc + "\",";

            if((*journalIt)->amount > 0)
            {
                html += "<td align=\"right\" width=\"10%\">" +
                        formatCurrency((*journalIt)->amount) +
                        "</td><td width=\"10%\"> </td>";

                csv += formatCurrency((*journalIt)->amount, false);
            }
            else
            {
                html += "<td width=\"10%\"> </td><td align=\"right\" "
                        "width=\"10%\">" +
                        formatCurrency(abs((*journalIt)->amount)) + "</td>";

                csv += formatCurrency((*journalIt)->amount, false);
            }

            csv += ",\n";
        }

        if(debitBalance != 0 || creditBalance != 0)
        {
            html += "</tr>\n"
                    "<tr><td width=\"19%\"> </td>"
                    "<td width=\"10%\"> </td>"
                    "<td width=\"10%\"> </td>"
                    "<td width=\"31%\">Current Period Change</td>";
            
            if(debitBalance == 0)
                html += "<td width=\"10%\"> </td>";
            else
                html += "<td align=\"right\" width=\"10%\">" +
                        formatCurrency(debitBalance) + "</td>";

            if(creditBalance == 0)
                html += "<td width=\"10%\"> </td>";
            else
                html += "<td align=\"right\" width=\"10%\">" +
                        formatCurrency(creditBalance) + "</td>";

            if(creditBalance == 0 && debitBalance == 0)
            {
                html += "<td width=\"10%\"> </td>";
            }
            else
            {
                html += "<td align=\"right\" width=\"10%\">" +
                        formatCurrency(debitBalance - creditBalance) + "</td>";
            }

            csv += ",,,,\"Current Period Change\"," +
                   formatCurrency(debitBalance, false) + "," +
                   formatCurrency(creditBalance, false) + "," +
                   formatCurrency(debitBalance - creditBalance, false) + "\n";
        }

        if(!accountDisplayed)
            html += "<tr><td width=\"19%\">" + (*accountIt)->desc +
                    "</td></tr>\n";

        html += "<tr><td width=\"19%\"> </td>"
                "<td width=\"10%\"><b>" + periodEnd.toString("MM/dd/yyyy") +
                "</b></td>"
                "<td width=\"10%\"> </td>"
                "<td width=\"31%\"><b>Ending Balance</b></td>"
                "<td width=\"10%\"> </td>"
                "<td width=\"10%\"> </td>";

        if(beginningBalance == 0 && debitBalance == 0 && creditBalance == 0)
        {
            html += "<td width=\"10%\"> </td>";
        }
        else
        {
            html += "<td align=\"right\" width=\"10%\"><b>" +
                    formatCurrency(beginningBalance + debitBalance
                                    - creditBalance) + "</b></td>";
        }

        html += "</tr><tr><td> </td></tr><tr><td> </td></tr>\n";

        csv += ",," + periodEnd.toString(Qt::ISODate) +
               ",,\"Ending Balance\",,," +
               formatCurrency(beginningBalance + debitBalance 
                               - creditBalance, false) + "\n";
    }

    html += "</table></body></html>";
}

void ReportsGenerator::genGeneralTrialReport(
                QDate periodEnd, QString accountBegin, QString accountEnd)
{
    setDocName("General Ledger Trial Balance");
    csv = "\"Account Id\",\"Account Description\",\"Debit Amt\","
          "\"Credit Amt\"\n";
    header = "<html><body><center>" + client->getClientName() +
	     " - General Ledger Trial Balance</center></body></html>";
    html = "<html><head><title>" + client->getClientName() +
           "</title></head><body>\n"
           "<center><h1>" + client->getClientName() + "</h1></center>\n"
           "<center><h2>General Ledger Trial Balance</h2></center>\n"
           "<center><h3>As of " + periodEnd.toString("MMM d, yyyy") +
           "</h3></center>\n";
    if(accountBegin == "" || accountEnd == "")
    {
        html += "<center><h4>All Accounts</h4></center>\n";
    }
    else
    {
        html += "<center><h4>Account Range: " + accountBegin + " to "
                + accountEnd +
                "</h4></center>\n";
    }
    html += "<table width=\"100%\"><tr><td> </td></tr><tr><td width=\"19%\">"
            "<b>Account ID</b></td>\n"
            "<td width=\"31%\"><b>Account Description</b></td>\n"
            "<td align=\"right\" width=\"10%\"><b>Debit Amt</b></td>\n"
            "<td align=\"right\" width=\"10%\"><b>Credit Amt</b></td>\n"
            "<td width=\"30%\"> </td></tr></table>\n"
            "<table width=\"100%\">\n";

    // Get all the accounts we need, sorted, with REAccount added
    vector<AccountsData::Account*> accounts = accountsData->getAccounts(
                    accountBegin, accountEnd, true, true);

    // Prepare our running totals
    Q_INT64 accountBalance;
    Q_INT64 totalDebit = 0;
    Q_INT64 totalCredit = 0;

    // Prepare the journal records container
    vector<JournalData::JournalRecord*> journalRecords;

    // Look at every account
    for(vector<AccountsData::Account*>::const_iterator accountIt =
        accounts.begin(); accountIt != accounts.end(); ++accountIt)
    {
        html += "<tr><td width=\"19%\">" + (*accountIt)->id + "</td>"
                "<td width=\"31%\">" + (*accountIt)->desc + "</td>";

        csv += "\"" + (*accountIt)->id + "\"," +
               "\"" + (*accountIt)->desc + "\",";

        // Initialize our account balance
        accountBalance = 0;

        // Get the journal records for this account, with a max date
        journalRecords = journalData->getJournalRecords(
            (*accountIt)->id, (*accountIt)->id, QDate(), periodEnd);

        // Look at every journal record
        for(vector<JournalData::JournalRecord*>::const_iterator journalIt =
            journalRecords.begin(); journalIt != journalRecords.end();
            ++journalIt)
        {
            // Update our account balance
            accountBalance += (*journalIt)->amount;
        }

        // Update our total balances
        if(accountBalance > 0)
            totalDebit += accountBalance;
        else
            totalCredit += accountBalance;

        html += "<td align=\"right\" width=\"10%\">";
        if(accountBalance > 0)
        {
            html += formatCurrency(abs(accountBalance));
            csv += formatCurrency(abs(accountBalance), false) + ",";
        }
        else
        {
            csv += "0,";
        }
        html += "</td><td align=\"right\" width=\"10%\">";
        if(accountBalance < 0)
        {
            html += formatCurrency(accountBalance);
            csv += formatCurrency(accountBalance, false) + "\n";
        }
        else
        {
            csv += "0\n";
        }
        html += "</td><td width=\"30%\"> </td></tr>\n";
    }

    html += "<tr><td> </td><td> </td><td> </td><td> </td></tr>\n"
            "<tr><td width=\"19%\"> </td><td width=\"31%\"><b>Total</b></td>\n"
            "<td align=\"right\" width=\"10%\"><b>" +
            formatCurrency(abs(totalDebit)) +
            "</b></td><td align=\"right\" width=\"10%\"><b>" +
            formatCurrency(totalCredit) +
            "</b></td><td width=\"30%\"> </td></tr></table></body></html>\n";

    csv += ",\"Total\"," + formatCurrency(abs(totalDebit), false) + "," +
           formatCurrency(totalCredit, false) + "\n";
}

void ReportsGenerator::genBalanceSheetReport(QDate periodEnd)
{
    setDocName("Balance Sheet");
    csv = "\"Account Description\",\"Balance\"\n";
    header = "<html><body><center>" + client->getClientName() +
	     " - Balance Sheet</center></body></html>";
    html = "<html><head><title>" + client->getClientName() +
           " - Balance Sheet</title></head><body>\n"
           "<center><h1>" + client->getClientName() + "</h1></center>\n"
           "<center><h2>Balance Sheet</h2></center>\n"
           "<center><h3>" + periodEnd.toString("MMM d, yyyy") +
           "</h3></center>\n"
           "<center><table width=\"60%\"><tr>"
           "<td width=\"70%\"><b>Account Description</b></td>"
           "<td align=\"right\" width=\"30%\"><b>Balance</b></td>"
           "</tr></table></center>\n"
           "<center><table width=\"60%\">\n";

    vector<ReportsEditor::Section> sections;
    sections.push_back(ReportsEditor::Assets);
    sections.push_back(ReportsEditor::Liabilities);
    sections.push_back(ReportsEditor::Equities);

    Q_INT64 accountBalance;
    Q_INT64 sectionBalance;

    for(vector<ReportsEditor::Section>::const_iterator sectionIt =
        sections.begin(); sectionIt != sections.end(); ++sectionIt)
    {
	html += "<tr><td colspan=\"2\"><center><h3>";
	if((*sectionIt) == ReportsEditor::Assets)
	{
	    html += "Assets";
	}
	else if((*sectionIt) == ReportsEditor::Liabilities)
	{
	    html += "Liabilities";
	}
	else if((*sectionIt) == ReportsEditor::Equities)
	{
	    html += "Equities";
	}
	html += "</h3></td></tr>\n";
	
        sectionBalance = 0;

        vector<ReportsEditor::Entity*> entities = 
            reportsEditor->getEntities(ReportsEditor::BalanceSheet, *sectionIt);

        for(vector<ReportsEditor::Entity*>::const_iterator entityIt =
            entities.begin(); entityIt != entities.end(); ++entityIt)
        {
            accountBalance = 0;

            vector<JournalData::JournalRecord*> records =
                journalData->getJournalRecords((*entityIt)->accountBegin,
                    (*entityIt)->accountEnd, QDate(), periodEnd);

            for(vector<JournalData::JournalRecord*>::const_iterator recordIt =
                records.begin(); recordIt != records.end(); ++recordIt)
            {
                accountBalance += (*recordIt)->amount;
            }

            sectionBalance += accountBalance;

            html += "<tr><td width=\"70%\">" + (*entityIt)->desc + "</td>"
                    "<td align=\"right\" width=\"30%\">" +
                    formatCurrency(accountBalance) + "</td></tr>\n";

            csv += (*entityIt)->desc + "," +
                   formatCurrency(accountBalance, false) + "\n";
        }

        html += "<tr><td> </td></tr>\n"
                "<tr><td width=\"70%\"><b>";
        if((*sectionIt) == ReportsEditor::Assets)
        {
            html += "Total Assets";
            csv += "\"Total Assets\"";
        }
        else if((*sectionIt) == ReportsEditor::Liabilities)
        {
            html += "Total Liabilities";
            csv += "\"Total Liabilities\"";
        }
        else if((*sectionIt) == ReportsEditor::Equities)
        {
            html += "Total Equities";
            csv += "\"Total Equities\"";
        }

        html += "</b></td><td align=\"right\" width=\"30%\"><b>" + 
                formatCurrency(sectionBalance) + "</b></td></tr>"
                "<tr><td> </td></tr><tr><td> </td></tr>\n";

        csv += "," + formatCurrency(sectionBalance, false) + "\n";
    } 

    html += "</table></body></html>\n";
}

void ReportsGenerator::genIncomeSvcReport(QDate periodEnd)
{
    QDate rangeBegin = client->getPeriodDate(1, false);
    QDate monthBegin = periodEnd.addMonths(-1).addDays(1);

    int monthRangeInt = periodEnd.month() - rangeBegin.month();
    if(monthRangeInt < 0)
        monthRangeInt += 12;
    ++monthRangeInt;

    QString monthRange = numberToString(monthRangeInt);

    setDocName("Income Statement Service Company");
    csv = "\"Account Description\",\"One Month Balance\",";
    if(monthRangeInt == 1)
        csv += "\"One Month Balance\"\n";
    else
        csv += "\"" + monthRange + " Month's Balance\"\n";

    header = "<html><body><center>" + client->getClientName() +
             " - Income Statement Service Company</center></body></html>";
    html = "<html><head><title>" + client->getClientName() +
           " - Income Statement Service Company</title></head></body>\n"
           "<center><h1>" + client->getClientName() + "</h1></center>\n"
           "<center><h2>Income Statement Service Company</h2></center>\n"
           "<center><h3>For One Month and ";
    if(monthRangeInt == 1)
        html += "One Month";
    else
        html += monthRange + " Months";

    html += " Ended " + periodEnd.toString("MMM d, yyyy") + "</h3>\n"
            "<table width=\"80%\"><tr>"
            "<td width=\"50%\"><b>Account Description</b></td>"
            "<td align=\"right\" width=\"25%\"><b>One Month</b></td>"
            "<td align=\"right\" width=\"25%\"><b>";
    if(monthRangeInt == 1)
        html += "One Month";
    else
        html += monthRange + " Months";

    html += "</b></td></tr></table></center>"
            "<center><table width=\"80%\">";

    vector<ReportsEditor::Section> sections;
    sections.push_back(ReportsEditor::Income);
    sections.push_back(ReportsEditor::Expenses);

    Q_INT64 sectionBalanceRange;
    Q_INT64 sectionBalanceMonth;
    Q_INT64 accountBalanceRange;
    Q_INT64 accountBalanceMonth;

    for(vector<ReportsEditor::Section>::const_iterator sectionIt =
        sections.begin(); sectionIt != sections.end(); ++sectionIt)
    {
        html += "<tr><td colspan=\"3\"><center><h3>";
        if((*sectionIt) == ReportsEditor::Income)
        {
            html += "Income";
        }
        else if((*sectionIt) == ReportsEditor::Expenses)
        {
            html += "Expenses";
        }
        html += "</h3></td></tr>\n";

        sectionBalanceRange = 0;
        sectionBalanceMonth = 0;

        vector<ReportsEditor::Entity*> entities =
            reportsEditor->getEntities(ReportsEditor::IncomeSvc, *sectionIt);

        for(vector<ReportsEditor::Entity*>::const_iterator entityIt =
            entities.begin(); entityIt != entities.end(); ++entityIt)
        {
            accountBalanceRange = 0;
            accountBalanceMonth = 0;

            vector<JournalData::JournalRecord*> recordsRange =
                journalData->getJournalRecords((*entityIt)->accountBegin,
                    (*entityIt)->accountEnd, rangeBegin, periodEnd);

            for(vector<JournalData::JournalRecord*>::const_iterator recordRangeIt =
                recordsRange.begin(); recordRangeIt != recordsRange.end();
                ++recordRangeIt)
            {
                accountBalanceRange += (*recordRangeIt)->amount;
            }

            vector<JournalData::JournalRecord*> recordsMonth =
                journalData->getJournalRecords((*entityIt)->accountBegin,
                    (*entityIt)->accountEnd, monthBegin, periodEnd);

            for(vector<JournalData::JournalRecord*>::const_iterator recordMonthIt =
                recordsMonth.begin(); recordMonthIt != recordsMonth.end();
                ++recordMonthIt)
            {
                accountBalanceMonth += (*recordMonthIt)->amount;
            }

            sectionBalanceRange += accountBalanceRange;
            sectionBalanceMonth += accountBalanceMonth;

            html += "<tr><td width=\"50%\">" + (*entityIt)->desc + "</td>"
                    "<td align=\"right\" width=\"25%\">" +
                    formatCurrency(accountBalanceMonth) + "</td>"
                    "<td align=\"right\" width=\"25%\">" +
                    formatCurrency(accountBalanceRange) + "</td></tr>\n";

            csv += "\"" + (*entityIt)->desc + "\"," +
                   formatCurrency(accountBalanceMonth, false) + "," +
                   formatCurrency(accountBalanceRange, false) + "\n";
        }

        html += "<tr><td> </td></tr>\n"
                "<tr><td align=\"right\" width=\"50%\"><b>";
        if((*sectionIt) == ReportsEditor::Income)
        {
            html += "Total Income";
            csv += "\"Total Income\"";
        }
        else if((*sectionIt) == ReportsEditor::Expenses)
        {
            html += "Total Expenses";
            csv += "\"Total Expenses\"";
        }
        html += "</b></td><td align=\"right\" width=\"25%\"><b>" +
                formatCurrency(sectionBalanceMonth) + "</b></td>"
                "<td align=\"right\" width=\"25%\"><b>" +
                formatCurrency(sectionBalanceRange) + "</b></td></tr>"
                "<tr><td> </td></tr><tr><td> </td></tr>\n";

        csv += "," + formatCurrency(sectionBalanceMonth, false) + "," +
                formatCurrency(sectionBalanceRange, false) + "\n";
    }

    html += "</table></body></html>\n";
}

void ReportsGenerator::genIncomeRetailReport(QDate periodEnd)
{
    QDate rangeBegin = client->getPeriodDate(1, false);
    QDate monthBegin = periodEnd.addMonths(-1).addDays(1);

    int monthRangeInt = periodEnd.month() - rangeBegin.month();
    if(monthRangeInt < 0)
        monthRangeInt += 12;
    ++monthRangeInt;

    QString monthRange = numberToString(monthRangeInt);

    setDocName("Income Statement Retail");
    csv = "\"Account Description\",\"One Month Balance\",";
    if(monthRangeInt == 1)
        csv += "\"One Month Balance\"\n";
    else
        csv += "\"" + monthRange + " Month's Balance\"\n";

    header = "<html><body><center>" + client->getClientName() +
             " - Income Statement Retail</center></body></html>";
    html = "<html><head><title>" + client->getClientName() +
           " - Income Statement Retail</title></head></body>\n"
           "<center><h1>" + client->getClientName() + "</h1></center>\n"
           "<center><h2>Income Statement Retail</h2></center>\n"
           "<center><h3>For One Month and ";
    if(monthRangeInt == 1)
        html += "One Month";
    else
        html += monthRange + " Months";

    html += " Ended " + periodEnd.toString("MMM d, yyyy") + "</h3>\n"
            "<table width=\"80%\"><tr>"
            "<td width=\"50%\"><b>Account Description</b></td>"
            "<td align=\"right\" width=\"25%\"><b>One Month</b></td>"
            "<td align=\"right\" width=\"25%\"><b>";
    if(monthRangeInt == 1)
        html += "One Month";
    else
        html += monthRange + " Months";

    html += "</b></td></tr></table></center>"
            "<center><table width=\"80%\">";

    vector<ReportsEditor::Section> sections;
    sections.push_back(ReportsEditor::Income);
    sections.push_back(ReportsEditor::CostOfSales);
    sections.push_back(ReportsEditor::Expenses);

    Q_INT64 incomeBalanceRange = 0;
    Q_INT64 incomeBalanceMonth = 0;
    Q_INT64 costOfSalesBalanceRange = 0;
    Q_INT64 costOfSalesBalanceMonth = 0;
    Q_INT64 expensesBalanceRange = 0;
    Q_INT64 expensesBalanceMonth = 0;

    Q_INT64 accountBalanceRange;
    Q_INT64 accountBalanceMonth;

    for(vector<ReportsEditor::Section>::const_iterator sectionIt =
        sections.begin(); sectionIt != sections.end(); ++sectionIt)
    {
        html += "<tr><td colspan=\"3\"><center><h3>";
        if((*sectionIt) == ReportsEditor::Income)
        {
            html += "Income";
        }
        else if((*sectionIt) == ReportsEditor::CostOfSales)
        {
            html += "Cost Of Sales";
        }
        else if((*sectionIt) == ReportsEditor::Expenses)
        {
            html += "Expenses";
        }
        html += "</h3></td></tr>\n";

        vector<ReportsEditor::Entity*> entities =
            reportsEditor->getEntities(ReportsEditor::IncomeRetail, *sectionIt);

        for(vector<ReportsEditor::Entity*>::const_iterator entityIt =
            entities.begin(); entityIt != entities.end(); ++entityIt)
        {
            accountBalanceRange = 0;
            accountBalanceMonth = 0;

            vector<JournalData::JournalRecord*> recordsRange =
                journalData->getJournalRecords((*entityIt)->accountBegin,
                    (*entityIt)->accountEnd, rangeBegin, periodEnd);

            for(vector<JournalData::JournalRecord*>::const_iterator recordRangeIt =
                recordsRange.begin(); recordRangeIt != recordsRange.end();
                ++recordRangeIt)
            {
                accountBalanceRange += (*recordRangeIt)->amount;
            }

            vector<JournalData::JournalRecord*> recordsMonth =
                journalData->getJournalRecords((*entityIt)->accountBegin,
                    (*entityIt)->accountEnd, monthBegin, periodEnd);

            for(vector<JournalData::JournalRecord*>::const_iterator recordMonthIt =
                recordsMonth.begin(); recordMonthIt != recordsMonth.end();
                ++recordMonthIt)
            {
                accountBalanceMonth += (*recordMonthIt)->amount;
            }

            if((*sectionIt) == ReportsEditor::Income)
            {
                incomeBalanceRange += accountBalanceRange;
                incomeBalanceMonth += accountBalanceMonth;
            }
            else if((*sectionIt) == ReportsEditor::CostOfSales)
            {
                costOfSalesBalanceRange += accountBalanceRange;
                costOfSalesBalanceMonth += accountBalanceMonth;
            }
            else if((*sectionIt) == ReportsEditor::Expenses)
            {
                expensesBalanceRange += accountBalanceRange;
                expensesBalanceMonth += accountBalanceMonth;
            }

            html += "<tr><td width=\"50%\">" + (*entityIt)->desc + "</td>"
                    "<td align=\"right\" width=\"25%\">" +
                    formatCurrency(accountBalanceMonth) + "</td>"
                    "<td align=\"right\" width=\"25%\">" +
                    formatCurrency(accountBalanceRange) + "</td></tr>\n";

            csv += "\"" + (*entityIt)->desc + "\"," +
                   formatCurrency(accountBalanceMonth, false) + "," +
                   formatCurrency(accountBalanceRange, false) + "\n";
        }

        html += "<tr><td> </td></tr>\n"
                "<tr><td align=\"right\" width=\"50%\"><b>";
        if((*sectionIt) == ReportsEditor::Income)
        {
            html += "Total Income"
                    "</b></td>"
                    "<td align=\"right\" width=\"25%\"><b>" +
                    formatCurrency(incomeBalanceMonth) + "</b>"
                    "<br><img src=\"" + ICON_PATH + "/underline_single.gif\" "
                    "width=\"80\" height=\"1\"></td>"
                    "<td align=\"right\" width=\"25%\"><b>" +
                    formatCurrency(incomeBalanceRange) + "</b>"
                    "<br><img src=\"" + ICON_PATH + "/underline_single.gif\" "
                    "width=\"80\" height=\"1\"></td></tr>"
                    "<tr><td> </td></tr><tr><td> </td></tr>\n";

            csv += "\"Total Income\"," +
                   formatCurrency(incomeBalanceMonth, false) + "," +
                   formatCurrency(incomeBalanceRange, false) + "\n";
        }
        else if((*sectionIt) == ReportsEditor::CostOfSales)
        {
            html += "Total Cost of Sales"
                    "</b></td>"
                    "<td align=\"right\" width=\"25%\"><b>" +
                    formatCurrency(costOfSalesBalanceMonth) + "</b>"
                    "<br><img src=\"" + ICON_PATH + "/underline_single.gif\" "
                    "width=\"80\" height=\"1\"></td>"
                    "<td align=\"right\" width=\"25%\"><b>" +
                    formatCurrency(costOfSalesBalanceRange) + "</b>"
                    "<br><img src=\"" + ICON_PATH + "/underline_single.gif\" "
                    "width=\"80\" height=\"1\"></td></tr>\n"
                    "<tr><td align=\"right\" width=\"50%\"><b>"
                    "Gross Profit</b></td>"
                    "<td align=\"right\" width=\"25%\"><b>" +
                    formatCurrency(incomeBalanceMonth - costOfSalesBalanceMonth) +
                    "</b><br><img src=\"" + ICON_PATH + "/underline_single.gif\" "
                    "width=\"80\" height=\"1\"></td>"
                    "<td align=\"right\" width=\"25%\"><b>" +
                    formatCurrency(incomeBalanceRange - costOfSalesBalanceRange) +
                    "</b><br><img src=\"" + ICON_PATH + "/underline_single.gif\" "
                    "width=\"80\" height=\"1\"></td></tr>"
                    "<tr><td> </td></tr><tr><td> </td></tr>\n";

            csv += "\"Total Cost of Sales\"," +
                   formatCurrency(costOfSalesBalanceMonth, false) + "," +
                   formatCurrency(costOfSalesBalanceRange, false) + "\n"
                   "\"Gross Profit\"," +
                   formatCurrency(incomeBalanceMonth - costOfSalesBalanceMonth, false) + "," +
                   formatCurrency(incomeBalanceRange - costOfSalesBalanceRange, false) + "\n";
        }
        else if((*sectionIt) == ReportsEditor::Expenses)
        {
            html += "Total Expenses"
                    "</b></td>"
                    "<td align=\"right\" width=\"25%\"><b>" +
                    formatCurrency(expensesBalanceMonth) + "</b>"
                    "<br><img src=\"" + ICON_PATH + "/underline_single.gif\" "
                    "width=\"80\" height=\"1\"></td>"
                    "<td align=\"right\" width=\"25%\"><b>" +
                    formatCurrency(expensesBalanceRange) + "</b>"
                    "<br><img src=\"" + ICON_PATH + "/underline_single.gif\" "
                    "width=\"80\" height=\"1\"></td></tr>\n"
                    "<tr><td align=\"right\" width=\"50%\"><b>"
                    "Net Income (Loss)</b></td>"
                    "<td align=\"right\" width=\"25%\"><b>" +
                    formatCurrency((incomeBalanceMonth - costOfSalesBalanceMonth) -
                        expensesBalanceMonth) +
                    "</b><br><img src=\"" + ICON_PATH + "/underline_double.gif\" "
                    "width=\"80\" height=\"3\"></td>"
                    "<td align=\"right\" width=\"25%\"><b>" +
                    formatCurrency((incomeBalanceRange - costOfSalesBalanceRange) -
                        expensesBalanceRange) +
                    "</b><br><img src=\"" + ICON_PATH + "/underline_double.gif\" "
                    "width=\"80\" height=\"3\"></td></tr>\n";

            csv += "\"Total Expenses\"," +
                   formatCurrency(expensesBalanceMonth, false) + "," +
                   formatCurrency(expensesBalanceRange, false) + "\n"
                   "\"Net Income (Loss)\"," +
                   formatCurrency((incomeBalanceMonth - costOfSalesBalanceMonth) -
                       expensesBalanceMonth) + "," +
                   formatCurrency((incomeBalanceRange - costOfSalesBalanceRange) -
                       expensesBalanceRange) + "\n";
        }
    }

    html += "</table></body></html>\n";
}

void ReportsGenerator::genChartOfAccountsReport()
{
    setDocName("Chart of Accounts");
    csv = "\"Account ID\",\"Account Description\"\n";
    header = "<html><body><center>" + client->getClientName() +
	     "</center></body></html>";
    html = "<html><head><title>" + client->getClientName() +
           " - Chart of Accounts</title></head><body>\n"
           "<center><h1>" + client->getClientName() + "</center></h1>\n"
           "<center><h2>Chart of Accounts</h2></center>\n"
           "<center><h3>As of " + QDate::currentDate().toString("MMM d, yyyy") +
           "</h3></center>\n"
           "<table width=\"100%\"><tr><td> </td></tr>"
	   "<tr><td width=\"15%\"><b>Account ID</b></td>"
           "<td width=\"40%\"><b>Account Description</b></td>"
           "<td width=\"45%\"> </td></tr>\n"
           "<table width=\"100%\">\n";

    // Get all accounts, with no accountBegin or accountEnd, with sorting, with REAccount added
    vector<AccountsData::Account*> accounts = accountsData->getAccounts(
                    "", "", true, true);

    for(vector<AccountsData::Account*>::const_iterator it = accounts.begin();
                    it != accounts.end(); ++it)
    {
        csv += (*it)->id + ",\"" + (*it)->desc + "\"\n";
        html += "<tr><td width=\"15%\">" + (*it)->id + "</td>"
                "<td width=\"40%\">" + (*it)->desc + "</td>"
                "<td width=\"45%\"> </td></tr>\n";
    }

    html += "</body></html>";
}

bool ReportsGenerator::printJournalEntryReport()
{
    ReportsPrint *reportsPrint = ReportsPrint::instance();

    QString tmpHeader;
    QString tmpHtml;

    tmpHeader = "<html><body><center>" + client->getClientName() +
	        " - Journal Entry Report</center></body></html>";
    tmpHtml = "<html><head><title>" + client->getClientName() +
	      "</title></head></body>\n"
	      "<center><h1>" + client->getClientName() + "</h1></center>\n"
	      "<center><h2>Journal Entry Report</h2></center>\n"
	      "<center><h3>Entered " + QDate::currentDate().toString("MMM d, yyyy") +
	      "</h3></center>\n"
	      "<table width=\"100%\"><tr><td> </td></tr><tr>"
	      "<td width=\"10%\"><b>Date</b></td>"
	      "<td width=\"10%\"><b>Account</b></td>"
	      "<td width=\"10%\"><b>Reference</b></td>"
	      "<td width=\"50%\"><b>Description</b></td>"
	      "<td width=\"10%\" align=\"right\"><b>Debit Amt</b></td>"
	      "<td width=\"10%\" align=\"right\"><b>Credit Amt</b></td>"
	      "<\tr>\n";

    Q_INT64 debitBalance = 0;
    Q_INT64 creditBalance = 0;

    vector<JournalData::JournalRecord*> journalTmpRecords;

    // Get all journalTmp entries
    journalTmpRecords = journalData->getJournalTmpRecords();

    for(vector<JournalData::JournalRecord*>::const_iterator it =
	journalTmpRecords.begin(); it != journalTmpRecords.end(); ++it)
    {
	/// \todo fix date-format (Qt::LocalDate does 1/1/2005 instead of 01/01/2005)
	tmpHtml += "<tr>"
		   "<td width=\"10%\">" + ((*it)->date).toString("MM/dd/yyyy") + "</td>"
		   "<td width=\"10%\">" + (*it)->account + "</td>"
		   "<td width=\"10%\">" + (*it)->reference + "</td>"
		   "<td width=\"50%\">" + (*it)->desc + "</td>";
	if((*it)->amount > 0)
	{
	    debitBalance += (*it)->amount;
	    tmpHtml += "<td align=\"right\" width=\"10%\">" +
		       formatCurrency((*it)->amount) +
		       "</td><td width=\"10%\"> </td>";
	}
	else
	{
	    creditBalance += (*it)->amount;
	    tmpHtml += "<td width=\"10%\"> </td><td align=\"right\" width=\"10%\">" +
		       formatCurrency((*it)->amount) + "</td>";
	}
	
	tmpHtml += "</tr>";
    }

    tmpHtml += "<tr><td> </td></tr>\n"
	       "<tr><td width=\"80%\" align=\"right\" colspan=\"4\">Debit:</td>"
	       "<td width=\"10%\" align=\"right\">" +
	       formatCurrency(debitBalance) +
	       "</td><td width=\"10%\"> </td></tr>"
	       "<tr><td width=\"80%\" align=\"right\" colspan=\"4\">Credit:</td>"
	       "<td width=\"10%\"> </td><td width=\"10%\" align=\"right\">" +
	       formatCurrency(creditBalance) +
	       "</td></tr>"
	       "<tr><td width=\"80%\" align=\"right\" colspan=\"4\">"
	       "<b>Out of Balance:</b></td>"
	       "<td width=\"10%\"> </td><td width=\"10%\" align=\"right\"><b>" +
	       formatCurrency(debitBalance + creditBalance) +
	       "</b></td></tr>"
	       "</table></body></html>";

    return reportsPrint->printReport(tmpHeader, tmpHtml,
		    "General Ledger - " + client->getClientName() +
		    " Journal Entry Report");
}

const QString& ReportsGenerator::getReportHeader() const
{
    return header;
}

const QString& ReportsGenerator::getReportHTML() const
{
    return html;
}

const QString& ReportsGenerator::getReportCSV() const
{
    return csv;
}

QString ReportsGenerator::numberToString(int number) const
{
    QString string;

    switch(number) {
        case 1:
            string = "One";
            break;
        case 2:
            string = "Two";
            break;
        case 3:
            string = "Three";
            break;
        case 4:
            string = "Four";
            break;
        case 5:
            string = "Five";
            break;
        case 6:
            string = "Six";
            break;
        case 7:
            string = "Seven";
            break;
        case 8:
            string = "Eight";
            break;
        case 9:
            string = "Nine";
            break;
        case 10:
            string = "Ten";
            break;
        case 11:
            string = "Eleven";
            break;
        case 12:
            string = "Twelve";
            break;
        default:
            string = "Invalid Number: "
                     "reportsEditor.cpp::numberToString("
                     + QString::number(number) + ")";
            break;
    }

    return string;
}

void ReportsGenerator::setDocName(const QString newDocName)
{
    docName = newDocName;
}

QString ReportsGenerator::getDocName() const
{
    return ("General Ledger - " + client->getClientName() +
	    " - " + docName);
}

ReportsGenerator::ReportsGenerator()
{
    client = Client::instance();
    accountsData = AccountsData::instance();
    journalData = JournalData::instance();
    reportsEditor = ReportsEditor::instance();

    html = QString::null;
    csv = QString::null;
}


