#include <qdialog.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include "config.h"
#include "closeDialog.h"

CloseDialog::CloseDialog() : QDialog(0)
{
    setCaption("Close Client");

    vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->setMargin(5);
    vBoxLayout->setSpacing(5);

    label = new QLabel("<nobr>There are entries in the journal which have not been saved and cleared.</nobr><br>"
                       "<nobr>Would you like to save and clear these entries now?</nobr><br><br>"
                       "Note: if you do not save and clear these entries, "
                       "they will <u>not</u> be lost.  You will find them "
                       "in the same place when you open this client again.", this);

    vBoxLayout->addWidget(label);
    vBoxLayout->addSpacing(10);

    hBoxLayout = new QHBoxLayout(vBoxLayout);
    hBoxLayout->setSpacing(5);

    saveButton = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/saveAndClear.png")),
                                 "Save and Clear Entries", this);
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveAndClear()));

    dontSaveButton = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/cancelButton.png")),
                                     "Don't Save and Clear", this);
    connect(dontSaveButton, SIGNAL(clicked()), this, SLOT(dontSave()));

    returnButton = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/return.png")),
                                   "Return to Journal Entry", this);
    connect(returnButton, SIGNAL(clicked()), this, SLOT(retToEntry()));

    hBoxLayout->addStretch();
    hBoxLayout->addWidget(saveButton);
    hBoxLayout->addWidget(dontSaveButton);
    hBoxLayout->addWidget(returnButton);
}

void CloseDialog::saveAndClear()
{
    done(0);
}

void CloseDialog::dontSave()
{
    done(1);
}

void CloseDialog::retToEntry()
{
    done(2);
}
