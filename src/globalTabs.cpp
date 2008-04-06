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

#include <qapplication.h>
#include <qtabwidget.h>
#include <qstring.h>
#include <qevent.h>

#include "globalTabs.h"
#include "homeTab.h"
#include "accountsTab.h"
#include "journalTab.h"
#include "reportsTab.h"
#include "helpTab.h"
#include "settings.h"
#include "client.h"
#include "config.h"

GlobalTabs::GlobalTabs(const char *name)
    : QTabWidget(0, name)
{
    client = Client::instance();
    settings = Settings::instance();

    // Connect clientOpened and clientClosed signals (from the Client instance)
    // so that we enable and disable tabs at the right times.
    connect(client, SIGNAL(clientOpened(QString)), this, SLOT(clientOpened(QString)));
    connect(client, SIGNAL(clientClosed(QString)), this, SLOT(clientClosed()));

    // Do not let the keyboard get focus on our tabs
    setFocusPolicy(QWidget::NoFocus);

    // Set first caption; is changed via clientOpened and Closed signals from Client
    // or from clientNameChanged
    setCaption("General Ledger");
    connect(client, SIGNAL(clientNameChanged(QString)), this, SLOT(clientNameChanged(QString)));

    // The first tab; contains the menu interface.
    // It emits lots of signals for changing tabs, closing, etc.
    homeTab = new HomeTab(this, "globalTabs::homeTab");
    connect(homeTab, SIGNAL(switchToAccounts()), this, SLOT(switchToAccounts()));
    connect(homeTab, SIGNAL(switchToJournal()), this, SLOT(switchToJournal()));
    connect(homeTab, SIGNAL(switchToReports()), this, SLOT(switchToReports()));
    connect(homeTab, SIGNAL(quit()), this, SLOT(quit()));

    // The Accounts tab.
    // Emits tab-switching signals since the table it contains intercepts
    // F# keys, which should switch tabs.
    accountsTab = new AccountsTab(this, "globalTabs::accountsTab");
    connect(accountsTab, SIGNAL(switchToHome()), this, SLOT(switchToHome()));
    connect(accountsTab, SIGNAL(switchToJournal()), this, SLOT(switchToJournal()));
    connect(accountsTab, SIGNAL(switchToReports()), this, SLOT(switchToReports()));
    connect(accountsTab, SIGNAL(switchToHelp()), this, SLOT(switchToHelp()));

    // The Journal tab.
    // Emits tab-switching signals since the table it contains intercepts
    // F# keys, which should switch tabs.
    journalTab = new JournalTab(this, "globalTabs::journalTab");
    connect(journalTab, SIGNAL(switchToHome()), this, SLOT(switchToHome()));
    connect(journalTab, SIGNAL(switchToAccounts()), this, SLOT(switchToAccounts()));
    connect(journalTab, SIGNAL(switchToReports()), this, SLOT(switchToReports()));
    connect(journalTab, SIGNAL(switchToHelp()), this, SLOT(switchToHelp()));

    // The Reports tab.
    // Only emits a switchToHome signal (upon ESC).
    reportsTab = new ReportsTab(this, "globalTabs::reportsTab");
    connect(reportsTab, SIGNAL(switchToHome()), this, SLOT(switchToHome()));

    // The Help tab, which like homeTab, is always active.
    // Only emits a switchToHome signal (upon ESC).
    helpTab = new HelpTab(this, "globalTabs::helpTab");
    connect(helpTab, SIGNAL(switchToHome()), this, SLOT(switchToHome()));
    
    // Add all our tabs but only enable homeTab and helpTab.
    addTab( homeTab, QIconSet(
        QPixmap::fromMimeSource(ICON_PATH + "/homeTab.png") ), "Home (F1)" );
    addTab( accountsTab, QIconSet(
        QPixmap::fromMimeSource(ICON_PATH + "/accountsTab.png") ), "Accounts (F2)" );
    setTabEnabled(accountsTab, 0);
    addTab( journalTab, QIconSet(
        QPixmap::fromMimeSource(ICON_PATH + "/journalTab.png") ), "Journal Entry (F3)" );
    setTabEnabled(journalTab, 0);
    addTab( reportsTab, QIconSet(
        QPixmap::fromMimeSource(ICON_PATH + "/reportsTab.png") ), "Reports (F4)" );
    setTabEnabled(reportsTab, 0);
    addTab( helpTab, QIconSet(
        QPixmap::fromMimeSource(ICON_PATH + "/helpTab.png") ), "Help (F5)" );

    // Size and position the window (we are the main widget).
    // Note that if the window was maximized, the all values are 10000.
    if(settings->getDefaultX() == 10000 && settings->getDefaultY() == 10000 
       && settings->getDefaultWidth() == 10000 && settings->getDefaultHeight() == 10000)
        showMaximized();
    else
    {
        move(settings->getDefaultX(), settings->getDefaultY());
        resize(settings->getDefaultWidth(), settings->getDefaultHeight());
    }
}

/// \note
/// Received from Client
void GlobalTabs::clientOpened(QString clientName)
{
    setCaption("General Ledger - " + clientName);

    setTabEnabled(accountsTab, true);
    setTabEnabled(journalTab, true);
    setTabEnabled(reportsTab, true);
}

/// \note
/// Received from Client
void GlobalTabs::clientClosed()
{
    setCaption("General Ledger");

    setTabEnabled(accountsTab, false);
    setTabEnabled(journalTab, false);
    setTabEnabled(reportsTab, false);
}

/// \note
/// Received from Client
void GlobalTabs::clientNameChanged(QString clientName)
{
    setCaption("General Ledger - " + clientName);
}

/// \note
/// Received from AccountsTab, JournalTab, ReportsTab, HelpTab
void GlobalTabs::switchToHome()
{
    showPage(homeTab);
}

/// \note
/// Received from HomeTab, JournalTab
void GlobalTabs::switchToAccounts()
{
    showPage(accountsTab);
}

/// \note
/// Received from HomeTab, AccountsTab
void GlobalTabs::switchToJournal()
{
    showPage(journalTab);
}

/// \note
/// Received from HomeTab, AccountsTab, JournalTab
void GlobalTabs::switchToReports()
{
    showPage(reportsTab);
}

/// \note
/// Received from HomeTab, AccountsTab, JournalTab
void GlobalTabs::switchToHelp()
{
    showPage(helpTab);
}

/// \note
/// Received from HomeTab
void GlobalTabs::quit()
{
    // Save normal layout settings when not maximized
    if(!isMaximized())
    {
        settings->setDefaultX(x());
        settings->setDefaultY(y());
        settings->setDefaultWidth(width());
        settings->setDefaultHeight(height());
    }

    // Save bizarre layout settings when maximized
    else
    {
        settings->setDefaultX(10000);
        settings->setDefaultY(10000);
        settings->setDefaultWidth(10000);
        settings->setDefaultHeight(10000);
    }
   
    // Finally quit 
    qApp->quit();
}

void GlobalTabs::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Key_F1:
            showPage(homeTab);
            break;
        case Key_F2:
            if(isTabEnabled(accountsTab))
                showPage(accountsTab);
            break;
        case Key_F3:
            if(isTabEnabled(journalTab))
                showPage(journalTab);
            break;
        case Key_F4:
            if(isTabEnabled(reportsTab))
                showPage(reportsTab);
            break;
        case Key_F5:
            showPage(helpTab);
            break;
    }
}

void GlobalTabs::closeEvent(QCloseEvent *event)
{
    homeTab->closeClient();

    if(!client->isOpen())
    {
        // call our quit function (slot)
        quit();
        event->accept();
    }
    else
    {
        // An `else' would be the user wants to return to the journal
        // entries instead of closing.
        event->ignore();
    }
}


