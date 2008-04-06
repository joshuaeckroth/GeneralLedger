#include <qobject.h>
#include <qframe.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qstring.h>

#include "config.h"
#include "database.h"
#include "editorElement.h"

EditorElement::EditorElement(Database::editorCategories newCategory, QString newId, QString newType,
    QString newDesc, QString newAccountBegin, QString newAccountEnd)

    : QObject(), category(newCategory), id(newId), type(newType), desc(newDesc),
      accountBegin(newAccountBegin), accountEnd(newAccountEnd)
{
    db = Database::instance();
}

EditorElement::~EditorElement()
{}
   
QFrame* EditorElement::getFrame(QWidget *parent, void *newSection)
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
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/elementUp.png") ), "", frame);
    upButton->setFixedSize(28, 28);
    connect(upButton, SIGNAL(clicked()), this, SLOT(elementUp()));
    
    downButton = new QPushButton(
                QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/elementDown.png") ), "", frame);
    downButton->setFixedSize(28, 28);
    connect(downButton, SIGNAL(clicked()), this, SLOT(elementDown()));
    
    removeButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/elementRemove.png") ), "", frame);
    removeButton->setFixedSize(28, 28);
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeElement()));
    
    layout->addWidget(descLabel);
    layout->addWidget(accountsLabel);
    layout->addWidget(upButton);
    layout->addWidget(downButton);
    layout->addWidget(removeButton);
    
    return frame;
}

void EditorElement::elementUp()
{
    db->moveElement(category, id, true);
    emit changed(section);
}

void EditorElement::elementDown()
{
    db->moveElement(category, id, false);
    emit changed(section);
}

void EditorElement::removeElement()
{
    db->removeElement(category, id);
    emit changed(section);
}

