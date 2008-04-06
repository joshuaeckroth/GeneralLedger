#include <qsettings.h>
#include <qprinter.h>

#include "settings.h"

Settings::Settings()
{
    // read settings
    QSettings get;
    get.setPath("eckroth.net", "GeneralLedger");
    get.beginGroup("/GeneralLedger");
    
    clientPath = get.readEntry("/paths/clientPath", "");
    exportPath = get.readEntry("/paths/exportPath", "");
    importPath = get.readEntry("/paths/importPath", "");

    defaultDb = get.readEntry("/database/defaultDb", "");
    defaultClient = get.readEntry("/database/defaultClient", "");

    defaultWidth = get.readNumEntry("/geometry/width", 700);
    defaultHeight = get.readNumEntry("/geometry/height", 500);
    defaultX = get.readNumEntry("/geometry/x", 0);
    defaultY = get.readNumEntry("/geometry/y", 0);

    helpPath = get.readEntry("/paths/helpPath", "");    
    iconPath = get.readEntry("/paths/iconPath", "");
    htmldocExec = get.readEntry("/paths/htmldocExec", "");
    aescryptExec = get.readEntry("/paths/aescryptExec", "");

    printerPrinterName = get.readEntry("/printer/printerName", "");
    printerOutputToFile = (bool)get.readNumEntry("/printer/outputToFile", 0);
    printerOutputFileName = get.readEntry("/printer/outputFileName", "");
    printerPrintProgram = get.readEntry("/printer/printProgram", "");
    printerOrientation = (QPrinter::Orientation)get.readNumEntry("/printer/orientation", 0);
    printerPageSize = (QPrinter::PageSize)get.readNumEntry("/printer/pageSize", 2);
    printerPageOrder = (QPrinter::PageOrder)get.readNumEntry("/printer/pageOrder", 0);
    printerColorMode = (QPrinter::ColorMode)get.readNumEntry("/printer/colorMode", 0);
    printerPaperSource = (QPrinter::PaperSource)get.readNumEntry("/printer/paperSource", 6);

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

    put.writeEntry("/paths/helpPath", helpPath);
    put.writeEntry("/paths/iconPath", iconPath);
    put.writeEntry("/paths/htmldocExec", htmldocExec);
    put.writeEntry("/paths/aescryptExec", aescryptExec);

    put.writeEntry("/printer/printerName", printerPrinterName);
    put.writeEntry("/printer/outputToFile", (int)printerOutputToFile);
    put.writeEntry("/printer/outputFileName", printerOutputFileName);
    put.writeEntry("/printer/printProgram", printerPrintProgram);
    put.writeEntry("/printer/orientation", (int)printerOrientation);
    put.writeEntry("/printer/pageSize", (int)printerPageSize);
    put.writeEntry("/printer/pageOrder", (int)printerPageOrder);
    put.writeEntry("/printer/colorMode", (int)printerColorMode);
    put.writeEntry("/printer/paperSource", (int)printerPaperSource);

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

QString Settings::getHelpPath() const
{
    return helpPath;
}

QString Settings::getIconPath() const
{
    return iconPath;
}

QString Settings::getHtmldocExec() const
{
    return htmldocExec;
}

QString Settings::getAescryptExec() const
{
    return aescryptExec;
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


Settings *Settings::settings = 0;
