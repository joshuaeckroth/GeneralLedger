#ifndef SETTINGS_H
#define SETTINGS_H

#include <qprinter.h>

class QString;

class Settings
{
    public:
        static Settings* instance();
        void destroy();

        QString getClientPath() const;
        void setClientPath(QString);
        QString getExportPath() const;
        void setExportPath(QString);
        QString getImportPath() const;
        void setImportPath(QString);

        QString getDefaultDb() const;
        void setDefaultDb(QString);
        QString getDefaultClient() const;
        void setDefaultClient(QString);

        int getDefaultWidth() const;
        void setDefaultWidth(int);
        int getDefaultHeight() const;
        void setDefaultHeight(int);
        int getDefaultX() const;
        void setDefaultX(int);
        int getDefaultY() const;
        void setDefaultY(int);

        QString getPrinterPrinterName() const;
        void setPrinterPrinterName(QString);
        bool getPrinterOutputToFile() const;
        void setPrinterOutputToFile(bool);
        QString getPrinterOutputFileName() const;
        void setPrinterOutputFileName(QString);
        QString getPrinterPrintProgram() const;
        void setPrinterPrintProgram(QString);
        QPrinter::Orientation getPrinterOrientation() const;
        void setPrinterOrientation(QPrinter::Orientation);
        QPrinter::PageSize getPrinterPageSize() const;
        void setPrinterPageSize(QPrinter::PageSize);
        QPrinter::PageOrder getPrinterPageOrder() const;
        void setPrinterPageOrder(QPrinter::PageOrder);
        QPrinter::ColorMode getPrinterColorMode() const;
        void setPrinterColorMode(QPrinter::ColorMode);
        QPrinter::PaperSource getPrinterPaperSource() const;
        void setPrinterPaperSource(QPrinter::PaperSource);

        int getPDFPaperSizeInt() const;
        QString getPDFPaperSizeString() const;
        void setPDFPaperSize(int);

        QString getPDFPaperSizeWidth() const;
        void setPDFPaperSizeWidth(QString);
        QString getPDFPaperSizeHeight() const;
        void setPDFPaperSizeHeight(QString);
        int getPDFPaperSizeUnits() const;
        void setPDFPaperSizeUnits(int);

        bool getPDFPasswordBool() const;
        QString getPDFPasswordString() const;
        void setPDFPassword(bool);

        QString getPDFPermissionsString() const;

        bool getPDFPermissionsNoPrint() const;
        void setPDFPermissionsNoPrint(bool);

        bool getPDFPermissionsNoModify() const;
        void setPDFPermissionsNoModify(bool);

        bool getPDFPermissionsNoCopy() const;
        void setPDFPermissionsNoCopy(bool);

        bool getPDFPermissionsNoAnnotate() const;
        void setPDFPermissionsNoAnnotate(bool);

    private:
        Settings();
        ~Settings();

        static Settings *settings;

        QString clientPath;
        QString exportPath;
        QString importPath;

        QString defaultDb;
        QString defaultClient;

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
