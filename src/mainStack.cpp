#include <vector>
using std::vector;
#include <typeinfo>

#include <qwidgetstack.h>
#include <qlayout.h>
#include <qvgroupbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qiconset.h>
#include <qpixmap.h>
#include <qdialog.h>
#include <qinputdialog.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qfiledialog.h>
#include <qevent.h>
#include <qregexp.h>

#include "mainStack.h"
#include "database.h"

MainStack::MainStack(QWidget *parent, const char *name)
    : QWidgetStack(parent,name)
{   
    main.widget = new QWidget(this);
    
    main.hBoxLayout = new QHBoxLayout(main.widget);
    
    main.hBoxLayout->addStretch();
    
    main.vBoxLayout = new QVBoxLayout(main.hBoxLayout);
    
    main.vBoxLayout->addStretch();
    
    main.mainGroup = new QVGroupBox("Main Options", main.widget);
    main.mainGroup->setInsideMargin(30);
    main.mainGroup->setInsideSpacing(20);
    
    main.vBoxLayout->add(main.mainGroup);
    
    db = new Database();
    if(db->getCurClient() != "")
    {
        main.openDefaultButton = new QPushButton(
                QIconSet( QPixmap::fromMimeSource("icons/openDefButton.png") ),
                "Open " + db->getCurClient(), main.mainGroup);
        connect(main.openDefaultButton, SIGNAL(clicked()), this, SLOT(openDefault()));
        main.openDefaultButton->setDefault(true);
        main.openDefaultButton->setFocus();
        main.openDefaultButton->installEventFilter(this);
    }
    else
    {
        main.openDefaultButton = new QPushButton(
                QIconSet( QPixmap::fromMimeSource("icons/openDefButton.png") ),
                "Open ...", main.mainGroup);
        main.openDefaultButton->setEnabled(false);
    }
    
    main.openNewButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/openNewButton.png") ),
            "Open a Different Client", main.mainGroup);
    connect(main.openNewButton, SIGNAL(clicked()), this, SLOT(openNewDialog()));
    main.openNewButton->installEventFilter(this);
    
    main.createNewButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/createNewButton.png") ),
            "Create a New Client", main.mainGroup);
    connect(main.createNewButton, SIGNAL(clicked()), this, SLOT(createNewDialog()));
    main.createNewButton->installEventFilter(this);
    
    main.vBoxLayout->addSpacing(30);
    
    main.otherGroup = new QVGroupBox("Other Options", main.widget);
    main.otherGroup->setInsideMargin(30);
    main.otherGroup->setInsideSpacing(20);
    
    main.vBoxLayout->add(main.otherGroup);
    
    main.importButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/importButton.png") ),
            "Import a Database File", main.otherGroup);
    connect(main.importButton, SIGNAL(clicked()), this, SLOT(import()));
    main.importButton->installEventFilter(this);
    
    main.quitButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/quitButton.png") ),
            "Quit General Ledger", main.otherGroup);
    connect(main.quitButton, SIGNAL(clicked()), this, SLOT(prepareQuit()));
    main.quitButton->installEventFilter(this);
    
    main.vBoxLayout->addStretch();
    
    main.hBoxLayout->addStretch();
    
    addWidget(main.widget, 0);
    
    active = 0;
}

MainStack::~MainStack()
{
    delete db;
    /*
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
    removeWidget(main.widget);
    delete main.quitButton;
    delete main.importButton;
    delete main.otherGroup;
    delete main.createNewButton;
    delete main.openNewButton;
    delete main.openDefaultButton;
    delete main.mainGroup;
    delete main.vBoxLayout;
    delete main.hBoxLayout;
    delete main.widget;
    */
}

void MainStack::clientOpened()
{
    main.openDefaultButton->setText("Open " + db->getCurClient());
    main.openDefaultButton->setEnabled(true);
    
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
            QIconSet( QPixmap::fromMimeSource("icons/closeClient.png") ),
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
            QIconSet( QPixmap::fromMimeSource("icons/accountsTab.png") ),
            "Accounts", tasks.clientGroup);
    connect(tasks.accountsButton, SIGNAL(clicked()), this, SIGNAL(switchToAccounts()));
    tasks.accountsButton->setDefault(true);
    tasks.accountsButton->setFocus();
    tasks.focusedButton = tasks.accountsButton;
    tasks.accountsButton->installEventFilter(this);
    
    tasks.journalButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/journalTab.png") ),
            "Journal Entry", tasks.clientGroup);
    connect(tasks.journalButton, SIGNAL(clicked()), this, SIGNAL(switchToJournal()));
    tasks.journalButton->installEventFilter(this);
    
    tasks.reportButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/reportTab.png") ),
            "Reports", tasks.clientGroup);
    connect(tasks.reportButton, SIGNAL(clicked()), this, SIGNAL(switchToReports()));
    tasks.reportButton->installEventFilter(this);
    
    tasks.vBoxLayout->addSpacing(30);
    
    tasks.adminGroup = new QVGroupBox("Administrative Options", tasks.widget);
    tasks.adminGroup->setInsideMargin(30);
    tasks.adminGroup->setInsideSpacing(20);
    
    tasks.vBoxLayout->add(tasks.adminGroup);
    
    tasks.editNameButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/editNameButton.png") ),
            "Edit Client Name", tasks.adminGroup);
    connect(tasks.editNameButton, SIGNAL(clicked()), this, SLOT(editName()));
    tasks.editNameButton->installEventFilter(this);
    
    tasks.copyDatabaseButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/copyDatabaseButton.png") ),
            "Copy Client Database to File", tasks.adminGroup);
    connect(tasks.copyDatabaseButton, SIGNAL(clicked()), this, SLOT(copyClient()));
    tasks.copyDatabaseButton->installEventFilter(this);
    
    tasks.cryptButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/encryptButton.png") ),
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
    }
}

bool MainStack::eventFilter(QObject *target, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = (QKeyEvent *)event;
        if(keyEvent->key() == Key_F8 && active == 0)
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
    if(db->openDefault())
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
    
    vBoxLayout.addWidget(&listBox);
    
    QHBoxLayout hBoxLayout(&vBoxLayout);
    hBoxLayout.setSpacing(5);
    
    QPushButton okButton("OK", &dialog);
    okButton.setDefault(true);
    connect(&okButton, SIGNAL(clicked()), &dialog, SLOT(accept()));
    QPushButton cancelButton("Cancel", &dialog);
    connect(&cancelButton, SIGNAL(clicked()), &dialog, SLOT(reject()));
    hBoxLayout.addWidget(&okButton);
    hBoxLayout.addWidget(&cancelButton);
    
    dialog.exec();
    if(dialog.result() == QDialog::Accepted)
    {
        QString file = listBox.selectedItem()->text();
        file.remove(QRegExp(".+\\("));
        file.remove(".db)");
        if(db->openNew(file))
        {
            clientOpened();
            emit dbOpened();
        }
    }
}

void MainStack::createNewDialog()
{
    bool ok;
    QString name = QInputDialog::getText("Create a New Client",
                                         "Input the Client's Name:",
                                         QLineEdit::Normal,
                                         QString::null, &ok, this);
    if( ok && !name.isEmpty() )
    {
        if(db->createNew(name))
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
            ".",
            "Client Database (*.db)",
            this,
            0,
            "Copy Client Database to File");
    
    if(!file.isEmpty())
        db->copyDb(file);
}

void MainStack::import()
{}

void MainStack::encrypt()
{}

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

