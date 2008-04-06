#ifndef PDF_OPTIONS_H
#define PDF_OPTIONS_H

#include <qobject.h>

class QString;
class QDialog;
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QComboBox;
class QLineEdit;
class QRadioButton;
class QGridLayout;
class QCheckBox;

class Settings;

class PDFOptions : public QObject
{
    Q_OBJECT
    public:
        PDFOptions();
        QString exec();

    private slots:
        void paperSizeChanged(int);
        void passwordNoClicked();
        void passwordYesClicked();

    private:
        Settings *settings;

        struct {
            QDialog *dialog;
            QVBoxLayout *vBoxLayout;
            QLabel *topLabel;
            QLabel *defaultLabel;
            QHBoxLayout *defaultHBoxLayout;
            QVBoxLayout *defaultVBoxLayout;
            QLabel *defaultPaperSizeLabel;
            QLabel *defaultPasswordLabel;
            QLabel *defaultPermissionsLabel;
            QHBoxLayout *buttonHBoxLayout;
            QPushButton *changeOptionsButton;
            QPushButton *useDefaultsButton;
        } question;

        struct {
            QDialog *dialog;
            QVBoxLayout *vBoxLayout;

            QLabel *paperSizeLabel;
            QHBoxLayout *paperSizeHBoxLayout;
            QVBoxLayout *paperSizeVBoxLayout;
            QHBoxLayout *paperSizeComboHBoxLayout;
            QComboBox *paperSizeComboBox;
            QHBoxLayout *paperSizeCustomHBoxLayout;
            QLineEdit *paperSizeCustomWidthLineEdit;
            QLabel *paperSizeCustomBetweenLabel;
            QLineEdit *paperSizeCustomHeightLineEdit;
            QComboBox *paperSizeCustomUnitsComboBox;

            QLabel *passwordLabel;
            QHBoxLayout *passwordHBoxLayout;
            QVBoxLayout *passwordVBoxLayout;
            QRadioButton *passwordNoRadioButton;
            QRadioButton *passwordYesRadioButton;
            QHBoxLayout *passwordYesHBoxLayout;
            QLabel *passwordYesLabel;

            QLabel *permissionsLabel;
            QHBoxLayout *permissionsHBoxLayout;
            QGridLayout *permissionsGridLayout;
            QCheckBox *permissionsNoPrint;
            QCheckBox *permissionsNoModify;
            QCheckBox *permissionsNoCopy;
            QCheckBox *permissionsNoAnnotate;

            QHBoxLayout *buttonHBoxLayout;
            QPushButton *buttonCreate;
            QPushButton *buttonCancel;
        } options;

        struct {
            QDialog *dialog;
            QVBoxLayout *vBoxLayout;
            QLabel *label;
            QLabel *label2;
            QGridLayout *gridLayout;
            QLabel *firstLabel;
            QLineEdit *firstLineEdit;
            QLabel *secondLabel;
            QLineEdit *secondLineEdit;
            QHBoxLayout *hBoxLayout;
            QPushButton *createButton;
            QPushButton *cancelButton;
        } password;

};

#endif

