#include <qhbox.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qcursor.h>

#include "config.h"
#include "goBackLabel.h"

GoBackLabel::GoBackLabel(QWidget *parent, const char *name)
    : QHBox(parent,name)
{
    icon = new QLabel(this);
    icon->setPixmap( QPixmap::fromMimeSource(ICON_PATH + "/back.png") );
    label = new QLabel(" Back (ESC)", this);
    setCursor(Qt::PointingHandCursor);
}

void GoBackLabel::mousePressEvent(QMouseEvent*)
{
    emit goBack();
}
