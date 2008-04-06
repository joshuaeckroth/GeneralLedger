#ifndef GLOBAL_GO_BACK_LABEL_H
#define GLOBAL_GO_BACK_LABEL_H

#include <qhbox.h>

class QLabel;
class QEvent;

class GlobalGoBackLabel : public QHBox
{
    Q_OBJECT
    public:
        GlobalGoBackLabel(QWidget *parent, const char *name);
    signals:
        void goBack();
    protected:
        void mousePressEvent(QMouseEvent*);
    private:
        QLabel *label;
        QLabel *icon;
};

#endif
