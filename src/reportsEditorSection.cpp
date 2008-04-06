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

#include <vector>
using std::vector;

#include <qwidget.h>
#include <qstring.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include "reportsEditorSection.h"
#include "reportsEditor.h"
#include "reportsEditorDialog.h"
#include "reportsEditorEntityWidget.h"
#include "config.h"

ReportsEditorSection::ReportsEditorSection(QWidget *parent, const char *name,
                const QString newTitle, ReportsEditor::Editor newEditor,
                ReportsEditor::Section newSection)
    : QWidget(parent,name), title(newTitle), editor(newEditor), section(newSection)
{
    reportsEditor = ReportsEditor::instance();


    vBoxLayout = new QVBoxLayout(this, 0, 5,
                    "reportsEditorSection::vBoxLayout");


    header = new QWidget(this, "reportsEditorSection::header");

    headerHBoxLayout = new QHBoxLayout(header, 0, -1,
                    "reportsEditorSection::headerHBoxLayout");

    headerLabel = new QLabel("<nobr><b>" + title + "</b></nobr>", header,
                    "reportsEditorSection::headerLabel");

    headerButton = new QPushButton(QIconSet(
                    QPixmap::fromMimeSource(ICON_PATH + "/addEntity.png")),
                    "Add to " + title, header,
                    "reportsEditorSection::headerButton");
    connect(headerButton, SIGNAL(clicked()), this, SLOT(addEntity()));

    headerHBoxLayout->addWidget(headerLabel);
    headerHBoxLayout->addStretch();
    headerHBoxLayout->addWidget(headerButton);


    body = new QWidget(this, "reportsEditorSection::body");

    bodyHBoxLayout = new QHBoxLayout(body, 0, -1,
                    "reportsEditorSection::bodyHBoxLayout");
    listSection();  // creates list widget
    bodyHBoxLayout->addSpacing(5);
    bodyHBoxLayout->addWidget(list);


    vBoxLayout->addWidget(header);
    vBoxLayout->addWidget(body);
}

void ReportsEditorSection::addEntity()
{
    ReportsEditorDialog *dialog = new ReportsEditorDialog(this,
                    "reportsEditorSection::dialog", title);

    if(dialog->exec())
    {
        QString accountBegin = dialog->getAccountBegin();
        QString accountEnd = dialog->getAccountEnd();
        QString desc = dialog->getDesc();

        if(accountBegin == accountEnd)
            accountEnd = "";

        if(!reportsEditor->entityExists(editor, section,
                        accountBegin, accountEnd, desc)
           && desc != "")
        {
            ReportsEditor::Entity *entity = new ReportsEditor::Entity;
            entity->editor = editor;
            entity->section = section;
            entity->index = reportsEditor->getNextIndex(editor, section);
            entity->accountBegin = accountBegin;
            entity->accountEnd = accountEnd;
            entity->desc = desc;

            reportsEditor->addEntity(entity);

            update();
        }
    }

    delete dialog;
}

void ReportsEditorSection::update()
{
    delete list;
    listSection();

    bodyHBoxLayout->addWidget(list);
    list->show();
}

void ReportsEditorSection::listSection()
{
    vector<ReportsEditor::Entity*> entities = reportsEditor->getEntities(editor, section);

    if(entities.size() == 0)
    {
        numOfEntities = 0;
        list = new QLabel("No Entries", body, "reportsEditorSection::list");
    }
    else
    {
        list = new QWidget(body, "reportsEditorSection::list");
        listVBoxLayout = new QVBoxLayout(list, 0, 5, "reportsEditorSection::listVBoxLayout");
        
        for(vector<ReportsEditor::Entity*>::const_iterator it = entities.begin();
                        it != entities.end(); ++it)
        {
            ReportsEditorEntityWidget *entityWidget = new ReportsEditorEntityWidget(
                            list, "reportsEditorSection::entityWidget", *it);
            connect(entityWidget, SIGNAL(changed()), this, SLOT(update()));

            listVBoxLayout->addWidget(entityWidget);
        }
    }
}


