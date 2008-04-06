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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <qprinter.h>

class QString;

/// Access user settings
class Settings
{
    public:
        /// Returns the single instance of this class
        /// 
        /// If the instance doesn't exist, this function creates it.
        /// The instance should not yet exist only when called from main.cpp.
        static Settings* instance();

        /// Destroys the single instance
        ///
        /// This should only occur in main.cpp
        void destroy();

        /// Get the path where database files are stored
        ///
        /// \note
        /// This doesn't make much sense if the database files are not stored
        /// singularly.
        QString getClientPath() const;

        /// Set the path where database files are stored
        void setClientPath(QString);

        /// Get the most recently used path for database exporting
        QString getExportPath() const;

        /// Set the most recently used path for database exporting
        void setExportPath(QString);

        /// Get the most recently used path for database importing
        QString getImportPath() const;

        /// Set the most recently used path for database importing
        void setImportPath(QString);

        /// Get the last opened client's id (specific to the database setup)
        QString getDefaultClientId() const;

        /// Set the last opened client's id (specific to the database setup)
        void setDefaultClientId(QString);

        /// Get the last opened client name
        QString getDefaultClientName() const;

        /// Set the last opened client name
        void setDefaultClientName(QString);

        /// Get the default window width
        ///
        /// \note
        /// This value will be 10000 if the window was maximized
        int getDefaultWidth() const;

        /// Set the default window width (set upon closing)
        ///
        /// \note
        /// Set this value to 10000 if the window is maximized
        void setDefaultWidth(int);

        /// Get the default window height
        ///
        /// \note
        /// This value will be 10000 if the window was maximized
        int getDefaultHeight() const;

        /// Set the default window height (set upon closing)
        ///
        /// \note
        /// Set this value to 10000 if the window is maximized
        void setDefaultHeight(int);

        /// Get the default window X position
        ///
        /// \note
        /// This value will be 10000 if the window was maximized
        int getDefaultX() const;
        
        /// Set the default window X position (set upon closing)
        ///
        /// \note
        /// Set this value to 10000 if the window is maximized
        void setDefaultX(int);

        /// Get the default window Y position
        ///
        /// \note
        /// This value will be 10000 if the window was maximized
        int getDefaultY() const;

        /// Set the default window Y position
        ///
        /// \note
        /// Set this value to 10000 if the window is maximized
        void setDefaultY(int);

        /// Get the default printer name
        QString getPrinterPrinterName() const;

        /// Set the default printer name
        void setPrinterPrinterName(QString);

        /// Get the default printer output-to-file yes/no value (Unix only)
        bool getPrinterOutputToFile() const;

        /// Set the default printer output-to-file yes/no value (Unix only)
        void setPrinterOutputToFile(bool);

        /// Get the default printer output-to-file filename (Unix only)
        QString getPrinterOutputFileName() const;

        /// Set the default printer output-to-file filename (Unix only)
        void setPrinterOutputFileName(QString);

        /// Get the default printer program (ie, lpr) (Unix only)
        QString getPrinterPrintProgram() const;

        /// Set the default printer program (ie, lpr) (Unix only)
        void setPrinterPrintProgram(QString);

        /// Get the default printer page orientation
        QPrinter::Orientation getPrinterOrientation() const;

        /// Set the default printer page orientation
        void setPrinterOrientation(QPrinter::Orientation);

        /// Get the default printer page size
        QPrinter::PageSize getPrinterPageSize() const;

        /// Set the default printer page size
        void setPrinterPageSize(QPrinter::PageSize);

        /// Get the default printer page order
        QPrinter::PageOrder getPrinterPageOrder() const;

        /// Set the default printer page order
        void setPrinterPageOrder(QPrinter::PageOrder);

        /// Get the default printer color mode
        QPrinter::ColorMode getPrinterColorMode() const;

        /// Set the default printer color mode
        void setPrinterColorMode(QPrinter::ColorMode);

        /// Get the default printer paper source
        QPrinter::PaperSource getPrinterPaperSource() const;

        /// Set the default printer paper source
        void setPrinterPaperSource(QPrinter::PaperSource);

        QString getPDFExportPath() const;

        void setPDFExportPath(QString newPDFExportPath);

        /// Get the default PDF paper size specifier
        ///
        /// The return type, int, specifies:
        /// 0 = Letter, 1 = A4, 2 = Universal, 3 = Custom
        int getPDFPaperSizeInt() const;

        /// Get the default PDF paper size as a string
        QString getPDFPaperSizeString() const;

        /// Set the default PDF paper size specifier
        void setPDFPaperSize(int);

        /// Get the default PDF paper size width
        ///
        /// This value will be a decimal number (ie, "10.0") that corresponds
        /// to Settings::getPDFPaperSizeUnits().
        /// \note
        /// This value only makes sense if Settings::getPDFPaperSizeInt() == 3
        QString getPDFPaperSizeWidth() const;

        /// Set the default PDF paper size width
        ///
        /// This value will be a decimal number (ie, "10.0") that corresponds
        /// to Settings::getPDFPaperSizeUnits().
        /// \note
        /// This value only makes sense if Settings::getPDFPaperSizeInt() == 3
        void setPDFPaperSizeWidth(QString);

        /// Get the default PDF paper size height
        ///
        /// This value will be a decimal number (ie, "10.0") that corresponds
        /// to Settings::getPDFPaperSizeUnits().
        /// \note
        /// This value only makes sense if Settings::getPDFPaperSizeInt() == 3
        QString getPDFPaperSizeHeight() const;

        /// Set the default PDF paper size height
        ///
        /// This value will be a decimal number (ie, "10.0") that corresponds
        /// to Settings::getPDFPaperSizeUnits().
        /// \note
        /// This value only makes sense if Settings::getPDFPaperSizeInt() == 3
        void setPDFPaperSizeHeight(QString);

        /// Get the default PDF paper size units
        ///
        /// This value will be a unit of measurement, such as "in" or "cm"
        /// \note
        /// This value only makes sense if Settings::getPDFPaperSizeInt() == 3
        int getPDFPaperSizeUnits() const;

        /// Set the default PDF paper size units
        ///
        /// This value will be a unit of measurement, such as "in" or "cm"
        /// \note
        /// This value only makes sense if Settings::getPDFPaperSizeInt() == 3
        void setPDFPaperSizeUnits(int);

        /// Get the default PDF password option
        ///
        /// \note
        /// True = use a password, False = do not use a password
        bool getPDFPasswordBool() const;

        /// Get the default PDF password option as a string
        QString getPDFPasswordString() const;

        /// Set the default PDF password option
        ///
        /// \note
        /// True = use a password, False = do not use a password
        void setPDFPassword(bool);

        /// Get the default PDF permissions as a string
        QString getPDFPermissionsString() const;

        /// Get the default PDF permission NoPrint as a yes/no value
        bool getPDFPermissionsNoPrint() const;

        /// Set the default PDF permission NoPrint as a yes/no value
        void setPDFPermissionsNoPrint(bool);

        /// Get the default PDF permission NoModify as a yes/no value
        bool getPDFPermissionsNoModify() const;

        /// Set the default PDF permission NoModify as a yes/no value
        void setPDFPermissionsNoModify(bool);

        /// Get the default PDF permission NoCopy as a yes/no value
        bool getPDFPermissionsNoCopy() const;

        /// Set the default PDF permission NoCopy as a yes/no value
        void setPDFPermissionsNoCopy(bool);

        /// Get the default PDF permission NoAnnotate as a yes/no value
        bool getPDFPermissionsNoAnnotate() const;

        /// Set the default PDF permission NoAnnotate as a yes/no value
        void setPDFPermissionsNoAnnotate(bool);

    private:
        Settings();
        ~Settings();

        static Settings *settings;

        QString clientPath;
        QString exportPath;
        QString importPath;

        QString defaultClientId;
        QString defaultClientName;

        int defaultWidth;
        int defaultHeight;
        int defaultX;
        int defaultY;

        QString printerPrinterName;
        bool printerOutputToFile;
        QString printerOutputFileName;
        QString printerPrintProgram;
        QPrinter::Orientation printerOrientation;
        QPrinter::PageSize printerPageSize;
        QPrinter::PageOrder printerPageOrder;
        QPrinter::ColorMode printerColorMode;
        QPrinter::PaperSource printerPaperSource;

        QString PDFExportPath;
        int PDFPaperSize;
        QString PDFPaperSizeWidth;
        QString PDFPaperSizeHeight;
        int PDFPaperSizeUnits;
        bool PDFPassword;
        bool PDFPermissionsNoPrint;
        bool PDFPermissionsNoModify;
        bool PDFPermissionsNoCopy;
        bool PDFPermissionsNoAnnotate;

};

#endif

