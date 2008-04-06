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
#include <qiodevice.h>
#include <qdatastream.h>
#include <qscrollview.h>
#include <qlayout.h>

#include "reportsEditor.h"
#include "reportsEditorSection.h"

ReportsEditor *ReportsEditor::thisInstance = 0;

ReportsEditor* ReportsEditor::instance()
{
    if(thisInstance == 0)
        thisInstance = new ReportsEditor;
    return thisInstance;
}

void ReportsEditor::destroy()
{
    if(thisInstance != 0)
        delete thisInstance;
}

void ReportsEditor::initiate()
{}

void ReportsEditor::readStream(QIODevice *device, Q_UINT16 editorEntityCount)
{
    QDataStream in(device);

    Q_UINT16 entityCount = 0;
    while(entityCount++ != editorEntityCount)
    {
        Entity *entity = new Entity;
        in >> entity->editor
           >> entity->section
           >> entity->index
           >> entity->accountBegin
           >> entity->accountEnd
           >> entity->desc;
        editorEntities.push_back(entity);
    }
}

void ReportsEditor::writeStream(QIODevice *device)
{
    QDataStream out(device);

    for(vector<Entity*>::const_iterator it = editorEntities.begin();
                    it != editorEntities.end(); ++it)
    {
        if((*it) != 0)
        {
            out << (*it)->editor
                << (*it)->section
                << (*it)->index
                << (*it)->accountBegin
                << (*it)->accountEnd
                << (*it)->desc;
        }
    }
}

void ReportsEditor::clearData()
{
    for(vector<Entity*>::iterator it = editorEntities.begin();
        it != editorEntities.end(); ++it)
    {
        if((*it) != 0)
            delete (*it);
    }

    editorEntities.clear();
}

Q_UINT16 ReportsEditor::getEditorEntityCount() const
{
    Q_UINT16 count = 0;
    for(vector<Entity*>::const_iterator it = editorEntities.begin();
                    it != editorEntities.end(); ++it)
    {
        if((*it) != 0)
            count++;
    }

    return count;
}

QWidget* ReportsEditor::createEditor(QWidget *parent, const char *name,
                ReportsEditor::Editor editor)
{
    QScrollView *scrollView = new QScrollView(parent,name);
    scrollView->setResizePolicy(QScrollView::AutoOneFit);

    QWidget *mainWidget = new QWidget(scrollView->viewport(),
                    "reportsEditor::mainWidget");

    QHBoxLayout *hBoxLayout = new QHBoxLayout(mainWidget, 0, -1,
                    "reportsEditor::hBoxLayout");
    hBoxLayout->addSpacing(5);
    
    QVBoxLayout *vBoxLayout = new QVBoxLayout(hBoxLayout, -1,
                    "reportsEditor::vBoxLayout");
    vBoxLayout->addSpacing(5);

    if(editor == BalanceSheet)
    {
        ReportsEditorSection *assets = new ReportsEditorSection(mainWidget,
                        "reportsEditor::assets",
                        "Assets", BalanceSheet, Assets);
        ReportsEditorSection *liabilities = new ReportsEditorSection(mainWidget,
                        "reportsEditor::liabilities",
                        "Liabilities", BalanceSheet, Liabilities);
        ReportsEditorSection *equities = new ReportsEditorSection(mainWidget,
                        "reportsEditor::equities",
                        "Equities", BalanceSheet, Equities);

        vBoxLayout->addWidget(assets);
        vBoxLayout->addSpacing(10);
        vBoxLayout->addWidget(liabilities);
        vBoxLayout->addSpacing(10);
        vBoxLayout->addWidget(equities);

    }
    else if(editor == IncomeSvc)
    {
        ReportsEditorSection *income = new ReportsEditorSection(mainWidget,
                        "reportsEditor::income",
                        "Income", IncomeSvc, Income);
        ReportsEditorSection *expenses = new ReportsEditorSection(mainWidget,
                        "reportsEditor::expenses",
                        "Expenses", IncomeSvc, Expenses);

        vBoxLayout->addWidget(income);
        vBoxLayout->addSpacing(10);
        vBoxLayout->addWidget(expenses);
    }
    else if(editor == IncomeRetail)
    {
        ReportsEditorSection *income = new ReportsEditorSection(mainWidget,
                        "reportsEditor::income",
                        "Income", IncomeRetail, Income);
        ReportsEditorSection *costOfSales = new ReportsEditorSection(mainWidget,
                        "reportsEditor::costOfSales",
                        "Cost Of Sales", IncomeRetail, CostOfSales);
        ReportsEditorSection *expenses = new ReportsEditorSection(mainWidget,
                        "reportsEditor::expenses",
                        "Expenses", IncomeRetail, Expenses);

        vBoxLayout->addWidget(income);
        vBoxLayout->addSpacing(10);
        vBoxLayout->addWidget(costOfSales);
        vBoxLayout->addSpacing(10);
        vBoxLayout->addWidget(expenses);
    }

    vBoxLayout->addStretch();

    hBoxLayout->addSpacing(5);
    
    scrollView->addChild(mainWidget);

    return scrollView;
}

vector<ReportsEditor::Entity*> ReportsEditor::getEntities(
                ReportsEditor::Editor editor, ReportsEditor::Section section)
{
    vector<Entity*> entitiesUnsorted;
    
    for(vector<Entity*>::const_iterator it = editorEntities.begin();
                    it != editorEntities.end(); ++it)
    {
        if((*it) != 0)
        {
            if((*it)->editor == editor && (*it)->section == section)
            {
                entitiesUnsorted.push_back(*it);
            }
        }
    }

    vector<Entity*> entitiesSorted;
    
    for(vector<Entity*>::const_iterator it = entitiesUnsorted.begin();
                    it != entitiesUnsorted.end(); ++it)
    {
        bool inserted = false;
        for(vector<Entity*>::iterator innerIt = entitiesSorted.begin();
                        innerIt != entitiesSorted.end(); ++innerIt)
        {
            if((*it)->index < (*innerIt)->index)
            {
                entitiesSorted.insert(innerIt, *it);

                inserted = true;
                break;
            }
        }
        
        if(!inserted)
            entitiesSorted.push_back(*it);
    }

    return entitiesSorted;
}

bool ReportsEditor::entityExists(
                ReportsEditor::Editor editor, ReportsEditor::Section section,
                QString accountBegin, QString accountEnd, QString desc)
{
    bool exists = false;
    for(vector<Entity*>::const_iterator it = editorEntities.begin();
                    it != editorEntities.end(); ++it)
    {
        if((*it) != 0)
        {
            if((*it)->editor == editor && (*it)->section == section
               && (*it)->accountBegin == accountBegin
               && (*it)->accountEnd == accountEnd
               && (*it)->desc == desc)
            {
                exists = true;
                break;
            }
        }
    }

    return exists;
}

Q_UINT16 ReportsEditor::getFirstIndex(
                ReportsEditor::Editor editor, ReportsEditor::Section section)
{
    Q_UINT16 index = getLastIndex(editor, section);
    for(vector<Entity*>::const_iterator it = editorEntities.begin();
                    it != editorEntities.end(); ++it)
    {
        if((*it) != 0)
        {
            if((*it)->editor == editor && (*it)->section == section
                            && (*it)->index < index)
            {
                index = (*it)->index;
            }
        }
    }

    return index;
}


Q_UINT16 ReportsEditor::getLastIndex(
                ReportsEditor::Editor editor, ReportsEditor::Section section)
{
    Q_UINT16 index = 0;
    for(vector<Entity*>::const_iterator it = editorEntities.begin();
                    it != editorEntities.end(); ++it)
    {
        if((*it) != 0)
        {
            if((*it)->editor == editor && (*it)->section == section
                            && (*it)->index > index)
            {
                index = (*it)->index;
            }
        }
    }

    return index;
}

Q_UINT16 ReportsEditor::getNextIndex(
                ReportsEditor::Editor editor, ReportsEditor::Section section)
{
    return (getLastIndex(editor,section) + 1);
}

void ReportsEditor::moveEntityUp(ReportsEditor::Entity *entity)
{
    Q_UINT16 before = getFirstIndex((Editor)(entity->editor),
                    (Section)(entity->section));

    for(vector<Entity*>::const_iterator it = editorEntities.begin();
                    it != editorEntities.end(); ++it)
    {
        if((*it) != 0)
        {
            if((*it)->editor == entity->editor
               && (*it)->section == entity->section)
            {
                if((*it)->index < entity->index && (*it)->index > before)
                {
                    before = (*it)->index;
                }
            }
        }
    }

    Q_UINT16 entityIndex = entity->index;

    for(vector<Entity*>::const_iterator it = editorEntities.begin();
                    it != editorEntities.end(); ++it)
    {
        if((*it) != 0)
        {
            if((*it)->editor == entity->editor
               && (*it)->section == entity->section)
            {
                if((*it)->index == before && (*it) != entity)
                {
                    (*it)->index = entityIndex;
                }
                if((*it) == entity)
                {
                    (*it)->index = before;
                }
            }
        }
    }

    emit saveToDisk();
}

void ReportsEditor::moveEntityDown(ReportsEditor::Entity *entity)
{
    Q_UINT16 after = getLastIndex((Editor)(entity->editor),
                    (Section)(entity->section));

    for(vector<Entity*>::const_iterator it = editorEntities.begin();
                    it != editorEntities.end(); ++it)
    {
        if((*it) != 0)
        {
            if((*it)->editor == entity->editor
               && (*it)->section == entity->section)
            {
                if((*it)->index > entity->index && (*it)->index < after)
                {
                    after = (*it)->index;
                }
            }
        }
    }

    Q_UINT16 entityIndex = entity->index;

    for(vector<Entity*>::const_iterator it = editorEntities.begin();
                    it != editorEntities.end(); ++it)
    {
        if((*it) != 0)
        {
            if((*it)->editor == entity->editor
               && (*it)->section == entity->section)
            {
                if((*it)->index == after && (*it) != entity)
                {
                    (*it)->index = entityIndex;
                }
                if(*it == entity)
                {
                    (*it)->index = after;
                }
            }
        }
    }

    emit saveToDisk();
}

void ReportsEditor::addEntity(ReportsEditor::Entity *entity)
{
    editorEntities.push_back(entity);

    emit saveToDisk();
}

void ReportsEditor::removeEntity(ReportsEditor::Entity *entity)
{
    for(vector<Entity*>::iterator it = editorEntities.begin();
                    it != editorEntities.end(); ++it)
    {
        if(*it == entity)
        {
            *it = 0;
            delete entity;
        }
    }

    emit saveToDisk();
}


// Private

ReportsEditor::ReportsEditor() : QObject(0, "ReportsEditor")
{}

ReportsEditor::~ReportsEditor()
{}

