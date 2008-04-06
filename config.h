#ifndef Q_OS_WIN32
#define ICON_PATH QString("/home/josh/projects/genledg/trunk/icons")
#define CRYPT_PATH QString("/home/josh/projects/genledg/trunk/unix")
#define HTMLDOC_PATH QString("/usr/bin")
#define HELP_PATH QString("/home/josh/projects/genledg/trunk/help")
#else
#include <qapplication.h>
#define ICON_PATH (qApp->applicationDirPath() + "/icons")
#define CRYPT_PATH (qApp->applicationDirPath() + "/aescrypt2")
#define HTMLDOC_PATH (qApp->applicationDirPath() + "/htmldoc")
#define HELP_PATH (qApp->applicationDirPath() + "/help")
#endif

#define VERSION "1.0"
#define RELEASE_DATE "(DATE)"
