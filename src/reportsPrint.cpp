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

#include "settings.h"
#include "reportsPrint.h"

ReportsPrint *ReportsPrint::thisInstance = 0;

ReportsPrint::ReportsPrint() : QPrinter(QPrinter::HighResolution)
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

ReportsPrint::~ReportsPrint()
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

ReportsPrint* ReportsPrint::instance()
{
    if(thisInstance == 0)
        thisInstance = new ReportsPrint;
    return thisInstance;
}

void ReportsPrint::destroy()
{
    if(thisInstance != 0)
        delete thisInstance;
}

bool ReportsPrint::printReport(const QString &header, const QString &data,
		const QString docName)
{
    if(setup(0))
    {
	setDocName(docName);

        QPainter painter(this);

        QPaintDeviceMetrics metrics(this);
        int headerGap = metrics.logicalDpiY() / 2;  // half inch
	int vertMargin = metrics.logicalDpiY() / 2;
	int horizMargin = metrics.logicalDpiX() / 2;

        QFont headerFont("Verdana", 8);
        QFont dataFont("Verdana", 8);
    
        QSimpleRichText headerText(header, headerFont);
    
        headerText.setWidth(&painter, painter.window().width());

        int pageHeight = (painter.window().height()
		          - headerText.height()
			  - (headerGap * 2)  // * 2: just to make the space to the footer greater
			  - (vertMargin * 2));

        QSimpleRichText dataText(data, dataFont, "", 0, 0, pageHeight);
        dataText.setWidth(&painter, painter.window().width() - (horizMargin * 2));

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
            
		if(j != 0)
		{
		    painter.save();
		    painter.translate(0, vertMargin);
                    headerText.draw(&painter, 0, 0, headerRect, QColorGroup());
		    painter.restore();
		}

                painter.save();

		if(j == 0)
		{
		    painter.translate(horizMargin,
				    -dataRect.top() + vertMargin);
		}
		else
		{
		    painter.translate(horizMargin,
				    -dataRect.top() + headerGap + vertMargin);
		}
                dataText.draw(&painter, 0, 0, dataRect, QColorGroup());
                painter.restore();

                painter.setFont(dataFont);
		painter.save();
		painter.translate(0, 0 - vertMargin);
                painter.drawText(painter.window(), Qt::AlignHCenter | Qt::AlignBottom,
                                 QString("Page " + QString::number(j + 1) + " of " +
					 QString::number(numPages)));
		painter.restore();

            }
        }
	return true;
    }
    return false;
}


