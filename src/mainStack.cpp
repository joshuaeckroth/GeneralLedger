#include <qwidgetstack.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qiconset.h>
#include <qpixmap.h>
#include <qdialog.h>
#include <qinputdialog.h>
#include <qlistbox.h>

#include "mainStack.h"

MainStack::MainStack(QWidget *parent, const char *name)
    : QWidgetStack(parent,name)
{
    QWidget *mainWidget = new QWidget(this);
    QHBoxLayout *mainHBoxLayout = new QHBoxLayout(mainWidget);
    
    QVBox *mainVBox = new QVBox(mainWidget);
    
    QPushButton *openDefaultButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/openDefButton.png") ),
            "Open Childress", mainVBox);
    connect(openDefaultButton, SIGNAL(clicked()), this, SIGNAL(openDefault()));
    
    QPushButton *openNew = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/openNewButton.png") ),
            "Open a Different Client", mainVBox);
    connect(openNew, SIGNAL(clicked()), this, SLOT(openNewDialog()));
    
    QPushButton *createNew = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/createNewButton.png") ),
            "Create a New Client", mainVBox);
    connect(createNew, SIGNAL(clicked()), this, SLOT(createNewDialog()));
    
    QPushButton *quit = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/quitButton.png") ),
            "Quit General Ledger", mainVBox);
    connect(quit, SIGNAL(clicked()), this, SIGNAL(quit()));
    
    mainHBoxLayout->addStretch();
    mainHBoxLayout->addWidget(mainVBox);
    mainHBoxLayout->addStretch();
    
    addWidget(mainWidget);
}

void MainStack::openNewDialog()
{
    bool ok;
    QDialog dialog;
    dialog.setCaption("Open a Different Client");
    
    QVBoxLayout vBoxLayout(&dialog);
    QListBox listBox(&dialog);
    listBox.insertItem("test");
    vBoxLayout.addWidget(&listBox);
    
    QHBoxLayout hBoxLayout(&vBoxLayout);
    
    QPushButton okButton("OK", &dialog);
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
