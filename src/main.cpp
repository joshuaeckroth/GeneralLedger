#include <qapplication.h>

#include "tabs.h"
#include "database.h"
#include "printer.h"
#include "settings.h"

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    
    Settings *settings = Settings::instance();
    Database *db = Database::instance();
    Printer *printer = Printer::instance();
    
    Tabs tabs(&app);
    tabs.show();
    app.setMainWidget(&tabs);
    int retVal = app.exec();
    db->destroy();
    printer->destroy();
    settings->destroy();
    return retVal;
}
