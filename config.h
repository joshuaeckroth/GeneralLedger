#ifndef Q_OS_WIN32
#define ICON_PATH QString("/usr/home/josh/projects/genledg/trunk/icons")
#define HTMLDOC_PATH QString("/usr/local/bin/htmldoc")
#define HELP_PATH QString("/usr/home/josh/projects/genledg/trunk/help")
#else
#include <qapplication.h>
#define ICON_PATH (qApp->applicationDirPath() + "/icons")
#define HTMLDOC_PATH (qApp->applicationDirPath() + "/htmldoc.exe")
#define HELP_PATH (qApp->applicationDirPath() + "/help")
#endif

#define VERSION "1.0"
#define RELEASE_DATE "(DATE)"
