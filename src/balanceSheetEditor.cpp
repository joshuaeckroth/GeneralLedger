#include <qscrollview.h>
#include <qlayout.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qptrlist.h>
#include <qstringlist.h>

#include "config.h"
#include "database.h"
#include "balanceSheetEditor.h"
#include "editorElement.h"
#include "accountList.h"

BalanceSheetEditor::BalanceSheetEditor(QWidget *parent, const char *name)
    : QScrollView(parent,name)
{
    db = Database::instance();

    setResizePolicy(QScrollView::AutoOneFit);
    setMargins(5,5,5,5);
    
    connect(db, SIGNAL(accountsChanged()), this, SLOT(updateAccounts()));
    accounts = db->getAccountsList();
   
    frame = new QFrame(viewport());
    vBoxLayout = new QVBoxLayout(frame);
    
    assets.topFrame = new QFrame(frame);
    assets.topHBoxLayout = new QHBoxLayout(assets.topFrame);
    
    
    assets.label = new QLabel("<b>Assets:</b>", assets.topFrame);
    assets.addButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/addElement.png") ),
            "Add to Assets", assets.topFrame);
    connect(assets.addButton, SIGNAL(clicked()), this, SLOT(addAssets()));

    assets.topHBoxLayout->addWidget(assets.label);
    assets.topHBoxLayout->addStretch();
    assets.topHBoxLayout->addWidget(assets.addButton);
    
    assets.bottomFrame = new QFrame(frame);
    assets.bottomHBoxLayout = new QHBoxLayout(assets.bottomFrame);
    listSection(&assets);
    
    assets.bottomHBoxLayout->addSpacing(5);
    assets.bottomHBoxLayout->addWidget(assets.listView);
    
    vBoxLayout->addWidget(assets.topFrame);
    vBoxLayout->addSpacing(5);

    vBoxLayout->addWidget(assets.bottomFrame);
    vBoxLayout->addSpacing(10);
   

    liabilities.topFrame = new QFrame(frame);
    liabilities.topHBoxLayout = new QHBoxLayout(liabilities.topFrame);
    
    liabilities.label = new QLabel("<b>Liabilities:</b>", liabilities.topFrame);
    liabilities.addButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/addElement.png") ),
            "Add to Liabilities", liabilities.topFrame);
    connect(liabilities.addButton, SIGNAL(clicked()), this, SLOT(addLiabilities()));
    
    liabilities.topHBoxLayout->addWidget(liabilities.label);
    liabilities.topHBoxLayout->addStretch();
    liabilities.topHBoxLayout->addWidget(liabilities.addButton);
    
    liabilities.bottomFrame = new QFrame(frame);
    liabilities.bottomHBoxLayout = new QHBoxLayout(liabilities.bottomFrame);
    listSection(&liabilities);
    
    liabilities.bottomHBoxLayout->addSpacing(5);
    liabilities.bottomHBoxLayout->addWidget(liabilities.listView);
    
    vBoxLayout->addWidget(liabilities.topFrame);
    vBoxLayout->addSpacing(5);
    vBoxLayout->addWidget(liabilities.bottomFrame);
    vBoxLayout->addSpacing(10);
   
    equities.topFrame = new QFrame(frame);
    equities.topHBoxLayout = new QHBoxLayout(equities.topFrame);
    
    equities.label = new QLabel("<b>Equities:</b>", equities.topFrame);
    equities.addButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/addElement.png") ),
            "Add to Equities", equities.topFrame);
    connect(equities.addButton, SIGNAL(clicked()), this, SLOT(addEquities()));
    
    equities.topHBoxLayout->addWidget(equities.label);
    equities.topHBoxLayout->addStretch();
    equities.topHBoxLayout->addWidget(equities.addButton);
    
    equities.bottomFrame = new QFrame(frame);
    equities.bottomHBoxLayout = new QHBoxLayout(equities.bottomFrame);
    listSection(&equities);
    
    equities.bottomHBoxLayout->addSpacing(5);
    equities.bottomHBoxLayout->addWidget(equities.listView);
    
    vBoxLayout->addWidget(equities.topFrame);
    vBoxLayout->addSpacing(5);
    vBoxLayout->addWidget(equities.bottomFrame);
    
    vBoxLayout->addStretch();
    
    addChild(frame);
}

BalanceSheetEditor::~BalanceSheetEditor()
{
    for(unsigned int i = 0; i < assets.elements.size(); i++)
        delete assets.elements[i];
    assets.elements.clear();
    
    for(unsigned int i = 0; i < liabilities.elements.size(); i++)
        delete liabilities.elements[i];
    liabilities.elements.clear();
    
    for(unsigned int i = 0; i < equities.elements.size(); i++)
        delete equities.elements[i];
    equities.elements.clear();
}

void BalanceSheetEditor::listSection(ElementSection *section)
{
    QPtrList<EditorElement> list;
    if(section == &assets)
        list = db->getElements(Database::balanceAssets);
    if(section == &liabilities)
        list = db->getElements(Database::balanceLiabilities);
    if(section == &equities)
        list = db->getElements(Database::balanceEquities);
    
    if(!list.isEmpty())
    {
        section->listView = new QFrame(section->bottomFrame);
        section->vBoxLayout = new QVBoxLayout(section->listView);
        
        EditorElement *element;
        
        for(element = list.first(); element; element = list.next())
        {
            connect(element, SIGNAL(changed(void*)), this, SLOT(updateSection(void*)));
            section->elements.push_back(element);
            section->vBoxLayout->addWidget(element->getFrame(section->listView, (void*)section));
        }
    }
    else
        section->listView = new QLabel("No Entries", section->bottomFrame);
    
}

void BalanceSheetEditor::deleteSection(ElementSection *section)
{
    if(section->elements.size())
    {
        for(unsigned int i = 0; i < section->elements.size(); i++)
            delete section->elements[i];
        section->elements.clear();
        delete section->vBoxLayout;
    }
    delete section->listView;
}

void BalanceSheetEditor::updateSection(void *newSection)
{
    ElementSection *section = (ElementSection*)newSection;
    deleteSection(section);
    listSection(section);
    
    if(section == &assets) 
    {
        assets.bottomHBoxLayout->addWidget(assets.listView);
        assets.listView->show();
    }
    if(section == &liabilities) 
    {
        liabilities.bottomHBoxLayout->addWidget(liabilities.listView);
        liabilities.listView->show();
    }
    if(section == &equities) 
    {
        equities.bottomHBoxLayout->addWidget(equities.listView);
        equities.listView->show();
    }
}

void BalanceSheetEditor::addElement(int category)
{
    dialog.main = new QDialog;
    switch((Database::editorCategories)category)
    {
        case Database::balanceAssets:
            dialog.main->setCaption("Add to Assets");
            break;
        case Database::balanceLiabilities:
            dialog.main->setCaption("Add to Liabilities");
            break;
        case Database::balanceEquities:
            dialog.main->setCaption("Add to Equities");
            break;
        default:
            break;
    }
    
    dialog.vBoxLayout = new QVBoxLayout(dialog.main);
    dialog.vBoxLayout->setMargin(5);
    dialog.vBoxLayout->setSpacing(5);
    
    dialog.oneAccount = new QRadioButton("One Account", dialog.main);
    connect(dialog.oneAccount, SIGNAL(clicked()), this, SLOT(oneAccountToggled()));
        
    dialog.oneAccountFrame = new QFrame(dialog.main);
    dialog.oneAccountHBoxLayout = new QHBoxLayout(dialog.oneAccountFrame);
    dialog.oneAccountHBoxLayout->setSpacing(5);
    
    dialog.oneAccountHBoxLayout->addSpacing(5);
    dialog.oneAccountLabel = new QLabel("Account:", dialog.oneAccountFrame);
    dialog.oneAccountHBoxLayout->addWidget(dialog.oneAccountLabel);
    dialog.oneAccountEdit = new AccountList(dialog.oneAccountFrame, 0, accounts);
    dialog.oneAccountHBoxLayout->addWidget(dialog.oneAccountEdit);
    dialog.oneAccountHBoxLayout->addStretch();
       
    dialog.rangeAccount = new QRadioButton("Range of Accounts", dialog.main);
    connect(dialog.rangeAccount, SIGNAL(clicked()), this, SLOT(rangeAccountToggled()));
       
    dialog.rangeAccountFrame = new QFrame(dialog.main, "rangeAccountFrame");
    dialog.rangeAccountHBoxLayout = new QHBoxLayout(dialog.rangeAccountFrame);
    dialog.rangeAccountHBoxLayout->addSpacing(5);
    
    dialog.rangeAccountFrameList = new QFrame(dialog.rangeAccountFrame);
    dialog.rangeAccountFrameListVBoxLayout = new QVBoxLayout(dialog.rangeAccountFrameList);
    dialog.rangeAccountFrameListVBoxLayout->setSpacing(5);
    
    dialog.rangeBeginEndFrame = new QFrame(dialog.rangeAccountFrameList);
    dialog.rangeBeginEndHBoxLayout = new QHBoxLayout(dialog.rangeBeginEndFrame);
    dialog.rangeBeginEndHBoxLayout->setSpacing(5);
    
    dialog.rangeBegin = new QLabel("Begin:", dialog.rangeBeginEndFrame);
    dialog.rangeBeginEndHBoxLayout->addWidget(dialog.rangeBegin);
    dialog.rangeBeginEdit = new AccountList(dialog.rangeBeginEndFrame, 0, accounts);
    dialog.rangeBeginEndHBoxLayout->addWidget(dialog.rangeBeginEdit);
    dialog.rangeBetween = new QLabel("to", dialog.rangeBeginEndFrame);
    dialog.rangeBeginEndHBoxLayout->addWidget(dialog.rangeBetween);
    dialog.rangeEndEdit = new AccountList(dialog.rangeBeginEndFrame, 0, accounts);
    dialog.rangeBeginEndHBoxLayout->addWidget(dialog.rangeEndEdit);
    dialog.rangeBeginEndHBoxLayout->addStretch();
    
    dialog.rangeDescFrame = new QFrame(dialog.rangeAccountFrameList);
    dialog.rangeDescFrameHBoxLayout = new QHBoxLayout(dialog.rangeDescFrame);
    dialog.rangeDescFrameHBoxLayout->setSpacing(5);
    
    dialog.rangeDesc = new QLabel("Description:", dialog.rangeDescFrame);
    dialog.rangeDescFrameHBoxLayout->addWidget(dialog.rangeDesc);
    dialog.rangeDescEdit = new QLineEdit(dialog.rangeDescFrame);
    dialog.rangeDescFrameHBoxLayout->addWidget(dialog.rangeDescEdit);
    
    dialog.rangeAccountFrameListVBoxLayout->addWidget(dialog.rangeBeginEndFrame);
    dialog.rangeAccountFrameListVBoxLayout->addWidget(dialog.rangeDescFrame);
    
    dialog.rangeAccountHBoxLayout->addWidget(dialog.rangeAccountFrameList);
    
        
    dialog.buttonFrame = new QFrame(dialog.main);
    dialog.buttonFrameHBoxLayout = new QHBoxLayout(dialog.buttonFrame);
    dialog.buttonFrameHBoxLayout->setSpacing(5);
    dialog.buttonFrameHBoxLayout->addStretch();
    
    dialog.ok = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/addElement.png")),
            "Add", dialog.buttonFrame);
    connect(dialog.ok, SIGNAL(clicked()), dialog.main, SLOT(accept()));
    dialog.buttonFrameHBoxLayout->addWidget(dialog.ok);
    
    dialog.cancel = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/cancelButton.png")),
            "Cancel", dialog.buttonFrame);
    connect(dialog.cancel, SIGNAL(clicked()), dialog.main, SLOT(reject()));
    dialog.buttonFrameHBoxLayout->addWidget(dialog.cancel);
    
    
    dialog.vBoxLayout->addWidget(dialog.oneAccount);
    dialog.vBoxLayout->addWidget(dialog.oneAccountFrame);
    dialog.vBoxLayout->addWidget(dialog.rangeAccount);
    dialog.vBoxLayout->addWidget(dialog.rangeAccountFrame);
    dialog.vBoxLayout->addSpacing(10);
    dialog.vBoxLayout->addWidget(dialog.buttonFrame);
    
    oneAccountToggled();
    
    if(dialog.main->exec())
    {
        if(dialog.oneAccount->isChecked())
            db->createElement((Database::editorCategories)category,
                                      "0", "", dialog.oneAccountEdit->accountNumber(), "");
        else
            db->createElement((Database::editorCategories)category,
                                      "1", dialog.rangeDescEdit->text(),
                                      dialog.rangeBeginEdit->accountNumber(), dialog.rangeEndEdit->accountNumber());
    }
    
    delete dialog.main;
}

void BalanceSheetEditor::oneAccountToggled()
{
    dialog.oneAccount->setChecked(true);
    dialog.rangeAccount->setChecked(false);
    dialog.oneAccountLabel->setEnabled(true);
    dialog.oneAccountEdit->setEnabled(true);
    dialog.rangeBegin->setEnabled(false);
    dialog.rangeBeginEdit->setEnabled(false);
    dialog.rangeBetween->setEnabled(false);
    dialog.rangeEndEdit->setEnabled(false);
    dialog.rangeDesc->setEnabled(false);
    dialog.rangeDescEdit->setEnabled(false);
}

void BalanceSheetEditor::rangeAccountToggled()
{
    dialog.rangeAccount->setChecked(true);
    dialog.oneAccount->setChecked(false);
    dialog.oneAccountLabel->setEnabled(false);
    dialog.oneAccountEdit->setEnabled(false);
    dialog.rangeBegin->setEnabled(true);
    dialog.rangeBeginEdit->setEnabled(true);
    dialog.rangeBetween->setEnabled(true);
    dialog.rangeEndEdit->setEnabled(true);
    dialog.rangeDesc->setEnabled(true);
    dialog.rangeDescEdit->setEnabled(true);
}

void BalanceSheetEditor::addAssets()
{
    addElement((int)Database::balanceAssets);
    updateSection(&assets);
}

void BalanceSheetEditor::addLiabilities()
{
    addElement((int)Database::balanceLiabilities);
    updateSection(&liabilities);
}

void BalanceSheetEditor::addEquities()
{
    addElement((int)Database::balanceEquities);
    updateSection(&equities);
}

void BalanceSheetEditor::updateAccounts()
{
    accounts = db->getAccountsList();
}
