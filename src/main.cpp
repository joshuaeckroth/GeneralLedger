#include <qapplication.h>

#include "tabs.h"

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    Tabs tabs(&app);
    tabs.show();
    app.setMainWidget(&tabs);
    return app.exec();
}
