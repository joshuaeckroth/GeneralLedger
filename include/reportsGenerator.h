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

#ifndef REPORTS_GENERATOR_H
#define REPORTS_GENERATOR_H

#include <qdatetime.h>

class QString;

class Client;
class AccountsData;
class JournalData;
class ReportsEditor;

/// Creates reports
class ReportsGenerator
{
    public:
        /// Returns the single instance of this class
        static ReportsGenerator* instance();

        /// Destroys the single instance
        void destroy();
        
        /// Generate General Ledger Detail report
        void genGeneralDetailReport(
            QDate periodBegin, QDate periodEnd,
            QString accountBegin, QString accountEnd);

        /// Generate General Ledger Trial Balance report
        void genGeneralTrialReport(
            QDate periodEnd, QString accountBegin = "", QString accountEnd = "");

        /// Generate Balance Sheet report
        void genBalanceSheetReport(QDate periodEnd);

        /// Generate Income Statement Service report
        void genIncomeSvcReport(QDate periodEnd);

        /// Generate Income Statement Retail report
        void genIncomeRetailReport(QDate periodEnd);

        /// Generate Chart of Accounts report
        void genChartOfAccountsReport();

        /// Print a Journal Entry report
        ///
        /// This report generates and prints the journal entries in the JournalTab
        /// in the order that they were entered.
        bool printJournalEntryReport();

	const QString& getReportHeader() const;

        /// Retrieve the report in HTML form
        ///
        /// This is used in the app and ReportsPrint
        const QString& getReportHTML() const;
        
        /// Retrieve the report's data in CSV form
        ///
        /// This data is used for exporting the report to CSV
        const QString& getReportCSV() const;

	void setDocName(const QString newDocName);

	QString getDocName() const;

    private:
        ReportsGenerator();
        QString numberToString(int number) const;

        static ReportsGenerator *thisInstance;

        Client *client;
        AccountsData *accountsData;
        JournalData *journalData;
        ReportsEditor *reportsEditor;
       
        QString docName;	
	QString header;
        QString html;
        QString csv;

};

#endif

