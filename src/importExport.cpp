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

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qfiledialog.h>
#include <qtextstream.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <qtable.h>
#include <qptrlist.h>
#include <qradiobutton.h>
#include <qvbuttongroup.h>

#include "importExport.h"
#include "settings.h"
#include "accountsData.h"
#include "journalData.h"
#include "reportsGenerator.h"
#include "reportsPDFDialog.h"
#include "config.h"

bool ImportExport::importCSV(const ImportExport::DataCategory cat)
{
    Settings *settings = Settings::instance();
        
    QString fileName = QFileDialog::getOpenFileName(
                    settings->getImportPath(),
                    "Comma Separated Values (*.*)",
                    0, "importExport::import_file_dialog",
                    "Import CSV Data");

    if(fileName != QString::null)
    {
        int dirPos = fileName.findRev("/");
        if(dirPos == -1)
            dirPos = fileName.findRev("\\");

        settings->setImportPath(fileName.left(dirPos));

        QFile file(fileName);
        if(file.open(IO_ReadOnly))
        {
            QTextStream in(&file);
            in.setEncoding(QTextStream::Locale);

            // Determine if the first line is data or field descriptors
            bool skipFirst = false;

            QStringList data;
            QPtrList<QStringList> values;
            QStringList *list;

            int i = 0;
            QString line;
            while(!in.atEnd())
            {
                line = in.readLine();
                data << line;

                if(i < 3)
                {
                    list = new QStringList(QStringList::split(",", line));
                    values.append(list);
                }
                ++i;
            }
            
            if(!values.isEmpty())
            {
                QDialog dialog;
                dialog.setCaption("Import CSV Data");

                QVBoxLayout vBoxLayout(&dialog, 5, 5);
                QLabel label("<nobr>The first three records of the file "
                             "are shown.</nobr><br>"
                             "<nobr>Choose whether the first row is "
                             "a header or data.</nobr>", &dialog);

                QTable dataTable(3, values.first()->count(), &dialog);
                dataTable.setLeftMargin(0);
                dataTable.setTopMargin(0);
                dataTable.setSelectionMode(QTable::NoSelection);

                int row = 0;
                int col = 0;
                for(list = values.first(); list; list = values.next())
                {
                    col = 0;
                    for(QStringList::Iterator it = list->begin();
                        it != list->end(); ++it)
                    {
                        dataTable.setItem(row, col,
                            new QTableItem(&dataTable, QTableItem::Never, *it));
                        ++col;
                    }
                    ++row;
                }
                dataTable.setColumnStretchable((col - 1), true);

                QVButtonGroup buttonGroup(&dialog);
                QRadioButton headerButton("The first row is a header",
                                &buttonGroup);
                headerButton.setChecked(true);
                QRadioButton dataButton("The first row is data",
                                &buttonGroup);
                QWidget okCancelWidget(&dialog);
                QHBoxLayout okCancelLayout(&okCancelWidget);
                okCancelLayout.setSpacing(5);

                QPushButton okButton(QIconSet(QPixmap::fromMimeSource(
                                ICON_PATH + "/okButton.png")),
                                "OK", &okCancelWidget);
                connect(&okButton, SIGNAL(clicked()),
                                &dialog, SLOT(accept()));
                QPushButton cancelButton(QIconSet(QPixmap::fromMimeSource(
                                ICON_PATH + "/cancelButton.png")),
                                "Cancel", &okCancelWidget);
                connect(&cancelButton, SIGNAL(clicked()),
                                &dialog, SLOT(reject()));

                okCancelLayout.addStretch();
                okCancelLayout.addWidget(&okButton);
                okCancelLayout.addWidget(&cancelButton);

                vBoxLayout.addWidget(&label);
                vBoxLayout.addWidget(&dataTable);
                vBoxLayout.addWidget(&buttonGroup);
                vBoxLayout.addSpacing(10);
                vBoxLayout.addWidget(&okCancelWidget);

                if(dialog.exec())
                {
                    if(headerButton.isChecked())
                        skipFirst = true;
                }
                else
                {
                    return false;
                }
            }

            file.reset();
            if(skipFirst)
                in.readLine();  // throw-away line

            if(cat == Accounts)
            {
                AccountsData *accountsData = AccountsData::instance();
                accountsData->importCSV(in);
            }
            else if(cat == JournalTmp)
            {
                JournalData *journalData = JournalData::instance();
                journalData->importCSV(in);
            }

            return true;
        }

        return false;
    }

    return false;
}

void ImportExport::exportCSV(const ImportExport::DataCategory cat)
{
    Settings *settings = Settings::instance();

    QString fileName = QFileDialog::getSaveFileName(
                    settings->getExportPath(),
                    "Comma Separated Values (*.*)",
                    0, "importExport::export_file_dialog",
                    "Export CSV Data");

    if(fileName != QString::null)
    {
        int dirPos = fileName.findRev("/");
        if(dirPos == -1)
            dirPos = fileName.findRev("\\");

        settings->setExportPath(fileName.left(dirPos));

        QFile file(fileName);

        if(file.exists())
        {
            if(!overwriteDialog())
            {
                return exportCSV(cat);
            }
        }
        if(file.open(IO_WriteOnly))
        {
            QTextStream out(&file);
            out.setEncoding(QTextStream::Locale);

            if(cat == Accounts)
            {
                AccountsData *accountsData = AccountsData::instance();
                accountsData->exportCSV(out);
            }
            else if(cat == JournalTmp)
            {
                JournalData *journalData = JournalData::instance();
                journalData->exportCSV(out);
            }
        }
    }
}

void ImportExport::exportCSV(const QString data)
{
    Settings *settings = Settings::instance();

    QString fileName = QFileDialog::getSaveFileName(
                    settings->getExportPath(),
                    "Comma Separated Values (*.*)",
                    0, "importExport::export_file_dialog",
                    "Export CSV Data");

    if(fileName != QString::null)
    {
        int dirPos = fileName.findRev("/");
        if(dirPos == -1)
            dirPos = fileName.findRev("\\");

        settings->setExportPath(fileName.left(dirPos));

        QFile file(fileName);

        if(file.exists())
        {
            if(!overwriteDialog())
            {
                return exportCSV(data);
            }
        }
        if(file.open(IO_WriteOnly))
        {
            QTextStream out(&file);
            out.setEncoding(QTextStream::Locale);

            out << data;
        }
        file.close();
    }
}

void ImportExport::exportPDF(const QString &text)
{
    Settings *settings = Settings::instance();

    QString file = QFileDialog::getSaveFileName(
        settings->getPDFExportPath(),
        "PDF Files (*.pdf)",
        0, "importExport::export_pdf_dialog",
        "Export Report to PDF");

    if(file != "")
    {
        int dirPos = file.findRev("/");
        if(dirPos == -1)
            dirPos = file.findRev("\\");

        settings->setPDFExportPath(file.left(dirPos));

        if(file.right(4) != ".pdf")
            file.append(".pdf");

        QFile pdfFile(file);
        if(pdfFile.exists())
        {
            if(!overwriteDialog())
            {
                return exportPDF(text);
            }
        }

        ReportsPDFDialog reportsPDFDialog;
        QString options = reportsPDFDialog.exec();
        if(options == QString::null)
            return;

        QFile htmlFile(settings->getClientPath() + "/tmp.html");
        if(!htmlFile.open( IO_WriteOnly ))
            return;

        QTextStream tmpStream(&htmlFile);
        QString alteredText = text;
        alteredText.replace("<tr><td> </td></tr>", "<tr><td>&nbsp;<br></td></tr>");
        tmpStream << alteredText;

        tmpStream.unsetDevice();
        htmlFile.close();

        system(QString(HTMLDOC_PATH + " --footer ./. --fontsize 9.0 "
               + options +
               "--right .5in --left .5in --top .2in "
               "--textfont helvetica --no-toc --no-title --quiet -f \""
               + file + "\" \"" + settings->getClientPath() + "/tmp.html\""));

        remove(settings->getClientPath() + "/tmp.html");
    }
}

// Private

ImportExport::ImportExport() {}

ImportExport::~ImportExport() {}

bool ImportExport::overwriteDialog()
{
    QDialog *dialog  = new QDialog(0, "importExport::dialog");
    dialog->setCaption("File Exists");

    QVBoxLayout *vBoxLayout = new QVBoxLayout(dialog, 5, 5,
        "importExport::vBoxLayout");

    QLabel *label = new QLabel("<nobr>The file you chose already exists.</nobr><br>"
        "<nobr>Would you like to overwrite this file?</nobr>", dialog,
        "importExport::label");
    vBoxLayout->addWidget(label);

    vBoxLayout->addSpacing(10);

    QHBoxLayout *hBoxLayout = new QHBoxLayout(vBoxLayout, 5,
        "importExport::hBoxLayout");

    QPushButton *overwriteButton = new QPushButton(QIconSet(
        QPixmap::fromMimeSource(ICON_PATH + "/deleteButton.png")),
        "Overwrite", dialog, "importExport::overwriteButton");
    connect(overwriteButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    overwriteButton->setDefault(false);

    QPushButton *cancelButton = new QPushButton(QIconSet(
        QPixmap::fromMimeSource(ICON_PATH + "/cancelButton.png")),
        "Cancel", dialog, "importExport::cancelButton");
    connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));
    cancelButton->setDefault(true);

    hBoxLayout->addStretch();
    hBoxLayout->addWidget(overwriteButton);
    hBoxLayout->addWidget(cancelButton);

    bool retVal = dialog->exec();
    delete dialog;
    return retVal;
}


