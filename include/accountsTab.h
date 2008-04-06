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

#ifndef ACCOUNTS_TAB_H
#define ACCOUNTS_TAB_H

#include <qwidget.h>

class QEvent;
class QVBoxLayout;
class QHBoxLayout;
class QVGroupBox;
class QPushButton;
class QLabel;
class QFrame;
class QBoxLayout;

class GlobalGoBackLabel;
class AccountsTable;
class AccountsData;

/// Accounts Tab
class AccountsTab : public QWidget
{
    Q_OBJECT
    public:
        /// Constructor
        AccountsTab( QWidget *parent, const char *name );
        
    signals:
        /// Go to Home tab
        void switchToHome();

        /// Go to Journal tab
        void switchToJournal();

        /// Go to Reports tab
        void switchToReports();

        /// Go to Help tab
        void switchToHelp();
        
    private slots:
        /// Import CSV data into accounts table
        void importCSV();

        /// Export CSV data from accounts table
        void exportCSV();

        void chooseREAccount();
        
    private:
        bool eventFilter(QObject *target, QEvent *event);
        void showEvent(QShowEvent*);
        
        AccountsData *accountsData;

        struct {
            QVBoxLayout *vBoxLayout;
            QHBoxLayout *labelLayout;
            GlobalGoBackLabel *topLabel;
            QPushButton *chooseREAccount;
            AccountsTable *dataTable;
            QWidget *bottomWidget;
            QHBoxLayout *bottomHBoxLayout;
            QLabel *bottomLabel;
            QFrame *bottomRightFrame;
            QBoxLayout *bottomRightBoxLayout;
            QPushButton *importButton;
            QPushButton *exportButton;
        } main;
        
};


#endif

