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
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include "reportsEditorEntityWidget.h"
#include "reportsEditor.h"
#include "config.h"

ReportsEditorEntityWidget::ReportsEditorEntityWidget(
                QWidget *parent, const char *name,
                ReportsEditor::Entity *newData)
    : QWidget(parent,name), data(newData)
{
    reportsEditor = ReportsEditor::instance();


    hBoxLayout = new QHBoxLayout(this, 0, 2,
                    "reportsEditorEntityWidget::hBoxLayout");

    descLabel = new QLabel(data->desc, this,
                    "reportsEditorEntityWidget::descLabel");

    if(data->accountEnd == "")
    {
        accountsLabel = new QLabel(data->accountBegin, this,
                        "reportsEditorEntityWidget::accountsLabel");
    }
    else
    {
        accountsLabel = new QLabel(data->accountBegin + " - " + data->accountEnd,
                        this, "reportsEditorEntityWidget::accountsLabel");
    }

    upButton = new QPushButton(QIconSet(
                    QPixmap::fromMimeSource(ICON_PATH + "/entityUp.png")),
                    "", this, "reportsEditorEntityWidget::upButton");
    upButton->setFixedSize(28, 28);
    connect(upButton, SIGNAL(clicked()), this, SLOT(moveUp()));

    Q_UINT16 firstIndex = reportsEditor->getFirstIndex(
                            (ReportsEditor::Editor)(data->editor),
                            (ReportsEditor::Section)(data->section));
    if(firstIndex == 0 || data->index == firstIndex)
    {
        upButton->setEnabled(false);
    }

    downButton = new QPushButton(QIconSet(
                    QPixmap::fromMimeSource(ICON_PATH + "/entityDown.png")),
                    "", this, "reportsEditorEntityWidget::downButton");
    downButton->setFixedSize(28, 28);
    connect(downButton, SIGNAL(clicked()), this, SLOT(moveDown()));

    if(data->index == reportsEditor->getLastIndex(
                            (ReportsEditor::Editor)(data->editor),
                            (ReportsEditor::Section)(data->section)))
    {
        downButton->setEnabled(false);
    }

    removeButton = new QPushButton(QIconSet(
                    QPixmap::fromMimeSource(ICON_PATH + "/entityRemove.png")),
                    "", this, "reportsEditorEntityWidget::removeButton");
    removeButton->setFixedSize(28, 28);
    connect(removeButton, SIGNAL(clicked()), this, SLOT(remove()));

    hBoxLayout->addWidget(descLabel);
    hBoxLayout->addWidget(accountsLabel);
    hBoxLayout->addWidget(upButton);
    hBoxLayout->addWidget(downButton);
    hBoxLayout->addWidget(removeButton);
}

void ReportsEditorEntityWidget::moveUp()
{
    reportsEditor->moveEntityUp(data);
    emit changed();
}

void ReportsEditorEntityWidget::moveDown()
{
    reportsEditor->moveEntityDown(data);
    emit changed();
}

void ReportsEditorEntityWidget::remove()
{
    reportsEditor->removeEntity(data);
    emit changed();
}



