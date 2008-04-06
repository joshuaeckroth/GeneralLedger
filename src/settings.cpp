#include <qsettings.h>
#include <qprinter.h>

#ifdef Q_OS_WIN32
#include <shlobj.h>
#endif

#include <stdlib.h>

#include "settings.h"

Settings::Settings()
{
    // read settings
    QSettings get;
    get.setPath("eckroth.net", "GeneralLedger");
    get.beginGroup("/GeneralLedger");
    
#ifdef Q_OS_WIN32
    TCHAR myDocPath[MAX_PATH];
    char myDocPath2[MAX_PATH*2];
    SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, myDocPath);
    wcstombs(myDocPath2, myDocPath, MAX_PATH * 2);
    clientPath = get.readEntry("/paths/clientPath", myDocPath2);
    exportPath = get.readEntry("/paths/exportPath", myDocPath2);
    importPath = get.readEntry("/paths/importPath", myDocPath2);
#else
    QString myHome = std::getenv("HOME");
    clientPath = get.readEntry("/paths/clientPath", myHome);
    exportPath = get.readEntry("/paths/exportPath", myHome);
    importPath = get.readEntry("/paths/importPath", myHome);
#endif

    defaultDb = get.readEntry("/database/defaultDb", "");
    defaultClient = get.readEntry("/database/defaultClient", "");

    defaultWidth = get.readNumEntry("/geometry/width", 700);
    defaultHeight = get.readNumEntry("/geometry/height", 500);
    defaultX = get.readNumEntry("/geometry/x", 0);
    defaultY = get.readNumEntry("/geometry/y", 0);

    printerPrinterName = get.readEntry("/printer/printerName", "");
    printerOutputToFile = (bool)get.readNumEntry("/printer/outputToFile", 0);
    printerOutputFileName = get.readEntry("/printer/outputFileName", "");
    printerPrintProgram = get.readEntry("/printer/printProgram", "");
    printerOrientation = (QPrinter::Orientation)get.readNumEntry("/printer/orientation", 0);
    printerPageSize = (QPrinter::PageSize)get.readNumEntry("/printer/pageSize", 2);
    printerPageOrder = (QPrinter::PageOrder)get.readNumEntry("/printer/pageOrder", 0);
    printerColorMode = (QPrinter::ColorMode)get.readNumEntry("/printer/colorMode", 0);
    printerPaperSource = (QPrinter::PaperSource)get.readNumEntry("/printer/paperSource", 6);

    PDFPaperSize = get.readNumEntry("/PDF/paperSize", 0);
    PDFPaperSizeWidth = get.readEntry("/PDF/paperSizeWidth", "");
    PDFPaperSizeHeight = get.readEntry("/PDF/paperSizeHeight", "");
    PDFPaperSizeUnits = get.readNumEntry("/PDF/paperSizeUnits", 0);
    PDFPassword = get.readNumEntry("/PDF/password", 0);
    PDFPermissionsNoPrint = get.readNumEntry("/PDF/permissionsNoPrint", 0);
    PDFPermissionsNoModify = get.readNumEntry("/PDF/permissionsNoModify", 0);
    PDFPermissionsNoCopy = get.readNumEntry("/PDF/permissionsNoCopy", 0);
    PDFPermissionsNoAnnotate = get.readNumEntry("/PDF/permissionsNoAnnotate", 0);

    get.endGroup();
    
}

Settings::~Settings()
{
    // write settings
    QSettings put;
    put.setPath("eckroth.net", "GeneralLedger");
    put.beginGroup("/GeneralLedger");

    put.writeEntry("/paths/clientPath", clientPath);
    put.writeEntry("/paths/exportPath", exportPath);
    put.writeEntry("/paths/importPath", importPath);

    put.writeEntry("/database/defaultDb", defaultDb);
    put.writeEntry("/database/defaultClient", defaultClient);

    put.writeEntry("/geometry/width", defaultWidth);
    put.writeEntry("/geometry/height", defaultHeight);
    put.writeEntry("/geometry/x", defaultX);
    put.writeEntry("/geometry/y", defaultY);

    put.writeEntry("/printer/printerName", printerPrinterName);
    put.writeEntry("/printer/outputToFile", (int)printerOutputToFile);
    put.writeEntry("/printer/outputFileName", printerOutputFileName);
    put.writeEntry("/printer/printProgram", printerPrintProgram);
    put.writeEntry("/printer/orientation", (int)printerOrientation);
    put.writeEntry("/printer/pageSize", (int)printerPageSize);
    put.writeEntry("/printer/pageOrder", (int)printerPageOrder);
    put.writeEntry("/printer/colorMode", (int)printerColorMode);
    put.writeEntry("/printer/paperSource", (int)printerPaperSource);

    put.writeEntry("/PDF/paperSize", PDFPaperSize);
    put.writeEntry("/PDF/paperSizeWidth", PDFPaperSizeWidth);
    put.writeEntry("/PDF/paperSizeHeight", PDFPaperSizeHeight);
    put.writeEntry("/PDF/paperSizeUnits", PDFPaperSizeUnits);
    put.writeEntry("/PDF/password", (int)PDFPassword);
    put.writeEntry("/PDF/permissionsNoPrint", (int)PDFPermissionsNoPrint);
    put.writeEntry("/PDF/permissionsNoModify", (int)PDFPermissionsNoModify);
    put.writeEntry("/PDF/permissionsNoCopy", (int)PDFPermissionsNoCopy);
    put.writeEntry("/PDF/permissionsNoAnnotate", (int)PDFPermissionsNoAnnotate);

    put.endGroup();
}

Settings* Settings::instance()
{
    if(settings == 0)
        settings = new Settings;
    return settings;
}

void Settings::destroy()
{
    if(settings != 0)
        delete settings;
}

QString Settings::getClientPath() const
{
    return clientPath;
}

void Settings::setClientPath(QString newClientPath)
{
    clientPath = newClientPath;
}

QString Settings::getExportPath() const
{
    return exportPath;
}

void Settings::setExportPath(QString newExportPath)
{
    exportPath = newExportPath;
}

QString Settings::getImportPath() const
{
    return importPath;
}

void Settings::setImportPath(QString newImportPath)
{
    importPath = newImportPath;
}

QString Settings::getDefaultDb() const
{
    return defaultDb;
}

void Settings::setDefaultDb(QString newDefaultDb)
{
    defaultDb = newDefaultDb;
}

QString Settings::getDefaultClient() const
{
    return defaultClient;
}

void Settings::setDefaultClient(QString newDefaultClient)
{
    defaultClient = newDefaultClient;
}

int Settings::getDefaultWidth() const
{
    return defaultWidth;
}

void Settings::setDefaultWidth(int newDefaultWidth)
{
    defaultWidth = newDefaultWidth;
}

int Settings::getDefaultHeight() const
{
    return defaultHeight;
}

void Settings::setDefaultHeight(int newDefaultHeight)
{
    defaultHeight = newDefaultHeight;
}

int Settings::getDefaultX() const
{
    return defaultX;
}

void Settings::setDefaultX(int newDefaultX)
{
    defaultX = newDefaultX;
}

int Settings::getDefaultY() const
{
    return defaultY;
}

void Settings::setDefaultY(int newDefaultY)
{
    defaultY = newDefaultY;
}

QString Settings::getPrinterPrinterName() const
{
    return printerPrinterName;
}

void Settings::setPrinterPrinterName(QString newPrinterPrinterName)
{
    printerPrinterName = newPrinterPrinterName;
}

bool Settings::getPrinterOutputToFile() const
{
    return printerOutputToFile;
}

void Settings::setPrinterOutputToFile(bool newPrinterOutputToFile)
{
    printerOutputToFile = newPrinterOutputToFile;
}

QString Settings::getPrinterOutputFileName() const
{
    return printerOutputFileName;
}

void Settings::setPrinterOutputFileName(QString newPrinterOutputFileName)
{
    printerOutputFileName = newPrinterOutputFileName;
}

QString Settings::getPrinterPrintProgram() const
{
    return printerPrintProgram;
}

void Settings::setPrinterPrintProgram(QString newPrinterPrintProgram)
{
    printerPrintProgram = newPrinterPrintProgram;
}

QPrinter::Orientation Settings::getPrinterOrientation() const
{
    return printerOrientation;
}

void Settings::setPrinterOrientation(QPrinter::Orientation newPrinterOrientation)
{
    printerOrientation = newPrinterOrientation;
}

QPrinter::PageSize Settings::getPrinterPageSize() const
{
    return printerPageSize;
}

void Settings::setPrinterPageSize(QPrinter::PageSize newPrinterPageSize)
{
    printerPageSize = newPrinterPageSize;
}

QPrinter::PageOrder Settings::getPrinterPageOrder() const
{
    return printerPageOrder;
}

void Settings::setPrinterPageOrder(QPrinter::PageOrder newPrinterPageOrder)
{
    printerPageOrder = newPrinterPageOrder;
}

QPrinter::ColorMode Settings::getPrinterColorMode() const
{
    return printerColorMode;
}

void Settings::setPrinterColorMode(QPrinter::ColorMode newPrinterColorMode)
{
    printerColorMode = newPrinterColorMode;
}

QPrinter::PaperSource Settings::getPrinterPaperSource() const
{
    return printerPaperSource;
}

void Settings::setPrinterPaperSource(QPrinter::PaperSource newPrinterPaperSource)
{
    printerPaperSource = newPrinterPaperSource;
}

int Settings::getPDFPaperSizeInt() const
{
    return PDFPaperSize;
}

QString Settings::getPDFPaperSizeString() const
{
    if(PDFPaperSize == 0)
        return "Letter (8.5 x 11in)";
    if(PDFPaperSize == 1)
        return "A4 (8.27 x 11.69in)";
    if(PDFPaperSize == 2)
        return "Universal (8.27 x 11in)";
    if(PDFPaperSize == 3)
        return "Custom";
    else
        return "";
}

void Settings::setPDFPaperSize(int newPDFPaperSize)
{
    PDFPaperSize = newPDFPaperSize;
}

QString Settings::getPDFPaperSizeWidth() const
{
    return PDFPaperSizeWidth;
}

void Settings::setPDFPaperSizeWidth(QString newPDFPaperSizeWidth)
{
    PDFPaperSizeWidth = newPDFPaperSizeWidth;
}

QString Settings::getPDFPaperSizeHeight() const
{
    return PDFPaperSizeHeight;
}

void Settings::setPDFPaperSizeHeight(QString newPDFPaperSizeHeight)
{
    PDFPaperSizeHeight = newPDFPaperSizeHeight;
}

int Settings::getPDFPaperSizeUnits() const
{
    return PDFPaperSizeUnits;
}

void Settings::setPDFPaperSizeUnits(int newPDFPaperSizeUnits)
{
    PDFPaperSizeUnits = newPDFPaperSizeUnits;
}

bool Settings::getPDFPasswordBool() const
{
    return PDFPassword;
}

QString Settings::getPDFPasswordString() const
{
    if(PDFPassword)
        return "Yes";
    else
        return "No";
}

void Settings::setPDFPassword(bool newPDFPassword)
{
    PDFPassword = newPDFPassword;
}

QString Settings::getPDFPermissionsString() const
{
    QString string("");

    if(PDFPermissionsNoPrint)
        string += "No-Print";
    
    if(PDFPermissionsNoModify)
    {
        if(string != "")
            string += ", No-Modify";
        else
            string += "No-Modify";
    }

    if(PDFPermissionsNoCopy)
    {
        if(string != "")
            string += ", No-Copy";
        else
            string += "No-Copy";
    }

    if(PDFPermissionsNoAnnotate)
    {
        if(string != "")
            string += ", No-Annotate";
        else
            string += "No-Annotate";
    }

    if(string == "")
        string += "All";

    return string;
}

bool Settings::getPDFPermissionsNoPrint() const
{
    return PDFPermissionsNoPrint;
}

void Settings::setPDFPermissionsNoPrint(bool newPDFPermissionsNoPrint)
{
    PDFPermissionsNoPrint = newPDFPermissionsNoPrint;
}

bool Settings::getPDFPermissionsNoModify() const
{
    return PDFPermissionsNoModify;
}

void Settings::setPDFPermissionsNoModify(bool newPDFPermissionsNoModify)
{
    PDFPermissionsNoModify = newPDFPermissionsNoModify;
}

bool Settings::getPDFPermissionsNoCopy() const
{
    return PDFPermissionsNoCopy;
}

void Settings::setPDFPermissionsNoCopy(bool newPDFPermissionsNoCopy)
{
    PDFPermissionsNoCopy = newPDFPermissionsNoCopy;
}

bool Settings::getPDFPermissionsNoAnnotate() const
{
    return PDFPermissionsNoAnnotate;
}

void Settings::setPDFPermissionsNoAnnotate(bool newPDFPermissionsNoAnnotate)
{
    PDFPermissionsNoAnnotate = newPDFPermissionsNoAnnotate;
}       
    

Settings *Settings::settings = 0;
