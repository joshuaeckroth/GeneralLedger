
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

HEADERS += config.h \
           include/accountEditItem.h \
           include/accountEditList.h \
           include/accountEditType.h \
           include/accountList.h \
           include/accountStack.h \
           include/accountTable.h \
           include/balanceSheetEditor.h \
           include/closeDialog.h \
           include/database.h \
           include/dateEdit.h \
           include/editorElement.h \
           include/goBackLabel.h \
           include/helpStack.h \
           include/incomeRetailEditor.h \
           include/incomeSvcEditor.h \
           include/journalDescEdit.h \
           include/journalStack.h \
           include/journalSummary.h \
           include/journalTable.h \
           include/mainStack.h \
           include/moneyEditItem.h \
           include/moneyValidator.h \
           include/pdfOptions.h \
           include/printer.h \
           include/reportStack.h \
           include/settings.h \
           include/tabs.h
SOURCES += src/accountEditItem.cpp \
           src/accountEditList.cpp \
           src/accountEditType.cpp \
           src/accountList.cpp \
           src/accountStack.cpp \
           src/accountTable.cpp \
           src/balanceSheetEditor.cpp \
           src/closeDialog.cpp \
           src/database.cpp \
           src/dateEdit.cpp \
           src/editorElement.cpp \
           src/goBackLabel.cpp \
           src/helpStack.cpp \
           src/incomeRetailEditor.cpp \
           src/incomeSvcEditor.cpp \
           src/journalDescEdit.cpp \
           src/journalStack.cpp \
           src/journalSummary.cpp \
           src/journalTable.cpp \
           src/main.cpp \
           src/mainStack.cpp \
           src/moneyEditItem.cpp \
           src/moneyValidator.cpp \
           src/pdfOptions.cpp \
           src/printer.cpp \
           src/reportStack.cpp \
           src/settings.cpp \
           src/tabs.cpp

unix:tags.target = tags
unix:tags.commands = ctags --extra=+q $$HEADERS $$SOURCES
unix:tags.depends = $$HEADERS $$SOURCES
