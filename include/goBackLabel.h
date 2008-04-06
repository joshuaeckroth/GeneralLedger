#ifndef GO_BACK_LABEL_H
#define GO_BACK_LABEL_H

#include <qhbox.h>

class QLabel;
class QEvent;

class GoBackLabel : public QHBox
{
    Q_OBJECT
    public:
        GoBackLabel(QWidget *parent = 0, const char *name = 0);
    signals:
        void goBack();
    protected:
        void mousePressEvent(QMouseEvent*);
    private:
        QLabel *label;
        QLabel *icon;
};

#endif
