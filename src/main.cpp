
#include "generalLedger.h"

int main(int argc, char *argv[])
{
    GeneralLedger *app = new GeneralLedger(argc,argv);
    return app->exec();
}
