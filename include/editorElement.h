#ifndef EDITOR_ELEMENT_H
#define EDITOR_ELEMENT_H

#include <qobject.h>

#include "database.h"

class QFrame;
class QString;
class QHBoxLayout;
class QLabel;
class QPushButton;

class EditorElement : public QObject
{
    Q_OBJECT
    public:
        EditorElement(Database::editorCategories newCategory, QString newId, QString newType,
                    QString newDesc, QString newAccountBegin, QString newAccountEnd);
     
        ~EditorElement();
     
        QFrame* getFrame(QWidget *parent, void *section);
     
    signals:
        void changed(void*);
        
    private slots:
        void elementUp();
        void elementDown();
        void removeElement();
     
    private:
        Database::editorCategories category;        
        void *section;
        
        Database *db;
        
        QString id;
        QString type;
        QString desc;
        QString accountBegin;
        QString accountEnd;
     
        QFrame *frame;
        QHBoxLayout *layout;
        QLabel *descLabel;
        QLabel *accountsLabel;
        QPushButton *upButton;
        QPushButton *downButton;
        QPushButton *removeButton;
};

#endif
