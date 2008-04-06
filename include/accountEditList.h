#ifndef ACCOUNT_EDIT_LIST_H
#define ACCOUNT_EDIT_LIST_H

#include <qcombobox.h>

class QStringList;
class QFocusEvent;
class QListBox;
class QEvent;

class AccountEditList : public QComboBox
{
    Q_OBJECT
    public:
        AccountEditList(QWidget *parent, const char *name, const QStringList &newAccounts, const QString &text);
        QString currentText() const;
        void updateAccounts(const QStringList &newAccounts);
        void focusInEvent(QFocusEvent *event);
        void focusOutEvent(QFocusEvent *event);
        
    private slots:
        void setText(const QString &newText);
        void changed(const QString &);
        
    private:
        QStringList accounts;
        QString curText;
        QListBox *list;
};

#endif

