#ifndef INCOME_RETAIL_EDITOR_H
#define INCOME_RETAIL_EDITOR_H

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

class EditorElement;
class AccountList;
class Database;

class IncomeRetailEditor : public QScrollView
{
    Q_OBJECT
    public:
        IncomeRetailEditor(QWidget *parent = 0, const char *name = 0);
        ~IncomeRetailEditor();
        
    private slots:
        void updateSection(void*);
        void addElement(int category);
        void addIncome();
        void addCOS();
        void addExpenses();
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
            std::vector<EditorElement*> elements;
        } income, cos, expenses;
        
        void listSection(ElementSection *section);
        void deleteSection(ElementSection *section);
        
        struct {
            QDialog *main;
            QVBoxLayout *vBoxLayout;
            QRadioButton *oneAccount;
            QFrame *oneAccountFrame;
            QHBoxLayout *oneAccountHBoxLayout;
            QLabel *oneAccountLabel;
            AccountList *oneAccountEdit;
            QRadioButton *rangeAccount;
            QFrame *rangeAccountFrame;
            QHBoxLayout *rangeAccountHBoxLayout;
            QFrame *rangeAccountFrameList;
            QVBoxLayout *rangeAccountFrameListVBoxLayout;
            QFrame *rangeBeginEndFrame;
            QHBoxLayout *rangeBeginEndHBoxLayout;
            QLabel *rangeBegin;
            AccountList *rangeBeginEdit;
            QLabel *rangeBetween;
            AccountList *rangeEndEdit;
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
