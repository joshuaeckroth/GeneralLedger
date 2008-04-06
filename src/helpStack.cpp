#include <qwidget.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtextbrowser.h>
#include <qstringlist.h>
#include <qmime.h>
#include <qlabel.h>

#include "helpStack.h"
#include "goBackLabel.h"

HelpStack::HelpStack(QWidget *parent, const char *name)
    : QWidget(parent,name)
{
    main.vBoxLayout = new QVBoxLayout(this);
    main.vBoxLayout->setMargin(5);
    main.vBoxLayout->setSpacing(5);
    
    main.labelLayout = new QBoxLayout(main.vBoxLayout, QBoxLayout::LeftToRight);
    
    main.topLabel = new GoBackLabel(this);
    connect(main.topLabel, SIGNAL(goBack()), this, SIGNAL(goBack()));
    
    main.helpContentsButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource("icons/help.png") ),
            "Help Contents (F8)", this);
    main.helpContentsButton->setFocusPolicy(QWidget::NoFocus);
    connect(main.helpContentsButton, SIGNAL(clicked()), this, SLOT(helpContents()));
    
    main.labelLayout->addWidget(main.topLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    main.labelLayout->addStretch();
    main.labelLayout->addWidget(main.helpContentsButton);
    
    main.content = new QTextBrowser(this);
    main.content->setVScrollBarMode(QScrollView::AlwaysOn);
    main.content->mimeSourceFactory()->addFilePath("/home/josh/projects/genledg/help");    
    main.content->setSource("contents.html");
    
    main.vBoxLayout->addWidget(main.content);
    
    main.bottomLabel = new QLabel(
            "<nobr><b>Keyboard Control:</b></nobr><br>"
            "<nobr> &nbsp; &nbsp; <b>Up</b> / <b>Down</b>: Scroll the page</nobr><br>"
            "<nobr> &nbsp; &nbsp; <b>Tab</b> / <b>Shift-Tab</b>: Select a link</nobr>", this);
    
    main.vBoxLayout->addWidget(main.bottomLabel);
   
}

HelpStack::~HelpStack()
{
    /*
    delete main.bottomLabel;
    delete main.content;
    delete main.topLabel;
    delete main.helpContentsButton;
    delete main.labelLayout;
    delete main.vBoxLayout;
    */
}

void HelpStack::helpContents()
{
    main.content->home();
}

