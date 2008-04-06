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

#ifndef GLOBAL_TABS_H
#define GLOBAL_TABS_H

#include <qtabwidget.h>

class QString;
class QKeyEvent;
class QCloseEvent;

class Client;
class Settings;
class HomeTab;
class AccountsTab;
class JournalTab;
class ReportsTab;
class HelpTab;

/// Tab manager
///
/// Manages tab-switching, app-closing, and activating and deactivating the tabs.
/// 
class GlobalTabs : public QTabWidget
{
    Q_OBJECT
    public:
        /// Constructor
        GlobalTabs(const char *name);


    private slots:
        /// Set a new caption and enable our tabs
        void clientOpened(QString);
        /// Put the normal caption back and disable our tabs
        void clientClosed();
        /// Set a new caption with the new client name
        void clientNameChanged(QString);
        /// Go to Home tab
        void switchToHome();
        /// Go to Accounts tab
        void switchToAccounts();
        /// Go to Journal tab
        void switchToJournal();
        /// Go to Reports tab
        void switchToReports();
        /// Go to Help tab
        void switchToHelp();
        /// Cleanup and quit
        void quit();

    private:
        /// Keyboard navigation
        void keyPressEvent(QKeyEvent *event);
        /// Close event
        void closeEvent(QCloseEvent *event);

        /// Client connection
        Client *client;
        /// Settings connection
        Settings *settings;

        /// The first tab; contains the menu interface
        HomeTab *homeTab;
        /// The Accounts tab
        AccountsTab *accountsTab;
        /// The Journal tab
        JournalTab *journalTab;
        /// The Reports tab
        ReportsTab *reportsTab;
        /// The Help tab
        HelpTab *helpTab;
};

#endif

