#ifndef MAIN_STACK_H
#define MAIN_STACK_H

#include <qwidgetstack.h>

class MainStack : public QWidgetStack
{
    Q_OBJECT
    public:
        MainStack( QWidget *parent = 0, const char *name = 0 );
        
    signals:
        void openDefault();
        void openNew(QString);
        void createNew(QString);
        void quit();
        
     private slots:
         void openNewDialog();
         void createNewDialog();
};


#endif
