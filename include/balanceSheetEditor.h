#ifndef BALANCE_SHEET_EDITOR_H
#define BALANCE_SHEET_EDITOR_H

#include <vector>

#include <qscrollview.h>

class QWidget;
class QFrame;
class QVBoxLayout;
class QLabel;
class QHBoxLayout;
class QPushButton;
class QDialog;
class QRadioButton;
class QLineEdit;
class QStringList;

class BalanceElement;
class AccountEditList;
class Database;

class BalanceSheetEditor : public QScrollView
{
    Q_OBJECT
    public:
        BalanceSheetEditor(QWidget *parent = 0, const char *name = 0);
        ~BalanceSheetEditor();
        
    private slots:
        void updateSection(void*);
        void addElement(int category);
        void addAssets();
        void addLiabilities();
        void addEquities();
        void oneAccountToggled();
        void rangeAccountToggled();
        void updateAccounts();
       
    private:
        Database *db;
        
        QStringList accounts;
        
        QFrame *frame;
        QVBoxLayout *vBoxLayout;
   
        struct ElementSection {
            QFrame *topFrame;
            QHBoxLayout *topHBoxLayout;
            QLabel *label;
            QPushButton *addButton;
            QFrame *bottomFrame;
            QHBoxLayout *bottomHBoxLayout;
            QWidget *listView;
            QVBoxLayout *vBoxLayout;
            std::vector<BalanceElement*> elements;
        } assets, liabilities, equities;
        
        void listSection(ElementSection *section);
        void deleteSection(ElementSection *section);
        
        struct {
            QDialog *main;
            QVBoxLayout *vBoxLayout;
            QRadioButton *oneAccount;
            QFrame *oneAccountFrame;
            QHBoxLayout *oneAccountHBoxLayout;
            QLabel *oneAccountLabel;
            AccountEditList *oneAccountEdit;
            QRadioButton *rangeAccount;
            QFrame *rangeAccountFrame;
            QHBoxLayout *rangeAccountHBoxLayout;
            QFrame *rangeAccountFrameList;
            QVBoxLayout *rangeAccountFrameListVBoxLayout;
            QFrame *rangeBeginEndFrame;
            QHBoxLayout *rangeBeginEndHBoxLayout;
            QLabel *rangeBegin;
            AccountEditList *rangeBeginEdit;
            QLabel *rangeBetween;
            AccountEditList *rangeEndEdit;
            QFrame *rangeDescFrame;
            QHBoxLayout *rangeDescFrameHBoxLayout;
            QLabel *rangeDesc;
            QLineEdit *rangeDescEdit;
            QFrame *buttonFrame;
            QHBoxLayout *buttonFrameHBoxLayout;
            QPushButton *ok;
            QPushButton *cancel;
        } dialog;

};

#endif
