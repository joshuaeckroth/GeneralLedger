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

#ifndef MAIN_STACK_H
#define MAIN_STACK_H

#include <qwidgetstack.h>

class QEvent;
class QHBoxLayout;
class QVBoxLayout;
class QVGroupBox;
class QPushButton;

class Client;
class JournalData;
class Settings;

/// Home Tab
class HomeTab : public QWidgetStack
{
    Q_OBJECT
    public:
        /// Constructor
        HomeTab(QWidget *parent, const char *name);

    signals:
        /// Go to Accounts Tab
        void switchToAccounts();

        /// Go to Journal Tab
        void switchToJournal();

        /// Go to Reports tab
        void switchToReports();

        /// Quit the program (user pressed Quit button)
        void quit();

    public slots:
        void closeClient();

    private slots:
        void clientOpened(QString);
        void openDefault();
        void openNewDialog();
        void createNewDialog();
        void editName();
        void changePath();
        //void import();
        //void encrypt();
        //void decrypt();
        void prepareQuit();
         
    private:
        void switchWidget();
        bool eventFilter(QObject *target, QEvent *event);
        
        Client *client;
        Settings *settings;
        JournalData *journalData;

        /*
        QString exportPath;
        QString importPath;
        */
        
        int active;
        
        struct {
            QWidget *widget;
            QHBoxLayout *hBoxLayout;
            QVBoxLayout *vBoxLayout;
            QVGroupBox *mainGroup;
            QPushButton *openDefaultButton;
            QPushButton *openNewButton;
            QPushButton *createNewButton;
            QVGroupBox *otherGroup;
            QPushButton *pathButton;
            //QPushButton *importButton;
            QPushButton *quitButton;
        } main;
            
        struct {
            QPushButton *focusedButton;
            QWidget *widget;
            QVBoxLayout *vBoxLabel;
            QHBoxLayout *hBoxLabel;
            QPushButton *closeButton;
            QHBoxLayout *hBoxLayout;
            QVBoxLayout *vBoxLayout;
            QVGroupBox *clientGroup;
            QPushButton *accountsButton;
            QPushButton *journalButton;
            QPushButton *reportButton;
            QVGroupBox *adminGroup;
            QPushButton *editNameButton;
            //QPushButton *cryptButton;
        } tasks;
};


#endif

