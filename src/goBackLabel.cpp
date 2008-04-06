#include <qhbox.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qcursor.h>

#include "goBackLabel.h"

GoBackLabel::GoBackLabel(QWidget *parent, const char *name)
    : QHBox(parent,name)
{
    icon = new QLabel(this);
    icon->setPixmap( QPixmap::fromMimeSource("icons/back.png") );
    label = new QLabel(tr("<nobr><font color=\"blue\"><u>Go Back</u></font> (ESC)</nobr>"), this);
    setCursor(Qt::PointingHandCursor);
}

void GoBackLabel::mousePressEvent(QMouseEvent*)
{
    emit goBack();
}
