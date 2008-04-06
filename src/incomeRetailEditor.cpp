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
#include "incomeRetailEditor.h"
#include "editorElement.h"
#include "accountList.h"

IncomeRetailEditor::IncomeRetailEditor(QWidget *parent, const char *name)
    : QScrollView(parent,name)
{
    db = Database::instance();
    
    setResizePolicy(QScrollView::AutoOneFit);
    setMargins(5,5,5,5);
    
    connect(db, SIGNAL(accountsChanged()), this, SLOT(updateAccounts()));
    accounts = db->getAccountsList();
   
    frame = new QFrame(viewport());
    vBoxLayout = new QVBoxLayout(frame);
    
    income.topFrame = new QFrame(frame);
    income.topHBoxLayout = new QHBoxLayout(income.topFrame);
    
    
    income.label = new QLabel("<b>Income:</b>", income.topFrame);
    income.addButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/addElement.png") ),
            "Add to Income", income.topFrame);
    connect(income.addButton, SIGNAL(clicked()), this, SLOT(addIncome()));

    income.topHBoxLayout->addWidget(income.label);
    income.topHBoxLayout->addStretch();
    income.topHBoxLayout->addWidget(income.addButton);
    
    income.bottomFrame = new QFrame(frame);
    income.bottomHBoxLayout = new QHBoxLayout(income.bottomFrame);
    listSection(&income);
    
    income.bottomHBoxLayout->addSpacing(5);
    income.bottomHBoxLayout->addWidget(income.listView);
    
    vBoxLayout->addWidget(income.topFrame);
    vBoxLayout->addSpacing(5);

    vBoxLayout->addWidget(income.bottomFrame);
    vBoxLayout->addSpacing(10);
   

    cos.topFrame = new QFrame(frame);
    cos.topHBoxLayout = new QHBoxLayout(cos.topFrame);
    
    cos.label = new QLabel("<nobr><b>Cost Of Sales:</b></nobr>", cos.topFrame);
    cos.addButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/addElement.png") ),
            "Add to Cost Of Sales", cos.topFrame);
    connect(cos.addButton, SIGNAL(clicked()), this, SLOT(addCOS()));
    
    cos.topHBoxLayout->addWidget(cos.label);
    cos.topHBoxLayout->addStretch();
    cos.topHBoxLayout->addWidget(cos.addButton);
    
    cos.bottomFrame = new QFrame(frame);
    cos.bottomHBoxLayout = new QHBoxLayout(cos.bottomFrame);
    listSection(&cos);
    
    cos.bottomHBoxLayout->addSpacing(5);
    cos.bottomHBoxLayout->addWidget(cos.listView);
    
    vBoxLayout->addWidget(cos.topFrame);
    vBoxLayout->addSpacing(5);
    vBoxLayout->addWidget(cos.bottomFrame);
    vBoxLayout->addSpacing(10);
   
    expenses.topFrame = new QFrame(frame);
    expenses.topHBoxLayout = new QHBoxLayout(expenses.topFrame);
    
    expenses.label = new QLabel("<b>Expenses:</b>", expenses.topFrame);
    expenses.addButton = new QPushButton(
            QIconSet( QPixmap::fromMimeSource(ICON_PATH + "/addElement.png") ),
            "Add to Expenses", expenses.topFrame);
    connect(expenses.addButton, SIGNAL(clicked()), this, SLOT(addExpenses()));
    
    expenses.topHBoxLayout->addWidget(expenses.label);
    expenses.topHBoxLayout->addStretch();
    expenses.topHBoxLayout->addWidget(expenses.addButton);
    
    expenses.bottomFrame = new QFrame(frame);
    expenses.bottomHBoxLayout = new QHBoxLayout(expenses.bottomFrame);
    listSection(&expenses);
    
    expenses.bottomHBoxLayout->addSpacing(5);
    expenses.bottomHBoxLayout->addWidget(expenses.listView);
    
    vBoxLayout->addWidget(expenses.topFrame);
    vBoxLayout->addSpacing(5);
    vBoxLayout->addWidget(expenses.bottomFrame);
    
    vBoxLayout->addStretch();
    
    addChild(frame);
}

IncomeRetailEditor::~IncomeRetailEditor()
{
    for(unsigned int i = 0; i < income.elements.size(); i++)
        delete income.elements[i];
    income.elements.clear();
    
    for(unsigned int i = 0; i < cos.elements.size(); i++)
        delete cos.elements[i];
    cos.elements.clear();
    
    for(unsigned int i = 0; i < expenses.elements.size(); i++)
        delete expenses.elements[i];
    expenses.elements.clear();
}

void IncomeRetailEditor::listSection(ElementSection *section)
{
    QPtrList<EditorElement> list;
    if(section == &income)
        list = db->getElements(Database::incomeRetailIncome);
    if(section == &cos)
        list = db->getElements(Database::incomeRetailCOS);
    if(section == &expenses)
        list = db->getElements(Database::incomeRetailExpenses);
    
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

void IncomeRetailEditor::deleteSection(ElementSection *section)
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

void IncomeRetailEditor::updateSection(void *newSection)
{
    ElementSection *section = (ElementSection*)newSection;
    deleteSection(section);
    listSection(section);
    
    if(section == &income) 
    {
        income.bottomHBoxLayout->addWidget(income.listView);
        income.listView->show();
    }
    if(section == &cos) 
    {
        cos.bottomHBoxLayout->addWidget(cos.listView);
        cos.listView->show();
    }
    if(section == &expenses) 
    {
        expenses.bottomHBoxLayout->addWidget(expenses.listView);
        expenses.listView->show();
    }
}

void IncomeRetailEditor::addElement(int category)
{
    dialog.main = new QDialog;
    switch((Database::editorCategories)category)
    {
        case Database::incomeRetailIncome:
            dialog.main->setCaption("Add to Income");
            break;
        case Database::incomeRetailCOS:
            dialog.main->setCaption("Add to Cost Of Sales");
            break;
        case Database::incomeRetailExpenses:
            dialog.main->setCaption("Add to Expenses");
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

void IncomeRetailEditor::oneAccountToggled()
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

void IncomeRetailEditor::rangeAccountToggled()
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

void IncomeRetailEditor::addIncome()
{
    addElement((int)Database::incomeRetailIncome);
    updateSection(&income);
}

void IncomeRetailEditor::addCOS()
{
    addElement((int)Database::incomeRetailCOS);
    updateSection(&cos);
}

void IncomeRetailEditor::addExpenses()
{
    addElement((int)Database::incomeRetailExpenses);
    updateSection(&expenses);
}

void IncomeRetailEditor::updateAccounts()
{
    accounts = db->getAccountsList();
}
