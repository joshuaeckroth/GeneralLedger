#ifndef PRINTER_H
#define PRINTER_H

#include <qprinter.h>

class QString;

class Settings;

class Printer : public QPrinter
{
    public:
        static Printer* instance();
        void destroy();
        void printReport(const QString &header, const QString &data);

    private:
        Printer();
        ~Printer();

        static Printer *printer;

        Settings *settings;

};

#endif
