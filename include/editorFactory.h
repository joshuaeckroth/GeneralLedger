#ifndef EDITOR_FACTORY_H
#define EDITOR_FACTORY_H

#include <qsqleditorfactory.h>

class QWidget;
class QSqlField;

class EditorFactory : public QSqlEditorFactory
{
    Q_OBJECT
    public:
        EditorFactory(QObject *parent = 0, const char *name = 0);
        QWidget* createEditor(QWidget *parent, const QSqlField *field); 
        
};

#endif

