#include <qobject.h>
#include <qframe.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qstring.h>

#include "database.h"
#include "settings.h"
#include "balanceElement.h"

BalanceElement::BalanceElement(Database::balanceCategory newCategory, QString newId, QString newType,
    QString newDesc, QString newAccountBegin, QString newAccountEnd)
    : QObject(), category(newCategory), id(newId), type(newType), desc(newDesc),
      accountBegin(newAccountBegin), accountEnd(newAccountEnd)
{
    db = Database::instance();
    settings = Settings::instance();

    iconPath = settings->getIconPath();
}

BalanceElement::~BalanceElement()
{}
   
QFrame* BalanceElement::getFrame(QWidget *parent, void *newSection)
{
    section = newSection;
    
    frame = new QFrame(parent);
    layout = new QHBoxLayout(frame);
    descLabel = new QLabel(desc, frame);
   
    if(type == "0")
        accountsLabel = new QLabel(accountBegin, frame);
    else
        accountsLabel = new QLabel(accountBegin + " - " + accountEnd, frame);
    
    upButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(iconPath + "/balanceElementUp.png") ), "", frame);
    upButton->setFixedSize(28, 28);
    connect(upButton, SIGNAL(clicked()), this, SLOT(elementUp()));
    
    downButton = new QPushButton(
                QIconSet( QPixmap::fromMimeSource(iconPath + "/balanceElementDown.png") ), "", frame);
    downButton->setFixedSize(28, 28);
    connect(downButton, SIGNAL(clicked()), this, SLOT(elementDown()));
    
    removeButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(iconPath + "/balanceElementRemove.png") ), "", frame);
    removeButton->setFixedSize(28, 28);
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeElement()));
    
    layout->addWidget(descLabel);
    layout->addWidget(accountsLabel);
    layout->addWidget(upButton);
    layout->addWidget(downButton);
    layout->addWidget(removeButton);
    
    return frame;
}

void BalanceElement::elementUp()
{
    db->moveBalanceElement(category, id, true);
    emit changed(section);
}

void BalanceElement::elementDown()
{
    db->moveBalanceElement(category, id, false);
    emit changed(section);
}

void BalanceElement::removeElement()
{
    db->removeBalanceElement(category, id);
    emit changed(section);
}

