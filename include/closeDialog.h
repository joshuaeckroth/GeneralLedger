#ifndef CLOSE_DIALOG_H
#define CLOSE_DIALOG_H

#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QPushButton;

class CloseDialog : public QDialog
{
    Q_OBJECT
    public:
        CloseDialog();

    private slots:
        void saveAndClear();
        void dontSave();
        void retToEntry();

    private:
        QVBoxLayout *vBoxLayout;
        QLabel *label;
        QHBoxLayout *hBoxLayout;
        QPushButton *saveButton;
        QPushButton *dontSaveButton;
        QPushButton *returnButton;

};

#endif
