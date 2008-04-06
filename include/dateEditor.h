#ifndef DATE_EDITOR_H
#define DATE_EDITOR_H

#include <qlineedit.h>

class QKeyEvent;

class DateEditor : public QLineEdit
{
    Q_OBJECT
    public:
        DateEditor(QWidget *parent = 0, const char *name = 0);
        
    signals:
        void advance();
        
    private:
        void keyPressEvent(QKeyEvent *event);
};

#endif
