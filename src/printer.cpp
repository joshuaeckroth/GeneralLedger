#include <cmath>
using std::ceil;

#include <qprinter.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qrect.h>
#include <qfont.h>
#include <qpalette.h>
#include <qsimplerichtext.h>
#include <qstring.h>
#include <qstringlist.h>

#include "printer.h"
#include "settings.h"

Printer::Printer() : QPrinter(QPrinter::HighResolution)
{
    settings = Settings::instance();
    setPrinterName(settings->getPrinterPrinterName());
    setOutputToFile(settings->getPrinterOutputToFile());
    setOutputFileName(settings->getPrinterOutputFileName());
    setPrintProgram(settings->getPrinterPrintProgram());
    setOrientation(settings->getPrinterOrientation());
    setPageSize(settings->getPrinterPageSize());
    setPageOrder(settings->getPrinterPageOrder());
    setColorMode(settings->getPrinterColorMode());
    setPaperSource(settings->getPrinterPaperSource());
}

Printer::~Printer()
{
    settings->setPrinterPrinterName(printerName());
    settings->setPrinterOutputToFile(outputToFile());
    settings->setPrinterOutputFileName(outputFileName());
    settings->setPrinterPrintProgram(printProgram());
    settings->setPrinterOrientation(orientation());
    settings->setPrinterPageSize(pageSize());
    settings->setPrinterPageOrder(pageOrder());
    settings->setPrinterColorMode(colorMode());
    settings->setPrinterPaperSource(paperSource());
}

Printer* Printer::instance()
{
    if(printer == 0)
        printer = new Printer;
    return printer;
}

void Printer::destroy()
{
    if(printer != 0)
        delete printer;
}

void Printer::printReport(const QString &header, const QString &data)
{
    if(setup(0))
    {

        QPainter painter(this);

        QPaintDeviceMetrics metrics(this);
        int headerGap = metrics.logicalDpiY() / 4;  // quarter inch

        QFont headerFont("Verdana", 8);
        QFont dataFont("Verdana", 8);
    
        QSimpleRichText headerText(header, headerFont);
    
        headerText.setWidth(&painter, painter.window().width());

        int pageHeight = painter.window().height() - headerText.height() - headerGap;

        QSimpleRichText dataText(data, dataFont, "", 0, 0, pageHeight);
        dataText.setWidth(&painter, painter.window().width());

        int numPages = (int)ceil((double)dataText.height() / pageHeight);

        int index;

        for(int i = 0; i < (int)numCopies(); ++i)
        {
            for(int j = 0; j < numPages; ++j)
            {
                if(i > 0 || j > 0)
                    newPage();

                if(pageOrder() == QPrinter::LastPageFirst)
                    index = numPages - j - 1;
                else
                    index = j;

                QRect headerRect(0, 0, headerText.width(), headerText.height());

                QRect dataRect(0, (index * pageHeight), dataText.width(), pageHeight);
            

                headerText.draw(&painter, 0, 0, headerRect, QColorGroup());

                painter.save();
                painter.translate(0, -dataRect.top() + headerText.height());
                dataText.draw(&painter, 0, 0, dataRect, QColorGroup());
                painter.restore();

                painter.setFont(dataFont);
                painter.drawText(painter.window(), Qt::AlignHCenter | Qt::AlignBottom,
                                 QString("Page " + QString::number(j + 1) + " of " + QString::number(numPages)));

            }
        }
    }
}

Printer *Printer::printer = 0;

