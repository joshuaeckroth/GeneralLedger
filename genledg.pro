
TEMPLATE = app
DEPENDPATH += . include src
INCLUDEPATH += . include
DESTDIR = bin
OBJECTS_DIR = build
RC_FILE = genledg.rc
CONFIG += debug warn_on
win32:CONFIG += exceptions

unix:QMAKE_MAKEFILE = Makefile.unix
win32:QMAKE_MAKEFILE = Makefile.win32

QMAKE_EXTRA_UNIX_TARGETS += tags
unix:PRE_TARGETDEPS += tags

HEADERS += \
           config.h \
           include/accountsData.h \
           include/accountsEditId.h \
           include/accountsTab.h \
           include/accountsTable.h \
           include/client.h \
           include/globalCloseDialog.h \
           include/globalGoBackLabel.h \
           include/globalTabs.h \
           include/helpTab.h \
           include/homeTab.h \
           include/importExport.h \
           include/journalData.h \
           include/journalEditAccount.h \
           include/journalEditAccountList.h \
           include/journalEditCurrency.h \
           include/journalEditCurrencyValidator.h \
           include/journalEditDate.h \
           include/journalEditDesc.h \
           include/journalSummary.h \
           include/journalTab.h \
           include/journalTable.h \
           include/main.h \
           include/reportsEditor.h \
           include/reportsEditorDialog.h \
           include/reportsEditorEntityWidget.h \
           include/reportsEditorSection.h \
           include/reportsGenerator.h \
           include/reportsPDFDialog.h \
           include/reportsPrint.h \
           include/reportsTab.h \
	   include/saveAndClearDialog.h \
           include/settings.h \

SOURCES += \
           src/accountsData.cpp \
           src/accountsEditId.cpp \
           src/accountsTab.cpp \
           src/accountsTable.cpp \
           src/client.cpp \
           src/globalCloseDialog.cpp \
           src/globalGoBackLabel.cpp \
           src/globalTabs.cpp \
           src/helpTab.cpp \
           src/homeTab.cpp \
           src/importExport.cpp \
           src/journalData.cpp \
           src/journalEditAccount.cpp \
           src/journalEditAccountList.cpp \
           src/journalEditCurrency.cpp \
           src/journalEditCurrencyValidator.cpp \
           src/journalEditDate.cpp \
           src/journalEditDesc.cpp \
           src/journalSummary.cpp \
           src/journalTab.cpp \
           src/journalTable.cpp \
           src/main.cpp \
           src/reportsEditor.cpp \
           src/reportsEditorDialog.cpp \
           src/reportsEditorEntityWidget.cpp \
           src/reportsEditorSection.cpp \
           src/reportsGenerator.cpp \
           src/reportsPDFDialog.cpp \
           src/reportsPrint.cpp \
           src/reportsTab.cpp \
	   src/saveAndClearDialog.cpp \
           src/settings.cpp \

unix:tags.target = tags
unix:tags.commands = ctags $$HEADERS $$SOURCES
unix:tags.depends = $$HEADERS $$SOURCES
