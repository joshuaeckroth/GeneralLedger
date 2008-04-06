#ifndef ACCOUNT_EDIT_H
#define ACCOUNT_EDIT_H

#include <qcombobox.h>

class QStringList;
class QListBox;
class QFocusEvent;

class AccountEdit : public QComboBox
{
    Q_OBJECT
    public:
        AccountEdit(QWidget *parent, const char *name, QStringList &newAccounts);
        QString currentText() const;
        void focusInEvent(QFocusEvent *event);
        void focusOutEvent(QFocusEvent *event);
        
    private slots:
        void changed(const QString &);
        
    private:
        QStringList accounts;
        QString text;
        QListBox *listbox;
};

#endif

