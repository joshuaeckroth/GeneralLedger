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

#include <qwidgetstack.h>
#include <qlayout.h>
#include <qvgroupbox.h>
#include <qpushbutton.h>
#include <qiconset.h>
#include <qpixmap.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qfiledialog.h>
#include <qevent.h>
#include <qregexp.h>
#include <qfile.h>
#include <qregexp.h>
#include <qvalidator.h>

#include "homeTab.h"
#include "config.h"
#include "client.h"
#include "settings.h"
#include "journalData.h"
#include "globalCloseDialog.h"

HomeTab::HomeTab(QWidget *parent, const char *name)
    : QWidgetStack(parent,name)
{
    client = Client::instance();
    connect(client, SIGNAL(clientOpened(QString)), this, SLOT(clientOpened(QString)));

    settings = Settings::instance();

    journalData = JournalData::instance();

    main.widget = new QWidget(this, "homeTab::main.widget");
    
    main.hBoxLayout = new QHBoxLayout(main.widget, 0, -1, "homeTab::main.hBoxLayout");
    
    main.hBoxLayout->addStretch();
    
    main.vBoxLayout = new QVBoxLayout(main.hBoxLayout, -1, "homeTab::main.vBoxLayout");
    
    main.vBoxLayout->addStretch();
    
    main.mainGroup = new QVGroupBox("Main Options", main.widget, "homeTab::main.mainGroup");
    main.mainGroup->setInsideMargin(30);
    main.mainGroup->setInsideSpacing(20);
    
    main.vBoxLayout->add(main.mainGroup);

    bool openDefaultReady = (client->getClientName() != "" && client->defaultClientExists());
    
    if(openDefaultReady)
    {
        main.openDefaultButton = new QPushButton(
                QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/openDefButton.png") ),
                "Open " + client->getClientName(), main.mainGroup, "homeTab::main.openDefaultButton");
        connect(main.openDefaultButton, SIGNAL(clicked()), this, SLOT(openDefault()));
        main.openDefaultButton->installEventFilter(this);
    }
    else
    {
        main.openDefaultButton = new QPushButton(
                QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/openDefButton.png") ),
                "Open ...", main.mainGroup, "homeTab::main.openDefaultButton");
        main.openDefaultButton->setEnabled(false);
    }
    
    main.openNewButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/openNewButton.png") ),
            "Open a Different Client", main.mainGroup, "homeTab::main.openNewButton");
    connect(main.openNewButton, SIGNAL(clicked()), this, SLOT(openNewDialog()));
    main.openNewButton->installEventFilter(this);
    
    main.createNewButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/createNewButton.png") ),
            "Create a New Client", main.mainGroup, "homeTab::main.createNewButton");
    connect(main.createNewButton, SIGNAL(clicked()), this, SLOT(createNewDialog()));
    main.createNewButton->installEventFilter(this);
    
    main.vBoxLayout->addSpacing(30);
    
    main.otherGroup = new QVGroupBox("Other Options", main.widget, "homeTab::main.otherGroup");
    main.otherGroup->setInsideMargin(30);
    main.otherGroup->setInsideSpacing(20);
    
    main.vBoxLayout->add(main.otherGroup);
    
    main.pathButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/pathButton.png") ),
            "Change Client Folder", main.otherGroup, "homeTab::main.pathButton");
    connect(main.pathButton, SIGNAL(clicked()), this, SLOT(changePath()));
    main.pathButton->installEventFilter(this);

    main.quitButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/quitButton.png") ),
            "Quit General Ledger", main.otherGroup, "homeTab::main.quitButton");
    connect(main.quitButton, SIGNAL(clicked()), this, SLOT(prepareQuit()));
    main.quitButton->installEventFilter(this);
    
    main.vBoxLayout->addStretch();
    
    main.hBoxLayout->addStretch();
    
    addWidget(main.widget, 0);

    if(openDefaultReady)
        main.openDefaultButton->setDefault(true);
    else
        main.openNewButton->setDefault(true);
 
    active = 0;
}

void HomeTab::clientOpened(QString clientName)
{
    main.openDefaultButton->setText("Open " + clientName);
    main.openDefaultButton->setEnabled(true);
    main.openDefaultButton->installEventFilter(this);
    disconnect(main.openDefaultButton, SIGNAL(clicked()), this, SLOT(openDefault()));
    connect(main.openDefaultButton, SIGNAL(clicked()), this, SLOT(openDefault()));
    
    active = 0;

    if(widget(1))
    {
        removeWidget(tasks.widget);
        //delete tasks.cryptButton;
        delete tasks.editNameButton;
        delete tasks.adminGroup;
        delete tasks.reportButton;
        delete tasks.journalButton;
        delete tasks.accountsButton;
        delete tasks.clientGroup;
        delete tasks.vBoxLayout;
        delete tasks.hBoxLayout;
        delete tasks.closeButton;
        delete tasks.hBoxLabel;
        delete tasks.vBoxLabel;
        delete tasks.widget;
    }
    
    tasks.widget = new QWidget(this, "homeTab::tasks.widget");
    
    tasks.vBoxLabel = new QVBoxLayout(tasks.widget, 0, -1, "homeTab::tasks.vBoxLabel");
    
    tasks.hBoxLabel = new QHBoxLayout(tasks.vBoxLabel, -1, "homeTab::tasks.hBoxLabel");
    tasks.hBoxLabel->setMargin(5);
    
    tasks.closeButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/closeClient.png") ),
            "Close Client (F8)", tasks.widget, "homeTab::tasks.closeButton");
    tasks.closeButton->setFocusPolicy(QWidget::NoFocus);
    connect(tasks.closeButton, SIGNAL(clicked()), this, SLOT(closeClient()));
    
    tasks.hBoxLabel->addStretch();
    tasks.hBoxLabel->addWidget(tasks.closeButton);
    
    tasks.hBoxLayout = new QHBoxLayout(tasks.vBoxLabel, -1, "homeTab::tasks.hBoxLayout");
    
    tasks.hBoxLayout->addStretch();
    
    tasks.vBoxLayout = new QVBoxLayout(tasks.hBoxLayout, -1, "homeTab::tasks.vBoxLayout");
    
    tasks.vBoxLayout->addStretch();
    
    tasks.clientGroup = new QVGroupBox("Client Tasks", tasks.widget, "homeTab::tasks.clientGroup");
    tasks.clientGroup->setInsideMargin(30);
    tasks.clientGroup->setInsideSpacing(20);
    
    tasks.vBoxLayout->add(tasks.clientGroup);
    
    tasks.accountsButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/accountsTab.png") ),
            "Accounts", tasks.clientGroup, "homeTab::tasks.accountsButton");
    connect(tasks.accountsButton, SIGNAL(clicked()), this, SIGNAL(switchToAccounts()));
    tasks.accountsButton->setDefault(true);
    tasks.accountsButton->setFocus();
    tasks.focusedButton = tasks.accountsButton;
    tasks.accountsButton->installEventFilter(this);
    
    tasks.journalButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/journalTab.png") ),
            "Journal Entry", tasks.clientGroup, "homeTab::tasks.journalButton");
    connect(tasks.journalButton, SIGNAL(clicked()), this, SIGNAL(switchToJournal()));
    tasks.journalButton->installEventFilter(this);
    
    tasks.reportButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/reportsTab.png") ),
            "Reports", tasks.clientGroup, "homeTab::tasks.reportButton");
    connect(tasks.reportButton, SIGNAL(clicked()), this, SIGNAL(switchToReports()));
    tasks.reportButton->installEventFilter(this);
    
    tasks.vBoxLayout->addSpacing(30);
    
    tasks.adminGroup = new QVGroupBox("Administrative Options", tasks.widget, "homeTab::tasks.adminGroup");
    tasks.adminGroup->setInsideMargin(30);
    tasks.adminGroup->setInsideSpacing(20);
    
    tasks.vBoxLayout->add(tasks.adminGroup);
    
    tasks.editNameButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/editNameButton.png") ),
            "Edit Client Name", tasks.adminGroup, "homeTab::tasks.editNameButton");
    connect(tasks.editNameButton, SIGNAL(clicked()), this, SLOT(editName()));
    tasks.editNameButton->installEventFilter(this);
    
    /*
    if(db->isEncrypted())
    {
        tasks.cryptButton = new QPushButton(
                QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/decryptButton.png") ),
                "Decrypt Database", tasks.adminGroup, "homeTab::tasks.cryptButton");
        connect(tasks.cryptButton, SIGNAL(clicked()), this, SLOT(decrypt()));
    }
    else
    {
        tasks.cryptButton = new QPushButton(
                QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/encryptButton.png") ),
                "Encrypt Database with Password", tasks.adminGroup, "homeTab::tasks.cryptButton");
        connect(tasks.cryptButton, SIGNAL(clicked()), this, SLOT(encrypt()));
    }
    tasks.cryptButton->installEventFilter(this);
    */
    
    tasks.vBoxLayout->addStretch();
    
    tasks.hBoxLayout->addStretch();
    
    addWidget(tasks.widget, 1);
        
    switchWidget();
}

void HomeTab::switchWidget()
{
    if(active == 0)
    {
        raiseWidget(1);
        active = 1;
        tasks.focusedButton->setFocus();
    }
    else
    {
        raiseWidget(0);
        active = 0;
        main.openDefaultButton->setFocus();
    }
}

bool HomeTab::eventFilter(QObject *target, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = (QKeyEvent *)event;
        if(keyEvent->key() == Key_F8 && active == 1)
        {
            closeClient();
            return true;
        }
    }
    if(event->type() == QEvent::FocusIn)
    {
        ((QPushButton*)target)->setDefault(true);
        if(active == 1)
            tasks.focusedButton = (QPushButton*)target;
    }
    if(event->type() == QEvent::FocusOut)
        ((QPushButton*)target)->setDefault(false);
    
    if(event->type() == QEvent::Show)
    {
        if(active == 1)
            tasks.focusedButton->setFocus();
        else
        {
            if(client->getClientName() != "" && client->defaultClientExists())
                main.openDefaultButton->setFocus();
            else
                main.openNewButton->setFocus();
        }
    }
    
    return QWidgetStack::eventFilter(target,event);
}

void HomeTab::openDefault()
{
    client->open(client->defaultClientId());
}

void HomeTab::openNewDialog()
{
    QDialog *dialog = new QDialog;
    dialog->setCaption("Open Client");
    
    QVBoxLayout *vBoxLayout = new QVBoxLayout(dialog);
    vBoxLayout->setMargin(5);
    vBoxLayout->setSpacing(5);
    
    QLabel *label = new QLabel("Choose a Client:", dialog);
    vBoxLayout->addWidget(label);
    
    QListBox *listBox = new QListBox(dialog);
    listBox->setFocus();
    listBox->insertStringList(client->getAllClients());
    listBox->setSelected(0, true);
    connect(listBox, SIGNAL(selected(QListBoxItem*)), dialog, SLOT(accept()));
    
    vBoxLayout->addWidget(listBox);
    vBoxLayout->addSpacing(10);
    
    QHBoxLayout *hBoxLayout = new QHBoxLayout(vBoxLayout);
    hBoxLayout->setSpacing(5);
   
    QPushButton *okButton = new QPushButton(QIconSet(
                    QPixmap::fromMimeSource(ICON_PATH + "/openNewButton.png")),
                    "Open", dialog, "homeTab::openNewDialog()::okButton");
    okButton->setDefault(true);
    connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    
    QPushButton *cancelButton = new QPushButton(QIconSet(
                    QPixmap::fromMimeSource(ICON_PATH + "/cancelButton.png")),
                    "Cancel", dialog);
    connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));
    
    hBoxLayout->addWidget(okButton);
    hBoxLayout->addWidget(cancelButton);

    if(dialog->exec() && listBox->count())
    {
        QString id = listBox->selectedItem()->text();
        id.remove(QRegExp(".+\\("));
        id.remove(QRegExp("\\)$"));
        id = id.left(id.length() - 4);  // take out ".glc"

        client->open(id);
    }

    delete dialog;
}

void HomeTab::createNewDialog()
{
    QDialog *dialog = new QDialog;
    dialog->setCaption("Create a New Client");
    
    QVBoxLayout *vBoxLayout = new QVBoxLayout(dialog);
    vBoxLayout->setMargin(5);
    vBoxLayout->setSpacing(5);
    
    QLabel *labelName = new QLabel("Input the Client's Name:", dialog);
    vBoxLayout->addWidget(labelName);
    
    QLineEdit *clientNameLineEdit = new QLineEdit(dialog);
    clientNameLineEdit->setMaxLength(50);
    clientNameLineEdit->setFocus();
    vBoxLayout->addWidget(clientNameLineEdit);
    
    QHBoxLayout *hBoxYearEnd = new QHBoxLayout(vBoxLayout);
    hBoxYearEnd->setSpacing(5);

    QLabel *yearEndLabel = new QLabel("Year-End Date:", dialog);
    QComboBox *yearEndList = new QComboBox(false, dialog);
    QStringList yearEndStrings;
    yearEndStrings << "January 31" << "February 28" << "March 31" << "April 30"
            << "May 31" << "June 30" << "July 31" << "August 31" << "September 30"
            << "October 31" << "November 30" << "December 31";
    yearEndList->insertStringList(yearEndStrings);
    
    hBoxYearEnd->addWidget(yearEndLabel);
    hBoxYearEnd->addWidget(yearEndList);

    QHBoxLayout *hBoxREAccount = new QHBoxLayout(vBoxLayout);
    hBoxREAccount->setSpacing(5);

    QLabel *REAccountLabel = new QLabel("Retained Earnings Account Id:", dialog);
    QLineEdit *REAccountLineEdit = new QLineEdit(dialog);
    REAccountLineEdit->setMaxLength(50);
    REAccountLineEdit->setValidator(new QRegExpValidator(QRegExp("\\d+-?\\d*"),
                            REAccountLineEdit));
    
    hBoxREAccount->addWidget(REAccountLabel);
    hBoxREAccount->addWidget(REAccountLineEdit);

    vBoxLayout->addSpacing(10);
    
    QHBoxLayout *hBoxButtons = new QHBoxLayout(vBoxLayout);
    hBoxButtons->setSpacing(5);
    
    QPushButton *createButton = new QPushButton(QIconSet(
                    QPixmap::fromMimeSource(ICON_PATH + "/createNewButton.png")),
                    "Create", dialog);
    createButton->setDefault(true);
    connect(createButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    
    QPushButton *cancelButton = new QPushButton(QIconSet(
                    QPixmap::fromMimeSource(ICON_PATH + "/cancelButton.png")),
                    "Cancel", dialog);
    connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));
    
    hBoxButtons->addStretch();
    hBoxButtons->addWidget(createButton);
    hBoxButtons->addWidget(cancelButton);
   
    /// \todo produce error concerning missing data 
    if(dialog->exec() && !clientNameLineEdit->text().isEmpty()
                      && !REAccountLineEdit->text().isEmpty())
    {
        client->create(clientNameLineEdit->text(), 
                       (yearEndList->currentItem() + 1),
                       REAccountLineEdit->text());
    }

    delete dialog;
}

void HomeTab::editName()
{
    QDialog *dialog = new QDialog;
    dialog->setCaption("Edit Client Name");

    QVBoxLayout *vBoxLayout = new QVBoxLayout(dialog);
    vBoxLayout->setMargin(5);
    vBoxLayout->setSpacing(5);

    QLabel *label = new QLabel("Input a new client name:", dialog);

    vBoxLayout->addWidget(label);

    QLineEdit *lineEdit = new QLineEdit(client->getClientName(), dialog);
    lineEdit->setMaxLength(50);
    lineEdit->setFocus();

    vBoxLayout->addWidget(lineEdit);
    vBoxLayout->addSpacing(10);

    QHBoxLayout *hBoxLayout = new QHBoxLayout(vBoxLayout);

    QPushButton *okButton = new QPushButton(QIconSet(
                         QPixmap::fromMimeSource(ICON_PATH + "/editNameButton.png")),
                         "Change Name", dialog);
    connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));

    QPushButton *cancelButton = new QPushButton(QIconSet(
                             QPixmap::fromMimeSource(ICON_PATH + "/cancelButton.png")),
                             "Cancel", dialog);
    connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));

    cancelButton->setDefault(false);
    okButton->setDefault(true);

    hBoxLayout->addStretch();
    hBoxLayout->addWidget(okButton);
    hBoxLayout->addWidget(cancelButton);

    if(dialog->exec() && !(lineEdit->text().isEmpty()))
    {
        client->setClientName(lineEdit->text());
        main.openDefaultButton->setText("Open " + lineEdit->text());
    }

    delete dialog;
}

void HomeTab::changePath()
{
    QString path = QFileDialog::getExistingDirectory(
            settings->getClientPath(),
            this,
            0,
            "Choose Client Database Folder");

    if(!path.isEmpty())
    {
        settings->setClientPath(path);
        main.openDefaultButton->setText("Open...");
        main.openDefaultButton->setEnabled(false);
        main.openDefaultButton->removeEventFilter(this);
    }
}

/*
void HomeTab::encrypt()
{
    QDialog dialog;
    dialog.setCaption("Encrypt Database");
    
    QVBoxLayout vBoxLayout(&dialog);
    vBoxLayout.setMargin(5);
    vBoxLayout.setSpacing(5);
    
    QHBoxLayout hBoxTop(&vBoxLayout);
    hBoxTop.setSpacing(5);
    
    QLabel label1("Input a password:", &dialog);
    QLineEdit password1(&dialog);
    password1.setMaxLength(20);
    password1.setEchoMode(QLineEdit::Password);
    hBoxTop.addStretch();
    hBoxTop.addWidget(&label1);
    hBoxTop.addWidget(&password1);
    
    QHBoxLayout hBoxMiddle(&vBoxLayout);
    hBoxMiddle.setSpacing(5);

    vBoxLayout.addSpacing(10);
    
    QLabel label2("Repeat:", &dialog);
    QLineEdit password2(&dialog);
    password2.setMaxLength(20);
    password2.setEchoMode(QLineEdit::Password);
    hBoxMiddle.addStretch();
    hBoxMiddle.addWidget(&label2);
    hBoxMiddle.addWidget(&password2);
    
    QHBoxLayout hBoxBottom(&vBoxLayout);
    hBoxBottom.setSpacing(5);
    
    QPushButton encryptButton(QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/encryptButton.png") ),
                              "Encrypt", &dialog);
    connect(&encryptButton, SIGNAL(clicked()), &dialog, SLOT(accept()));
    
    QPushButton cancelButton(QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/cancelButton.png") ),
                             "Cancel", &dialog);
    connect(&cancelButton, SIGNAL(clicked()), &dialog, SLOT(reject()));
    
    hBoxBottom.addStretch();
    hBoxBottom.addWidget(&encryptButton);
    hBoxBottom.addWidget(&cancelButton);
    
    while(dialog.exec())
    {
        if(password1.text() == password2.text())
        {
            db->encryptDb(password1.text());
            clientOpened();
            break;
        }
    }
}

void HomeTab::decrypt()
{
    QDialog dialog;
    dialog.setCaption("Decrypt Database");

    QVBoxLayout vBoxLayout(&dialog);
    vBoxLayout.setMargin(5);
    vBoxLayout.setSpacing(5);

    QLabel label("<nobr>Are you sure you want to decrypt the database?</nobr>", &dialog);

    vBoxLayout.addWidget(&label);
    vBoxLayout.addSpacing(10);

    QHBoxLayout hBoxLayout(&vBoxLayout);
    hBoxLayout.setSpacing(5);

    QPushButton decryptButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/decryptButton.png")),
                              "Decrypt", &dialog);
    connect(&decryptButton, SIGNAL(clicked()), &dialog, SLOT(accept()));
    decryptButton.setDefault(false);

    QPushButton cancelButton(QIconSet(QPixmap::fromMimeSource(QString(ICON_PATH + "/cancelButton.png"))),
                             "Cancel", &dialog);
    connect(&cancelButton, SIGNAL(clicked()), &dialog, SLOT(reject()));
    cancelButton.setDefault(true);

    hBoxLayout.addStretch();
    hBoxLayout.addWidget(&decryptButton);
    hBoxLayout.addWidget(&cancelButton);

    if(dialog.exec())
    {
        db->decryptDb();
        clientOpened();
    }
}

*/

void HomeTab::closeClient()
{
    if(active == 0)
        return;

    if(!(journalData->journalTmpEmpty()))
    {
        GlobalCloseDialog dialog(this, "homeTab::closeClient()::dialog");
        dialog.exec();

        switch(dialog.result())
        {
            case 0:
                journalData->saveAndClear();
                break;
            case 1:
                break;
            case 2:
                topLevelWidget()->showNormal();
                emit switchToJournal();
                return;
                break;
            default:
                break;
        }
    }

    client->close();
    switchWidget();

    return;
}

void HomeTab::prepareQuit()
{
    if(active == 1)
        closeClient();
    emit quit();
}

