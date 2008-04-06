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

#ifndef GLOBAL_CLOSE_DIALOG_H
#define GLOBAL_CLOSE_DIALOG_H

#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QPushButton;

class GlobalCloseDialog : public QDialog
{
    Q_OBJECT
    public:
        GlobalCloseDialog(QWidget *parent, const char *name);

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
