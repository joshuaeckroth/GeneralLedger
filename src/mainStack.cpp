#include <vector>
using std::vector;
#include <typeinfo>

#include <qwidgetstack.h>
#include <qhbox.h>
#include <qvgroupbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qiconset.h>
#include <qpixmap.h>
#include <qdialog.h>
#include <qinputdialog.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qsqlquery.h>
#include <qdir.h>
#include <qsqldatabase.h>
#include <qfiledialog.h>
#include <qevent.h>

#include "mainStack.h"
#include "generalLedger.h"

MainStack::MainStack(QString curClient, QWidget *parent, const char *name)
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
    
    if(curClient != "")
    {
        main.openDefaultButton = new QPushButton(
                QIconSet( QPixmap::fromMimeSource("icons/openDefButton.png") ),
                QString("Open ").append(curClient), main.mainGroup);
        connect(main.openDefaultButton, SIGNAL(clicked()), this, SIGNAL(openDefault()));
        main.openDefaultButton->setDefault(true);
        main.openDefaultButton->setFocus();
    }
    else
    {
        main.openDefaultButton = new QPushButton(
                QIconSet( QPixmap::fromMimeSource("icons/openDefButton.png") ),
                "Open ...", main.mainGroup);
        main.openDefaultButton->setEnabled(false);
    }
    
    main.widgetVect.push_back(main.openDefaultButton);
    
    main.openNewButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/openNewButton.png") ),
            "Open a Different Client", main.mainGroup);
    connect(main.openNewButton, SIGNAL(clicked()), this, SLOT(openNewDialog()));
    
    main.widgetVect.push_back(main.openNewButton);
    
    main.createNewButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/createNewButton.png") ),
            "Create a New Client", main.mainGroup);
    connect(main.createNewButton, SIGNAL(clicked()), this, SLOT(createNewDialog()));
    
    main.widgetVect.push_back(main.createNewButton);
    
    main.vBoxLayout->addSpacing(30);
    
    main.otherGroup = new QVGroupBox("Other Options", main.widget);
    main.otherGroup->setInsideMargin(30);
    main.otherGroup->setInsideSpacing(20);
    
    main.vBoxLayout->add(main.otherGroup);
    
    main.importButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/importButton.png") ),
            "Import / Merge a Database File", main.otherGroup);
//     connect(...
    
    main.widgetVect.push_back(main.importButton);
    
    main.quitButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/quitButton.png") ),
            "Quit General Ledger", main.otherGroup);
    connect(main.quitButton, SIGNAL(clicked()), this, SIGNAL(prepareQuit()));
    
    main.widgetVect.push_back(main.quitButton);
    main.widgetIter = main.widgetVect.begin();
    
    main.vBoxLayout->addStretch();
    
    main.hBoxLayout->addStretch();
    
    addWidget(main.widget, 0);
    
    active = 0;
}

bool MainStack::focusNextPrevChild(bool next)
{
    QWidget *prev;
    if(active == 0)
    {
        if(next)
        {
            if(main.widgetIter == (main.widgetVect.end() - 1))
            {
                prev = *main.widgetIter;
                main.widgetIter = main.widgetVect.begin();
            }
            else
            {
                prev = *main.widgetIter;
                main.widgetIter++;
            }
        }    
        else
        {
            if(main.widgetIter == main.widgetVect.begin())
            {
                prev = *main.widgetIter;
                main.widgetIter = (main.widgetVect.end() - 1);
            }
            else
            {
                prev = *main.widgetIter;
                main.widgetIter--;
            }
        }
    
        (*main.widgetIter)->setFocus();
        if(typeid(**main.widgetIter) == typeid(QPushButton))
            dynamic_cast<QPushButton*>(*main.widgetIter)->setDefault(true);
        if(typeid(*prev) == typeid(QPushButton))
            dynamic_cast<QPushButton*>(prev)->setDefault(false);
    }
    else
    {
        if(next)
        {
            if(tasks.widgetIter == (tasks.widgetVect.end() - 1))
            {
                prev = *tasks.widgetIter;
                tasks.widgetIter = tasks.widgetVect.begin();
            }
            else
            {
                prev = *tasks.widgetIter;
                tasks.widgetIter++;
            }
        }    
        else
        {
            if(tasks.widgetIter == tasks.widgetVect.begin())
            {
                prev = *tasks.widgetIter;
                tasks.widgetIter = (tasks.widgetVect.end() - 1);
            }
            else
            {
                prev = *tasks.widgetIter;
                tasks.widgetIter--;
            }
        }
    
        (*tasks.widgetIter)->setFocus();
        if(typeid(**tasks.widgetIter) == typeid(QPushButton))
            dynamic_cast<QPushButton*>(*tasks.widgetIter)->setDefault(true);
        if(typeid(*prev) == typeid(QPushButton))
            dynamic_cast<QPushButton*>(prev)->setDefault(false);
    }
    
    return true;
}

void MainStack::dbOpened()
{
    QSqlQuery query("SELECT name FROM main");
    if(query.next())
    {
        main.openDefaultButton->setText(QString("Open ").append(query.value(0).toString()));
        main.openDefaultButton->setEnabled(true);
    }
    
    if(id(tasks.widget) == 1)
    {
        removeWidget(tasks.widget);
        tasks.widgetVect.clear();
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
    connect(tasks.closeButton, SIGNAL(clicked()), this, SIGNAL(closeDb()));
    
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
    
    tasks.widgetVect.push_back(tasks.accountsButton);
    
    tasks.journalButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/journalTab.png") ),
            "Journal Entry", tasks.clientGroup);
    connect(tasks.journalButton, SIGNAL(clicked()), this, SIGNAL(switchToJournal()));
    
    tasks.widgetVect.push_back(tasks.journalButton);
    
    tasks.reportButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/reportTab.png") ),
            "Reports", tasks.clientGroup);
    connect(tasks.reportButton, SIGNAL(clicked()), this, SIGNAL(switchToReports()));
    
    tasks.widgetVect.push_back(tasks.reportButton);
    
    tasks.vBoxLayout->addSpacing(30);
    
    tasks.adminGroup = new QVGroupBox("Administrative Options", tasks.widget);
    tasks.adminGroup->setInsideMargin(30);
    tasks.adminGroup->setInsideSpacing(20);
    
    tasks.vBoxLayout->add(tasks.adminGroup);
    
    tasks.editNameButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/editNameButton.png") ),
            "Edit Client Name", tasks.adminGroup);
    connect(tasks.editNameButton, SIGNAL(clicked()), this, SLOT(editName()));
    
    tasks.widgetVect.push_back(tasks.editNameButton);
    
    tasks.copyDatabaseButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/copyDatabaseButton.png") ),
            "Copy Client Database to File", tasks.adminGroup);
    connect(tasks.copyDatabaseButton, SIGNAL(clicked()), this, SLOT(copyClient()));
    
    tasks.widgetVect.push_back(tasks.copyDatabaseButton);
    
    tasks.cryptButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/encryptButton.png") ),
            "Encrypt Database with Password", tasks.adminGroup);
//     connect(...
    
    tasks.widgetVect.push_back(tasks.cryptButton);
    tasks.widgetIter = tasks.widgetVect.begin();
    
    tasks.vBoxLayout->addStretch();
    
    tasks.hBoxLayout->addStretch();
    
    addWidget(tasks.widget, 1);
        
    switchWidget();
}

void MainStack::switchWidget()
{
    if(active == 0)
    {
        (*tasks.widgetIter)->setFocus();
        raiseWidget(1);
        active = 1;
    }
    else
    {
        (*main.widgetIter)->setFocus();
        raiseWidget(0);
        active = 0;
    }
}

void MainStack::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Key_F8)
        emit closeDb();
    else
        QWidgetStack::keyPressEvent(event);   
    
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
    
#ifdef Q_WS_WIN
    db = QSqlDatabase::addDatabase("QSQLITEX", "temp");
#else 
    db = QSqlDatabase::addDatabase("QSQLITE", "temp");
#endif 
    QDir dir("data");
    QStringList::Iterator it;
    QStringList files = dir.entryList("*.db", QDir::Files);
    it = files.begin();
    while(it != files.end())
    {
        db->setDatabaseName(QString("data/").append(*it));
        if(db->open())
        {
            QSqlQuery query("SELECT name FROM main", db);
            if(query.next())
                listBox.insertItem(query.value(0).toString());
            db->close();
        }
        ++it;
    }
    
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
        emit openNew(listBox.selectedItem()->text());
}

void MainStack::createNewDialog()
{
    bool ok;
    QString name = QInputDialog::getText("Create a New Client",
                                         "Input the Client's Name:",
                                         QLineEdit::Normal,
                                         QString::null, &ok, this);
   if( ok && !name.isEmpty() )
       emit createNew(name);
}

void MainStack::editName()
{
    bool ok;
    QString name = QInputDialog::getText("Edit Client Name",
                                         "Enter a new Client Name:",
                                         QLineEdit::Normal,
                                         QString::null, &ok, this);
    if( ok && !name.isEmpty() )
        qDebug(name);
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
        emit copyDb(file);
}

void MainStack::dbClosed()
{
    switchWidget();
}



