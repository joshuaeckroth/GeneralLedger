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

#include <qwidget.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtextbrowser.h>
#include <qstringlist.h>
#include <qmime.h>
#include <qlabel.h>
#include <qevent.h>

#include "config.h"
#include "helpTab.h"
#include "globalGoBackLabel.h"

HelpTab::HelpTab(QWidget *parent, const char *name)
    : QWidget(parent,name)
{
    main.vBoxLayout = new QVBoxLayout(this, 0, -1, "helpTab::main.vBoxLayout");
    main.vBoxLayout->setMargin(5);
    main.vBoxLayout->setSpacing(5);
    
    main.labelLayout = new QBoxLayout(main.vBoxLayout, QBoxLayout::LeftToRight, -1,
                    "helpTab::main.labelLayout");
    
    main.topLabel = new GlobalGoBackLabel(this, "helpTab::main.topLabel");
    connect(main.topLabel, SIGNAL(goBack()), this, SIGNAL(switchToHome()));
    
    main.helpContentsButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/helpTab.png") ),
            "Help Contents (F8)", this, "helpTab::main.helpContentsButton");
    main.helpContentsButton->setFocusPolicy(QWidget::NoFocus);
    connect(main.helpContentsButton, SIGNAL(clicked()), this, SLOT(helpContents()));
    
    main.labelLayout->addWidget(main.topLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    main.labelLayout->addStretch();
    main.labelLayout->addWidget(main.helpContentsButton);
    
    main.content = new QTextBrowser(this, "helpTab::main.content");
    main.content->setVScrollBarMode(QScrollView::AlwaysOn);
    main.content->mimeSourceFactory()->addFilePath(HELP_PATH);
    main.content->setSource("contents.html");
    main.content->installEventFilter(this);
    
    main.vBoxLayout->addWidget(main.content);
    
    main.bottomLabel = new QLabel(
            "<nobr><b>Keyboard Control:</b></nobr><br>"
            "<nobr> &nbsp; &nbsp; <b>Up</b> / <b>Down</b>: Scroll the page</nobr><br>"
            "<nobr> &nbsp; &nbsp; <b>Tab</b> / <b>Shift-Tab</b>: Select a link</nobr>", this,
            "helpTab::main.bottomLabel");
    
    main.vBoxLayout->addWidget(main.bottomLabel);
   
}

void HelpTab::helpContents()
{
    main.content->home();
}

void HelpTab::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Key_Escape:
            emit switchToHome();
            break;
        case Key_F8:
            helpContents();
            break;
        default:
            QWidget::keyPressEvent(event);
    }
}


