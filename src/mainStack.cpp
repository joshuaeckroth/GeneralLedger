#include <typeinfo>

#include <qwidgetstack.h>
#include <qlayout.h>
#include <qvgroupbox.h>
#include <qpushbutton.h>
#include <qiconset.h>
#include <qpixmap.h>
#include <qdialog.h>
#include <qinputdialog.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qfiledialog.h>
#include <qevent.h>
#include <qregexp.h>
#include <qfile.h>

#include "database.h"
#include "settings.h"
#include "mainStack.h"

MainStack::MainStack(QWidget *parent, const char *name)
    : QWidgetStack(parent,name)
{
    db = Database::instance();
    settings = Settings::instance();

    iconPath = settings->getIconPath();
    exportPath = settings->getExportPath();
    importPath = settings->getImportPath();
    
    main.widget = new QWidget(this);
    
    main.hBoxLayout = new QHBoxLayout(main.widget);
    
    main.hBoxLayout->addStretch();
    
    main.vBoxLayout = new QVBoxLayout(main.hBoxLayout);
    
    main.vBoxLayout->addStretch();
    
    main.mainGroup = new QVGroupBox("Main Options", main.widget);
    main.mainGroup->setInsideMargin(30);
    main.mainGroup->setInsideSpacing(20);
    
    main.vBoxLayout->add(main.mainGroup);
    
    if(db->getCurClient() != "" && db->curClientExists())
    {
        main.openDefaultButton = new QPushButton(
                QIconSet( QPixmap::fromMimeSource(iconPath + "/openDefButton.png") ),
                "Open " + db->getCurClient(), main.mainGroup);
        connect(main.openDefaultButton, SIGNAL(clicked()), this, SLOT(openDefault()));
        main.openDefaultButton->setDefault(true);
        main.openDefaultButton->setFocus();
        main.openDefaultButton->installEventFilter(this);
    }
    else
    {
        main.openDefaultButton = new QPushButton(
                QIconSet( QPixmap::fromMimeSource(iconPath + "/openDefButton.png") ),
                "Open ...", main.mainGroup);
        main.openDefaultButton->setEnabled(false);
    }
    
    main.openNewButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(iconPath + "/openNewButton.png") ),
            "Open a Different Client", main.mainGroup);
    connect(main.openNewButton, SIGNAL(clicked()), this, SLOT(openNewDialog()));
    main.openNewButton->installEventFilter(this);
    
    main.createNewButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(iconPath + "/createNewButton.png") ),
            "Create a New Client", main.mainGroup);
    connect(main.createNewButton, SIGNAL(clicked()), this, SLOT(createNewDialog()));
    main.createNewButton->installEventFilter(this);
    
    main.vBoxLayout->addSpacing(30);
    
    main.otherGroup = new QVGroupBox("Other Options", main.widget);
    main.otherGroup->setInsideMargin(30);
    main.otherGroup->setInsideSpacing(20);
    
    main.vBoxLayout->add(main.otherGroup);
    
    main.importButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(iconPath + "/importButton.png") ),
            "Import a Database File", main.otherGroup);
    connect(main.importButton, SIGNAL(clicked()), this, SLOT(import()));
    main.importButton->installEventFilter(this);
    
    main.quitButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(iconPath + "/quitButton.png") ),
            "Quit General Ledger", main.otherGroup);
    connect(main.quitButton, SIGNAL(clicked()), this, SLOT(prepareQuit()));
    main.quitButton->installEventFilter(this);
    
    main.vBoxLayout->addStretch();
    
    main.hBoxLayout->addStretch();
    
    addWidget(main.widget, 0);
    
    active = 0;
}

MainStack::~MainStack()
{}

void MainStack::clientOpened()
{
    main.openDefaultButton->setText("Open " + db->getCurClient());
    main.openDefaultButton->setEnabled(true);
    main.openDefaultButton->installEventFilter(this);
    connect(main.openDefaultButton, SIGNAL(clicked()), this, SLOT(openDefault()));
    
    if(widget(1))
    {
        removeWidget(tasks.widget);
        delete tasks.cryptButton;
        delete tasks.copyDatabaseButton;
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
    
    tasks.widget = new QWidget(this);
    
    tasks.vBoxLabel = new QVBoxLayout(tasks.widget);
    
    tasks.hBoxLabel = new QHBoxLayout(tasks.vBoxLabel);
    tasks.hBoxLabel->setMargin(5);
    
    tasks.closeButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(iconPath + "/closeClient.png") ),
            "Close Client (F8)", tasks.widget);
    tasks.closeButton->setFocusPolicy(QWidget::NoFocus);
    connect(tasks.closeButton, SIGNAL(clicked()), this, SLOT(closeClient()));
    
    tasks.hBoxLabel->addStretch();
    tasks.hBoxLabel->addWidget(tasks.closeButton);
    
    tasks.hBoxLayout = new QHBoxLayout(tasks.vBoxLabel);
    
    tasks.hBoxLayout->addStretch();
    
    tasks.vBoxLayout = new QVBoxLayout(tasks.hBoxLayout);
    
    tasks.vBoxLayout->addStretch();
    
    tasks.clientGroup = new QVGroupBox("Client Tasks", tasks.widget);
    tasks.clientGroup->setInsideMargin(30);
    tasks.clientGroup->setInsideSpacing(20);
    
    tasks.vBoxLayout->add(tasks.clientGroup);
    
    tasks.accountsButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(iconPath + "/accountsTab.png") ),
            "Accounts", tasks.clientGroup);
    connect(tasks.accountsButton, SIGNAL(clicked()), this, SIGNAL(switchToAccounts()));
    tasks.accountsButton->setDefault(true);
    tasks.accountsButton->setFocus();
    tasks.focusedButton = tasks.accountsButton;
    tasks.accountsButton->installEventFilter(this);
    
    tasks.journalButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(iconPath + "/journalTab.png") ),
            "Journal Entry", tasks.clientGroup);
    connect(tasks.journalButton, SIGNAL(clicked()), this, SIGNAL(switchToJournal()));
    tasks.journalButton->installEventFilter(this);
    
    tasks.reportButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(iconPath + "/reportTab.png") ),
            "Reports", tasks.clientGroup);
    connect(tasks.reportButton, SIGNAL(clicked()), this, SIGNAL(switchToReports()));
    tasks.reportButton->installEventFilter(this);
    
    tasks.vBoxLayout->addSpacing(30);
    
    tasks.adminGroup = new QVGroupBox("Administrative Options", tasks.widget);
    tasks.adminGroup->setInsideMargin(30);
    tasks.adminGroup->setInsideSpacing(20);
    
    tasks.vBoxLayout->add(tasks.adminGroup);
    
    tasks.editNameButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(iconPath + "/editNameButton.png") ),
            "Edit Client Name", tasks.adminGroup);
    connect(tasks.editNameButton, SIGNAL(clicked()), this, SLOT(editName()));
    tasks.editNameButton->installEventFilter(this);
    
    tasks.copyDatabaseButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(iconPath + "/copyDatabaseButton.png") ),
            "Copy Client Database to File", tasks.adminGroup);
    connect(tasks.copyDatabaseButton, SIGNAL(clicked()), this, SLOT(copyClient()));
    tasks.copyDatabaseButton->installEventFilter(this);
    
    tasks.cryptButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(iconPath + "/encryptButton.png") ),
            "Encrypt Database with Password", tasks.adminGroup);
    connect(tasks.cryptButton, SIGNAL(clicked()), this, SLOT(encrypt()));
    tasks.cryptButton->installEventFilter(this);
    
    tasks.vBoxLayout->addStretch();
    
    tasks.hBoxLayout->addStretch();
    
    addWidget(tasks.widget, 1);
        
    switchWidget();
}

void MainStack::switchWidget()
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

bool MainStack::eventFilter(QObject *target, QEvent *event)
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
        if(typeid(*target) == typeid(QPushButton))
        {
            ((QPushButton*)target)->setDefault(true);
            if(active == 1)
                tasks.focusedButton = (QPushButton*)target;
        }
    }
    if(event->type() == QEvent::FocusOut)
    {
        if(typeid(*target) == typeid(QPushButton))
            ((QPushButton*)target)->setDefault(false);
    }
    if(event->type() == QEvent::Show)
    {
        if(active == 1)
            tasks.focusedButton->setFocus();
    }
    
    return QWidgetStack::eventFilter(target,event);
}

void MainStack::openDefault()
{
    QString file = db->getCurDb();

    bool encrypted = false;

    if(file.right(4) == ".dbx")
    {
        file.remove(".dbx");
        encrypted = true;
    }
    if(file.right(3) == ".db")
        file.remove(".db");

    QString password(QString::null);

    if(encrypted)
    {
        QDialog dialog2;
        dialog2.setCaption("Enter Password");
    
        QVBoxLayout vBoxLayout2(&dialog2);
        vBoxLayout2.setMargin(5);
        vBoxLayout2.setSpacing(5);
    
        QLabel label2("<nobr>This database is encrypted</nobr><br><nobr>Enter the password:</nobr>", &dialog2);
        QLineEdit password2(&dialog2);
        password2.setMaxLength(20);
        password2.setEchoMode(QLineEdit::Password);
        vBoxLayout2.addStretch();
        vBoxLayout2.addWidget(&label2);
        vBoxLayout2.addWidget(&password2);
    
        QHBoxLayout hBoxLayout2(&vBoxLayout2);
        hBoxLayout2.setSpacing(5);
    
        QPushButton decryptButton2(QIconSet( QPixmap::fromMimeSource(iconPath + "/decryptButton.png") ),
                                   "Open", &dialog2);
        connect(&decryptButton2, SIGNAL(clicked()), &dialog2, SLOT(accept()));
    
        QPushButton cancelButton2(QIconSet( QPixmap::fromMimeSource(iconPath + "/cancelButton.png") ),
                                  "Cancel", &dialog2);
        connect(&cancelButton2, SIGNAL(clicked()), &dialog2, SLOT(reject()));
    
        hBoxLayout2.addStretch();
        hBoxLayout2.addWidget(&decryptButton2);
        hBoxLayout2.addWidget(&cancelButton2);

        if(dialog2.exec())
            password = password2.text();
    }

    if(db->openNew(file, password))
    {
        clientOpened();
        emit dbOpened();
    }
}

void MainStack::openNewDialog()
{
    QDialog dialog;
    dialog.setCaption("Open Client");
    
    QVBoxLayout vBoxLayout(&dialog);
    vBoxLayout.setMargin(5);
    vBoxLayout.setSpacing(5);
    
    QLabel label("Choose a Client:", &dialog);
    vBoxLayout.addWidget(&label);
    
    QListBox listBox(&dialog);
    listBox.setFocus();
    listBox.insertStringList(db->getClientList());
    listBox.setSelected(0, true);
    connect(&listBox, SIGNAL(selected(QListBoxItem*)), &dialog, SLOT(accept()));
    
    vBoxLayout.addWidget(&listBox);
    
    QHBoxLayout hBoxLayout(&vBoxLayout);
    hBoxLayout.setSpacing(5);
    
    QPushButton okButton(QIconSet(QPixmap::fromMimeSource(iconPath + "/openNewButton.png")), "Open", &dialog);
    okButton.setDefault(true);
    connect(&okButton, SIGNAL(clicked()), &dialog, SLOT(accept()));
    
    QPushButton cancelButton(QIconSet(QPixmap::fromMimeSource(iconPath + "/cancelButton.png")), "Cancel", &dialog);
    connect(&cancelButton, SIGNAL(clicked()), &dialog, SLOT(reject()));
    
    hBoxLayout.addWidget(&okButton);
    hBoxLayout.addWidget(&cancelButton);
    
    if(dialog.exec())
    {
        QString file = listBox.selectedItem()->text();
        file.remove(QRegExp(".+\\("));

        bool encrypted = false;

        if(file.right(5) == ".dbx)")
        {
            file.remove(".dbx)");
            encrypted = true;
        }
        if(file.right(4) == ".db)")
            file.remove(".db)");

        QString password(QString::null);

        if(encrypted)
        {
            QDialog dialog2;
            dialog2.setCaption("Enter Password");
    
            QVBoxLayout vBoxLayout2(&dialog2);
            vBoxLayout2.setMargin(5);
            vBoxLayout2.setSpacing(5);
    
            QLabel label2("<nobr>This database is encrypted</nobr><br><nobr>Enter the password:</nobr>", &dialog2);
            QLineEdit password2(&dialog2);
            password2.setMaxLength(20);
            password2.setEchoMode(QLineEdit::Password);
            vBoxLayout2.addStretch();
            vBoxLayout2.addWidget(&label2);
            vBoxLayout2.addWidget(&password2);
    
            QHBoxLayout hBoxLayout2(&vBoxLayout2);
            hBoxLayout2.setSpacing(5);
    
            QPushButton decryptButton2(QIconSet( QPixmap::fromMimeSource(iconPath + "/decryptButton.png") ),
                                      "Open", &dialog2);
            connect(&decryptButton2, SIGNAL(clicked()), &dialog2, SLOT(accept()));
    
            QPushButton cancelButton2(QIconSet( QPixmap::fromMimeSource(iconPath + "/cancelButton.png") ),
                                     "Cancel", &dialog2);
            connect(&cancelButton2, SIGNAL(clicked()), &dialog2, SLOT(reject()));
    
            hBoxLayout2.addStretch();
            hBoxLayout2.addWidget(&decryptButton2);
            hBoxLayout2.addWidget(&cancelButton2);

            if(dialog2.exec())
                password = password2.text();
        }

        if(db->openNew(file, password))
        {
            clientOpened();
            emit dbOpened();
        }
    }
}

void MainStack::createNewDialog()
{
    QDialog dialog;
    dialog.setCaption("Create a New Client");
    
    QVBoxLayout vBoxLayout(&dialog);
    vBoxLayout.setMargin(5);
    vBoxLayout.setSpacing(5);
    
    QLabel labelName("Input the Client's Name:", &dialog);
    vBoxLayout.addWidget(&labelName);
    
    QLineEdit lineEdit(&dialog);
    lineEdit.setMaxLength(50);
    lineEdit.setFocus();
    vBoxLayout.addWidget(&lineEdit);
    
    QHBoxLayout hBoxYearEnd(&vBoxLayout);
    hBoxYearEnd.setSpacing(5);
    
    QLabel yearEndLabel("Year-End Date:", &dialog);
    QComboBox yearEndList(false, &dialog);
    QStringList yearEndStrings;
    yearEndStrings << "January 31" << "February 28" << "March 31" << "April 30"
            << "May 31" << "June 30" << "July 31" << "August 31" << "September 30"
            << "October 31" << "November 30" << "December 31";
    yearEndList.insertStringList(yearEndStrings);
    
    hBoxYearEnd.addWidget(&yearEndLabel);
    hBoxYearEnd.addWidget(&yearEndList);
    
    QHBoxLayout hBoxButtons(&vBoxLayout);
    hBoxButtons.setSpacing(5);
    
    QPushButton createButton(QIconSet(QPixmap::fromMimeSource(iconPath + "/createNewButton.png")), "Create", &dialog);
    createButton.setDefault(true);
    connect(&createButton, SIGNAL(clicked()), &dialog, SLOT(accept()));
    
    QPushButton cancelButton(QIconSet(QPixmap::fromMimeSource(iconPath + "/cancelButton.png")), "Cancel", &dialog);
    connect(&cancelButton, SIGNAL(clicked()), &dialog, SLOT(reject()));
    
    hBoxButtons.addStretch();
    hBoxButtons.addWidget(&createButton);
    hBoxButtons.addWidget(&cancelButton);
    
    if(dialog.exec() && !lineEdit.text().isEmpty())
    {
        if(db->createNew(lineEdit.text(), yearEndList.currentText()))
        {
            clientOpened();
            emit dbOpened();
        }
    }
}

void MainStack::editName()
{
    bool ok;
    QString name = QInputDialog::getText("Edit Client Name",
                                         "Enter a new Client Name:",
                                         QLineEdit::Normal,
                                         QString::null, &ok, this);
    if( ok && !name.isEmpty() )
        db->editName(name);
    main.openDefaultButton->setText("Open " + db->getCurClient());
    emit nameChanged();
}

void MainStack::copyClient()
{
    QString file = QFileDialog::getSaveFileName(
            exportPath,
            "Client Database (*.db *.dbx)",
            this,
            0,
            "Copy Client Database to File");
    
    if(!file.isEmpty())
        db->copyDb(file);
}

void MainStack::import()
{
    QString file = QFileDialog::getOpenFileName(
            importPath,
            "Client Database (*.db *.dbx)",
            this,
            0,
            "Import a Database File");

    if(!file.isEmpty())
        db->importDb(file);
}

void MainStack::encrypt()
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
    
    QLabel label2("Repeat:", &dialog);
    QLineEdit password2(&dialog);
    password2.setMaxLength(20);
    password2.setEchoMode(QLineEdit::Password);
    hBoxMiddle.addStretch();
    hBoxMiddle.addWidget(&label2);
    hBoxMiddle.addWidget(&password2);
    
    QHBoxLayout hBoxBottom(&vBoxLayout);
    hBoxBottom.setSpacing(5);
    
    QPushButton encryptButton(QIconSet( QPixmap::fromMimeSource(iconPath + "/encryptButton.png") ),
                              "Encrypt", &dialog);
    connect(&encryptButton, SIGNAL(clicked()), &dialog, SLOT(accept()));
    
    QPushButton cancelButton(QIconSet( QPixmap::fromMimeSource(iconPath + "/cancelButton.png") ),
                             "Cancel", &dialog);
    connect(&cancelButton, SIGNAL(clicked()), &dialog, SLOT(reject()));
    
    hBoxBottom.addStretch();
    hBoxBottom.addWidget(&encryptButton);
    hBoxBottom.addWidget(&cancelButton);
    
    if(dialog.exec())
    {
        if(password1.text() == password2.text())
           db->encryptDb(password1.text());
    }
}

void MainStack::closeClient()
{
    db->closeDb();
    switchWidget();
    emit dbClosed();
}

void MainStack::prepareQuit()
{
    if(active == 1)
        closeClient();
    emit quit();
}

