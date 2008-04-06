#include <qlabel.h>

#include "goBackLabel.h"

GoBackLabel::GoBackLabel(QWidget *parent, const char *name)
    : QLabel(tr("<font color=\"blue\"><u>Go Back</u></font>"), parent, name)
{}

void GoBackLabel::mousePressEvent(QMouseEvent *event)
{
    emit goBack();
}
  