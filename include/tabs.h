#ifndef TABS_H
#define TABS_H

#include <qtabwidget.h>
#include <qsqldatabase.h>

class Tabs : public QTabWidget
{
    Q_OBJECT
    public:
        Tabs( QWidget* parent = 0, const char* name = 0, WFlags f = 0 );
        QString getClient() const;
        
    signals:
        void appQuit();
    
    private slots:
        void openDefault();
        void openNew(QString name);
        void createNew(QString name);
        void quit();
        
    private:
        QSqlDatabase *db;
};

#endif