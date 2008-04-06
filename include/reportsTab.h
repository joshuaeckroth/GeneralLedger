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

#ifndef REPORTS_TAB_H
#define REPORTS_TAB_H

#include <qwidgetstack.h>

#include "reportsEditor.h"

class QEvent;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QVGroupBox;
class QVBox;
class QFrame;
class QComboBox;
class QCheckBox;
class QPushButton;
class QLabel;
class QLineEdit;
class QRadioButton;
class QBoxLayout;
class QTextEdit;
class QStringList;

class AccountsData;
class Client;
class Settings;
class GlobalGoBackLabel;
class ReportsPrint;
class ReportsGenerator;
class ReportsPDFDialog;

/// Reports Tab
class ReportsTab : public QWidgetStack
{
    Q_OBJECT
    public:
        /// Constructor
        ReportsTab(QWidget *parent, const char *name);
        ~ReportsTab();
        
    signals:
        /// Go to Home tab
        void switchToHome();
        
    private slots:
        /// Internal slot for the "all" or "range" radio button
        void detailAccountsAllChanged(bool);

        /// Internal slot for the "all" or "range" radio button
        void trialAccountsAllChanged(bool);

        void clientOpened();

        /// Slot for changes in AccountsData
        void updateAccounts();

        /// Export report as PDF
        void exportPDF();

        /// Export report as CSV
        void exportCSV();

        /// Switch the widget stack widget
        void switchWidget();

        /// View a general detail report
        void generalDetailReport();

        /// View a general trial balance report
        void generalTrialReport();

        /// View a chart of accounts report
        void chartAccountsReport();

        /// View a balance sheet report
        void balanceReport();

        /// View an income service statement report
        void incomeSvcReport();

        /// View an income retail statement report
        void incomeRetailReport();

        /// Edit the balance sheet report
        void switchToEditBalance();

        /// Edit the income service statement report
        void switchToEditIncomeSvc();

        /// Edit the income retail statement report
        void switchToEditIncomeRetail();

        /// Print the report
        void print();
        
    private:
        /// Catch events
        bool eventFilter(QObject *target, QEvent *event);

        /// Create and switch to a report editor
        void switchToEditor(ReportsEditor::Editor editorType);

        void viewReport();

        /// Pointer to Settings instance
        Settings *settings;
       
        /// Pointer to AccountsData instance
        AccountsData *accountsData;

        /// Pointer to Client instance
        Client *client;

        /// Pointer to report editors
        ReportsEditor *reportsEditor;

        /// Pointer to ReportsGenerator instance
        ReportsGenerator *reportsGenerator;

        /// Pointer to ReportsPrint instance
        ReportsPrint *reportsPrint;

        /// Pointer to the pdf save options dialog
        ReportsPDFDialog *reportsPDFDialog;

        /// Path for tmp.html file for PDF conversion
        QString clientPath;

        /// Default path for exported reports
        QString exportPath;

        /// The active widget in the stack
        int active;

        /// Current list of accounts
        QStringList accounts;
       
        /// A bunch of widgets for the report chooser 
        struct {
            QWidget *widget;
            QVBoxLayout *vBoxLabel;
            QHBoxLayout *hBoxLabel;
            GlobalGoBackLabel *topLabel;
            QHBoxLayout *hBoxLayout;
            QVBoxLayout *vBoxLayout;
            
            QVGroupBox *reportsGroup;
            QWidget *reportsWidget;
            QGridLayout *reportsGridLayout;
            
            QVGroupBox *generalDetailVGroupBox;
            QWidget *generalDetailDateRangeWidget;
            QHBoxLayout *generalDetailDateRangeHBoxLayout;
            QLabel *generalDetailDateRangeLabelBegin;
            QComboBox *generalDetailDateRangeBegin;
            QLabel *generalDetailDateRangeLabelBetween;
            QComboBox *generalDetailDateRangeEnd;
            QWidget *generalDetailAccountRangeWidget;
            QHBoxLayout *generalDetailAccountRangeHBoxLayout;
            QLabel *generalDetailAccountRangeLabel;
            QComboBox *generalDetailAccountRangeBegin;
            QLabel *generalDetailAccountRangeLabelBetween;
            QComboBox *generalDetailAccountRangeEnd;
            QCheckBox *generalDetailAccountRangeAll;
            QVBoxLayout *generalDetailButtonVBoxLayout;
            QPushButton *generalDetailButton;
            
            QVGroupBox *generalTrialVGroupBox;
            QWidget *generalTrialMonthEndWidget;
            QHBoxLayout *generalTrialMonthEndHBoxLayout;
            QLabel *generalTrialMonthEndLabel;
            QComboBox *generalTrialMonthEndList;
            QWidget *generalTrialAccountRangeWidget;
            QHBoxLayout *generalTrialAccountRangeHBoxLayout;
            QLabel *generalTrialAccountRangeLabel;
            QComboBox *generalTrialAccountRangeBegin;
            QLabel *generalTrialAccountRangeLabelBetween;
            QComboBox *generalTrialAccountRangeEnd;
            QCheckBox *generalTrialAccountRangeAll;
            QVBoxLayout *generalTrialButtonVBoxLayout;
            QPushButton *generalTrialButton;
            
            QVGroupBox *editableVGroupBox;
            QWidget *editableMonthEndWidget;
            QHBoxLayout *editableMonthEndHBoxLayout;
            QLabel *editableMonthEndLabel;
            QComboBox *editableMonthEndList;
            QWidget *editableViewWidget;
            QVBoxLayout *editableViewVBoxLayout;
            QWidget *editableListWidget;
            QHBoxLayout *editableListHBoxLayout;
            QVBoxLayout *editableListVBoxLayout;
            QHBoxLayout *editableBalanceHBoxLayout;
            QLabel *editableBalanceLabel;
            QPushButton *editableBalanceModify;
            QPushButton *editableBalanceView;
            QHBoxLayout *editableIncomeSvcHBoxLayout;
            QLabel *editableIncomeSvcLabel;
            QPushButton *editableIncomeSvcModify;
            QPushButton *editableIncomeSvcView;
            QHBoxLayout *editableIncomeRetailHBoxLayout;
            QLabel *editableIncomeRetailLabel;
            QPushButton *editableIncomeRetailModify;
            QPushButton *editableIncomeRetailView;
            
            QVBoxLayout *chartAccountsButtonVBoxLayout;
            QPushButton *chartAccountsButton;
        } main;
       
        /// Report widget 
        struct {
            QWidget *widget;
            QVBoxLayout *vBoxLayout;
            QBoxLayout *labelLayout;
            GlobalGoBackLabel *topLabel;
            QPushButton *print;
            QTextEdit *view;
            QWidget *bottomWidget;
            QHBoxLayout *bottomHBoxLayout;
            QLabel *bottomLabel;
            QFrame *bottomRightFrame;
            QBoxLayout *bottomRightBoxLayout;
            QPushButton *exportPDFButton;
            QPushButton *exportCSVButton;
        } report;

        /// Report Editor widget
        struct {
            QWidget *widget;
            QVBoxLayout *vBoxLayout;
            QBoxLayout *labelLayout;
            GlobalGoBackLabel *topLabel;
            QWidget *editor;
        } editor;
        
};


#endif
