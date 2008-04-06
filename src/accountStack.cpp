#include <typeinfo>

#include <qwidget.h>
#include <qvgroupbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qlabel.h>
#include <qfiledialog.h>
#include <qevent.h>

#include "accountStack.h"
#include "accountTable.h"
#include "goBackLabel.h"
#include "database.h"

AccountStack::AccountStack(QWidget *parent, const char *name)
    : QWidget(parent,name)
{
    db = new Database;
    active = false;
}

AccountStack::~AccountStack()
{
    delete db;
    /*
    if(active)
    {
        delete main.exportButton;
        delete main.importButton;
        delete main.bottomRightBoxLayout;
        delete main.bottomRightFrame;
        delete main.bottomLabel;
        delete main.bottomHBoxLayout;
        delete main.bottomWidget;
        delete main.dataTable;
        delete main.topLabel;
        delete main.labelLayout;
        delete main.vBoxLayout;
    }
    */
}

void AccountStack::dbOpened()
{
    if(active)
    {
        delete main.exportButton;
        delete main.importButton;
        delete main.bottomRightBoxLayout;
        delete main.bottomRightFrame;
        delete main.bottomLabel;
        delete main.bottomHBoxLayout;
        delete main.bottomWidget;
        delete main.dataTable;
        delete main.topLabel;
        delete main.labelLayout;
        delete main.vBoxLayout;
    }
    
    main.vBoxLayout = new QVBoxLayout(this);
    main.vBoxLayout->setMargin(5);
    main.vBoxLayout->setSpacing(5);
    
    main.labelLayout = new QHBoxLayout(main.vBoxLayout);
    main.topLabel = new GoBackLabel(this);
    connect(main.topLabel, SIGNAL(goBack()), this, SIGNAL(goBack()));
    main.labelLayout->addWidget(main.topLabel);
    main.labelLayout->addStretch();
    
    main.dataTable = new AccountTable(this);
    main.vBoxLayout->addWidget(main.dataTable);
    
    main.bottomWidget = new QWidget(this);
    main.bottomHBoxLayout = new QHBoxLayout(main.bottomWidget);
    
    main.bottomLabel = new QLabel(
            "<nobr><b>Keyboard Control:</b></nobr><br>"
            "<nobr> &nbsp; &nbsp; <b>Enter</b>: Edit current field, move to next field</nobr><br>"
            "<nobr> &nbsp; &nbsp; <b>Insert</b> / <b>Delete</b>: Create new entry / Delete selected entry</nobr>",
            main.bottomWidget);
    
    main.bottomRightFrame = new QFrame(main.bottomWidget);
    main.bottomRightBoxLayout = new QBoxLayout(main.bottomRightFrame, QBoxLayout::LeftToRight);
    main.bottomRightBoxLayout->setSpacing(5);
    
    main.importButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/import.png") ),
            "Import from CSV (F11)", main.bottomRightFrame);
    
    main.exportButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/export.png") ),
            "Export to CSV (F12)", main.bottomRightFrame);
    
    main.bottomRightBoxLayout->addWidget(main.importButton, 0, Qt::AlignLeft | Qt::AlignTop);
    main.bottomRightBoxLayout->addWidget(main.exportButton, 0, Qt::AlignLeft | Qt::AlignTop);
    
    main.bottomHBoxLayout->addWidget(main.bottomLabel);
    main.bottomHBoxLayout->addStretch();
    main.bottomHBoxLayout->addWidget(main.bottomRightFrame);
    
    main.vBoxLayout->addWidget(main.bottomWidget);
    
    active = true;
}

void AccountStack::importCSV()
{
    QString file = QFileDialog::getOpenFileName(
            ".",
            "Comma Separated Values (*.csv)",
            this,
            0,
            "Import Accounts");

    db->importCSV("accounts", file);    
}

void AccountStack::exportCSV()
{
    QString file = QFileDialog::getSaveFileName(
            ".",
            "Comma Separated Values (*.csv)",
            this,
            0,
            "Export Accounts");

    db->exportCSV("accounts", file);
}

bool AccountStack::eventFilter(QObject *target, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = (QKeyEvent *)event;
        if(keyEvent->key() == Key_Escape)
        {
            emit goBack();
            return true;
        }
    }
    
    return QWidget::eventFilter(target,event);
}  


