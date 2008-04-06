#include <qhbox.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qcursor.h>

#include "goBackLabel.h"
#include "settings.h"

GoBackLabel::GoBackLabel(QWidget *parent, const char *name)
    : QHBox(parent,name)
{
    Settings *settings = Settings::instance();

    icon = new QLabel(this);
    icon->setPixmap( QPixmap::fromMimeSource(settings->getIconPath() + "/back.png") );
    label = new QLabel(" Back (ESC)", this);
    setCursor(Qt::PointingHandCursor);
}

void GoBackLabel::mousePressEvent(QMouseEvent*)
{
    emit goBack();
}
