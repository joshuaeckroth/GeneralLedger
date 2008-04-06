
#include <qlineedit.h>
#include <qevent.h>

#include "dateEditor.h"

DateEditor::DateEditor(QWidget *parent, const char *name)
    : QLineEdit(parent,name)
{
    setInputMask("99/99/9999");
}

void DateEditor::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Key_Space:
            break;
        case Key_Return:
            emit advance();
            break;
        case Key_Enter:
            emit advance();
            break;
        default:
            QLineEdit::keyPressEvent(event);
    }
}

