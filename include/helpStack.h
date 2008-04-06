#ifndef HELP_STACK_H
#define HELP_STACK_H

#include <qwidget.h>

class QVBoxLayout;
class QBoxLayout;
class QPushButton;
class QTextBrowser;
class QLabel;

class GoBackLabel;

class HelpStack : public QWidget
{
    Q_OBJECT
    public:
        HelpStack(QWidget *parent = 0, const char *name = 0);
        ~HelpStack();
        
    signals:
        void goBack();
        
    private slots:
        void helpContents();
        
    private:
        
        struct {
            QVBoxLayout *vBoxLayout;
            QBoxLayout *labelLayout;
            GoBackLabel *topLabel;
            QPushButton *helpContentsButton;
            QTextBrowser *content;
            QLabel *bottomLabel;
        } main;

};

#endif

