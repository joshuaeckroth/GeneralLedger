#ifndef ACCOUNT_EDIT_LIST_H
#define ACCOUNT_EDIT_LIST_H

#include <qcombobox.h>

class QStringList;
class QListBox;
class QFocusEvent;

class AccountEditList : public QComboBox
{
    Q_OBJECT
    public:
        AccountEditList(QWidget *parent, const char *name, const QStringList &newAccounts, const QString &text);
        void setText(const QString &newText);
        QString currentText() const;
        void updateAccounts(const QStringList &newAccounts);
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

