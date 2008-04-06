/* General Ledger, Copyright (C) 2004  Joshua Eckroth <josh@eckroth.net>
 * http://www.eckroth.net/software/genledg
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  US
*/

#ifndef REPORTS_PDF_DIALOG_H
#define REPORTS_PDF_DIALOG_H

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

class ReportsPDFDialog : public QObject
{
    Q_OBJECT
    public:
        ReportsPDFDialog();
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

