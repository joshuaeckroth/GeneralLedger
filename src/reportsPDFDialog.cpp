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

#include <qstring.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qcheckbox.h>

#include "config.h"
#include "reportsPDFDialog.h"
#include "settings.h"

ReportsPDFDialog::ReportsPDFDialog() : QObject(0)
{
    settings = Settings::instance();
}

QString ReportsPDFDialog::exec()
{
    QString string = QString::null;

    int PDFPaperSize = settings->getPDFPaperSizeInt();
    QString PDFPaperSizeWidth = settings->getPDFPaperSizeWidth();
    QString PDFPaperSizeHeight = settings->getPDFPaperSizeHeight();
    int PDFPaperSizeUnits = settings->getPDFPaperSizeUnits();

    bool PDFPassword = settings->getPDFPasswordBool();
    QString PDFPasswordString = QString::null;

    bool PDFPermissionsNoPrint = settings->getPDFPermissionsNoPrint();
    bool PDFPermissionsNoModify = settings->getPDFPermissionsNoModify();
    bool PDFPermissionsNoCopy = settings->getPDFPermissionsNoCopy();
    bool PDFPermissionsNoAnnotate = settings->getPDFPermissionsNoAnnotate();

    question.dialog = new QDialog(0);
    question.dialog->setCaption("PDF Options");

    question.vBoxLayout = new QVBoxLayout(question.dialog);
    question.vBoxLayout->setMargin(5);
    question.vBoxLayout->setSpacing(5);

    question.topLabel = new QLabel("<nobr>Would you like to change your default PDF Options?</nobr>", question.dialog);
    question.vBoxLayout->addWidget(question.topLabel);

    question.vBoxLayout->addSpacing(10);

    question.defaultLabel = new QLabel("Defaults:", question.dialog);
    question.vBoxLayout->addWidget(question.defaultLabel);

    question.defaultHBoxLayout = new QHBoxLayout(question.vBoxLayout);
    question.defaultHBoxLayout->addSpacing(5);

    question.defaultVBoxLayout = new QVBoxLayout(question.defaultHBoxLayout);
    question.defaultVBoxLayout->setSpacing(5);

    question.defaultPaperSizeLabel = new QLabel(question.dialog);

    if(PDFPaperSize == 3)
    {
        QString paperSize("Paper Size: Custom (" + settings->getPDFPaperSizeWidth() + " x "
                          + settings->getPDFPaperSizeHeight());
        if(PDFPaperSizeUnits == 0)
            paperSize += "in)";
        if(PDFPaperSizeUnits == 1)
            paperSize += "cm)";
        else
            paperSize += "mm)";

        question.defaultPaperSizeLabel->setText(paperSize);
    }
    else
        question.defaultPaperSizeLabel->setText("Paper Size: " + settings->getPDFPaperSizeString());

    question.defaultPasswordLabel = new QLabel("Password Protection: " + settings->getPDFPasswordString(), question.dialog);
    question.defaultPermissionsLabel = new QLabel("Permissions: " + settings->getPDFPermissionsString(), question.dialog);

    question.defaultVBoxLayout->addWidget(question.defaultPaperSizeLabel);
    question.defaultVBoxLayout->addWidget(question.defaultPasswordLabel);
    question.defaultVBoxLayout->addWidget(question.defaultPermissionsLabel);

    question.vBoxLayout->addSpacing(10);

    question.buttonHBoxLayout = new QHBoxLayout(question.vBoxLayout);
    question.buttonHBoxLayout->setSpacing(5);

    question.changeOptionsButton = new QPushButton(
            QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/changeOptionsButton.png")),
            "Change Options", question.dialog);
    question.changeOptionsButton->setDefault(false);
    connect(question.changeOptionsButton, SIGNAL(clicked()), question.dialog, SLOT(accept()));

    question.useDefaultsButton = new QPushButton(
            QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/okButton.png")),
            "Use Defaults", question.dialog);
    question.useDefaultsButton->setDefault(true);
    connect(question.useDefaultsButton, SIGNAL(clicked()), question.dialog, SLOT(reject()));

    question.buttonHBoxLayout->addStretch();
    question.buttonHBoxLayout->addWidget(question.changeOptionsButton);
    question.buttonHBoxLayout->addWidget(question.useDefaultsButton);

    if(question.dialog->exec())
    {
        options.dialog = new QDialog(0);
        options.dialog->setCaption("PDF Options");

        options.vBoxLayout = new QVBoxLayout(options.dialog);
        options.vBoxLayout->setMargin(5);
        options.vBoxLayout->setSpacing(5);

        options.paperSizeLabel = new QLabel("Paper Size:", options.dialog);
        options.vBoxLayout->addWidget(options.paperSizeLabel);

        options.paperSizeHBoxLayout = new QHBoxLayout(options.vBoxLayout);
        options.paperSizeHBoxLayout->addSpacing(5);

        options.paperSizeVBoxLayout = new QVBoxLayout(options.paperSizeHBoxLayout);
        options.paperSizeVBoxLayout->setSpacing(5);

        options.paperSizeComboHBoxLayout = new QHBoxLayout(options.paperSizeVBoxLayout);

        options.paperSizeComboBox = new QComboBox(options.dialog);
        options.paperSizeComboBox->insertStringList(
                QStringList("Letter (8.5 x 11in)") << "A4 (8.27 x 11.69in)"
                << "Universal (8.27 x 11in)" << "Custom");
        connect(options.paperSizeComboBox, SIGNAL(activated(int)), this, SLOT(paperSizeChanged(int)));
        options.paperSizeComboBox->setCurrentItem(PDFPaperSize);

        options.paperSizeComboHBoxLayout->addWidget(options.paperSizeComboBox);
        options.paperSizeComboHBoxLayout->addStretch();

        options.paperSizeCustomHBoxLayout = new QHBoxLayout(options.paperSizeVBoxLayout);
        options.paperSizeCustomHBoxLayout->setSpacing(5);

        options.paperSizeCustomWidthLineEdit = new QLineEdit(options.dialog);
        options.paperSizeCustomWidthLineEdit->setMaxLength(7);
        if(PDFPaperSize == 3)
            options.paperSizeCustomWidthLineEdit->setText(PDFPaperSizeWidth);
        else
            options.paperSizeCustomWidthLineEdit->setEnabled(false);
        options.paperSizeCustomHBoxLayout->addWidget(options.paperSizeCustomWidthLineEdit);

        options.paperSizeCustomBetweenLabel = new QLabel("x", options.dialog);
        if(PDFPaperSize != 3)
            options.paperSizeCustomBetweenLabel->setEnabled(false);
        options.paperSizeCustomHBoxLayout->addWidget(options.paperSizeCustomBetweenLabel);

        options.paperSizeCustomHeightLineEdit = new QLineEdit(options.dialog);
        options.paperSizeCustomHeightLineEdit->setMaxLength(7);
        if(PDFPaperSize == 3)
            options.paperSizeCustomHeightLineEdit->setText(PDFPaperSizeHeight);
        else
            options.paperSizeCustomHeightLineEdit->setEnabled(false);
        options.paperSizeCustomHBoxLayout->addWidget(options.paperSizeCustomHeightLineEdit);

        options.paperSizeCustomUnitsComboBox = new QComboBox(options.dialog);
        options.paperSizeCustomUnitsComboBox->insertStringList(
                QStringList("in") << "cm" << "mm");
        if(PDFPaperSize == 3)
            options.paperSizeCustomUnitsComboBox->setCurrentItem(PDFPaperSizeUnits);
        else
            options.paperSizeCustomUnitsComboBox->setEnabled(false);
        options.paperSizeCustomHBoxLayout->addWidget(options.paperSizeCustomUnitsComboBox);

        options.paperSizeCustomHBoxLayout->addStretch();

        options.vBoxLayout->addSpacing(10);


        options.passwordLabel = new QLabel("Password Protection:", options.dialog);
        options.vBoxLayout->addWidget(options.passwordLabel);

        options.passwordHBoxLayout = new QHBoxLayout(options.vBoxLayout);
        options.passwordHBoxLayout->addSpacing(5);

        options.passwordVBoxLayout = new QVBoxLayout(options.passwordHBoxLayout);
        options.passwordVBoxLayout->setSpacing(5);

        options.passwordNoRadioButton = new QRadioButton("No Password", options.dialog);
        connect(options.passwordNoRadioButton, SIGNAL(clicked()), this, SLOT(passwordNoClicked()));
        options.passwordVBoxLayout->addWidget(options.passwordNoRadioButton);

        options.passwordYesRadioButton = new QRadioButton("Use Password", options.dialog);
        connect(options.passwordYesRadioButton, SIGNAL(clicked()), this, SLOT(passwordYesClicked()));
        options.passwordVBoxLayout->addWidget(options.passwordYesRadioButton);

        if(PDFPassword)
            options.passwordYesRadioButton->setChecked(true);
        else
            options.passwordNoRadioButton->setChecked(true);

        options.passwordYesHBoxLayout = new QHBoxLayout(options.passwordVBoxLayout);
        options.passwordYesHBoxLayout->addSpacing(5);

        options.passwordYesLabel = new QLabel("<nobr>(You will be asked to enter this password</nobr>"
                                              "<br>after you press \"Create PDF\")",
                                              options.dialog);
        options.passwordYesHBoxLayout->addWidget(options.passwordYesLabel);

        options.vBoxLayout->addSpacing(10);


        options.permissionsLabel = new QLabel("Permissions:", options.dialog);
        options.vBoxLayout->addWidget(options.permissionsLabel);

        options.permissionsHBoxLayout = new QHBoxLayout(options.vBoxLayout);
        options.permissionsHBoxLayout->addSpacing(5);

        options.permissionsGridLayout = new QGridLayout(options.permissionsHBoxLayout, 2, 2, 5);

        options.permissionsNoPrint = new QCheckBox("No-Print", options.dialog);
        if(PDFPermissionsNoPrint)
            options.permissionsNoPrint->setChecked(true);

        options.permissionsNoModify = new QCheckBox("No-Modify", options.dialog);
        if(PDFPermissionsNoModify)
            options.permissionsNoModify->setChecked(true);

        options.permissionsNoCopy = new QCheckBox("No-Copy", options.dialog);
        if(PDFPermissionsNoCopy)
            options.permissionsNoCopy->setChecked(true);

        options.permissionsNoAnnotate = new QCheckBox("No-Annotate", options.dialog);
        if(PDFPermissionsNoAnnotate)
            options.permissionsNoAnnotate->setChecked(true);

        options.permissionsGridLayout->addWidget(options.permissionsNoPrint, 0, 0);
        options.permissionsGridLayout->addWidget(options.permissionsNoModify, 0, 1);
        options.permissionsGridLayout->addWidget(options.permissionsNoCopy, 1, 0);
        options.permissionsGridLayout->addWidget(options.permissionsNoAnnotate, 1, 1);

        options.vBoxLayout->addSpacing(10);


        options.buttonHBoxLayout = new QHBoxLayout(options.vBoxLayout);
        options.buttonHBoxLayout->setSpacing(5);

        options.buttonCreate = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/exportPDF.png")),
                                               "Create PDF", options.dialog);
        connect(options.buttonCreate, SIGNAL(clicked()), options.dialog, SLOT(accept()));

        options.buttonCancel = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/cancelButton.png")),
                                               "Cancel", options.dialog);
        connect(options.buttonCancel, SIGNAL(clicked()), options.dialog, SLOT(reject()));

        options.buttonHBoxLayout->addStretch();
        options.buttonHBoxLayout->addWidget(options.buttonCreate);
        options.buttonHBoxLayout->addWidget(options.buttonCancel);

        if(options.dialog->exec())
        {
            PDFPaperSize = options.paperSizeComboBox->currentItem();
            if(PDFPaperSize == 3)
            {
                PDFPaperSizeWidth = options.paperSizeCustomWidthLineEdit->text();
                PDFPaperSizeHeight = options.paperSizeCustomHeightLineEdit->text();
                PDFPaperSizeUnits = options.paperSizeCustomUnitsComboBox->currentItem();
            }

            PDFPassword = options.passwordYesRadioButton->isChecked();

            PDFPermissionsNoPrint = options.permissionsNoPrint->isChecked();
            PDFPermissionsNoModify = options.permissionsNoModify->isChecked();
            PDFPermissionsNoCopy = options.permissionsNoCopy->isChecked();
            PDFPermissionsNoAnnotate = options.permissionsNoAnnotate->isChecked();

            delete options.dialog;
            delete question.dialog;
        }
        else
        {
            delete options.dialog;
            delete question.dialog;
            return string;
        }
    }

    if(PDFPassword)
    {
        password.dialog = new QDialog(0);

        password.vBoxLayout = new QVBoxLayout(password.dialog);
        password.vBoxLayout->setMargin(5);
        password.vBoxLayout->setSpacing(5);

        password.label = new QLabel("<nobr>Enter a password to protect the PDF document.</nobr>", password.dialog);
        password.label2 = new QLabel("Note: If you wish not to use a password,<br>leave both fields blank.", password.dialog);

        password.vBoxLayout->addWidget(password.label);
        password.vBoxLayout->addSpacing(5);
        password.vBoxLayout->addWidget(password.label2);

        password.vBoxLayout->addSpacing(10);

        password.gridLayout = new QGridLayout(password.vBoxLayout, 2, 2, 5);  // rows, cols, spacing

        password.firstLabel = new QLabel("Password:", password.dialog);
        password.firstLineEdit = new QLineEdit(password.dialog);
        password.firstLineEdit->setMaxLength(20);
        password.firstLineEdit->setEchoMode(QLineEdit::Password);

        password.secondLabel = new QLabel("Repeat:", password.dialog);
        password.secondLineEdit = new QLineEdit(password.dialog);
        password.secondLineEdit->setMaxLength(20);
        password.secondLineEdit->setEchoMode(QLineEdit::Password);

        password.gridLayout->addWidget(password.firstLabel, 0, 0);
        password.gridLayout->addWidget(password.firstLineEdit, 0, 1);
        password.gridLayout->addWidget(password.secondLabel, 1, 0, Qt::AlignRight | Qt::AlignVCenter);
        password.gridLayout->addWidget(password.secondLineEdit, 1, 1);

        password.vBoxLayout->addSpacing(10);

        password.hBoxLayout = new QHBoxLayout(password.vBoxLayout);
        password.hBoxLayout->setSpacing(5);

        password.createButton = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/exportPDF.png")),
                                                "Create PDF", password.dialog);
        connect(password.createButton, SIGNAL(clicked()), password.dialog, SLOT(accept()));

        password.cancelButton = new QPushButton(QIconSet(QPixmap::fromMimeSource(ICON_PATH + "/cancelButton.png")),
                                                "Cancel", password.dialog);
        connect(password.cancelButton, SIGNAL(clicked()), password.dialog, SLOT(reject()));

        password.hBoxLayout->addStretch();
        password.hBoxLayout->addWidget(password.createButton);
        password.hBoxLayout->addWidget(password.cancelButton);

        bool accepted = false;
        while(password.dialog->exec())
        {
            if(password.firstLineEdit->text() == password.secondLineEdit->text())
            {
                PDFPasswordString = password.firstLineEdit->text();
                accepted = true;
                break;
            }
            password.label->setText("<nobr>The passwords did not match.  Try again.</nobr>");
        }
        delete password.dialog;

        if(!accepted)
            return string;
    }

    if(PDFPaperSize == 0)
        string += "--size letter ";
    if(PDFPaperSize == 1)
        string += "--size a4 ";
    if(PDFPaperSize == 2)
        string += "8.27x11.69in ";
    if(PDFPaperSize == 3)
    {
        string += "--size " + PDFPaperSizeWidth + "x" + PDFPaperSizeHeight;
        if(PDFPaperSizeUnits == 0)
            string += "in ";
        if(PDFPaperSizeUnits == 1)
            string += "cm ";
        if(PDFPaperSizeUnits == 2)
            string += "mm ";
    }

    if(PDFPassword || PDFPermissionsNoPrint || PDFPermissionsNoModify || PDFPermissionsNoCopy || PDFPermissionsNoAnnotate)
        string += "--encryption ";

    if(PDFPassword)
        string += "--user-password \"" + PDFPasswordString + "\" ";

    if(PDFPermissionsNoPrint || PDFPermissionsNoModify || PDFPermissionsNoCopy || PDFPermissionsNoAnnotate)
    {
        if(PDFPermissionsNoPrint)
            string += "--permissions no-print ";
        if(PDFPermissionsNoModify)
            string += "--permissions no-modify ";
        if(PDFPermissionsNoCopy)
            string += "--permissions no-copy ";
        if(PDFPermissionsNoAnnotate)
            string += "--permissions no-annotate ";
    }

    settings->setPDFPaperSize(PDFPaperSize);
    settings->setPDFPaperSizeWidth(PDFPaperSizeWidth);
    settings->setPDFPaperSizeHeight(PDFPaperSizeHeight);
    settings->setPDFPaperSizeUnits(PDFPaperSizeUnits);
    settings->setPDFPassword(PDFPassword);
    settings->setPDFPermissionsNoPrint(PDFPermissionsNoPrint);
    settings->setPDFPermissionsNoModify(PDFPermissionsNoModify);
    settings->setPDFPermissionsNoCopy(PDFPermissionsNoCopy);
    settings->setPDFPermissionsNoAnnotate(PDFPermissionsNoAnnotate);

    return string;
}

void ReportsPDFDialog::paperSizeChanged(int index)
{
    if(index != 3)
    {
        options.paperSizeCustomWidthLineEdit->setEnabled(false);
        options.paperSizeCustomBetweenLabel->setEnabled(false);
        options.paperSizeCustomHeightLineEdit->setEnabled(false);
        options.paperSizeCustomUnitsComboBox->setEnabled(false);
    }
    else
    {
        options.paperSizeCustomWidthLineEdit->setEnabled(true);
        options.paperSizeCustomBetweenLabel->setEnabled(true);
        options.paperSizeCustomHeightLineEdit->setEnabled(true);
        options.paperSizeCustomUnitsComboBox->setEnabled(true);
    }
}

void ReportsPDFDialog::passwordNoClicked()
{
    options.passwordYesRadioButton->setChecked(false);
    options.passwordNoRadioButton->setChecked(true);
}

void ReportsPDFDialog::passwordYesClicked()
{
    options.passwordNoRadioButton->setChecked(false);
    options.passwordYesRadioButton->setChecked(true);
}

