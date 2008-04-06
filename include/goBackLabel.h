#ifndef GO_BACK_LABEL_H
#define GO_BACK_LABEL_H

#include <qlabel.h>

class GoBackLabel : public QLabel
{
    Q_OBJECT
    public:
        GoBackLabel(QWidget *parent = 0, const char *name = 0);
    signals:
        void goBack();
    protected:
        void mousePressEvent(QMouseEvent *event);
};

#endif
