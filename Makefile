#############################################################################
# Makefile for building: genledg
# Generated by qmake (1.07a) (Qt 3.3.2) on: Tue Aug  3 21:59:52 2004
# Project:  genledg.pro
# Template: app
# Command: $(QMAKE) -o Makefile genledg.pro
#############################################################################

####### Compiler, tools and options

CC       = gcc
CXX      = g++
LEX      = flex
YACC     = yacc
CFLAGS   = -pipe -Wall -W -O2  -g -DQT_THREAD_SUPPORT
CXXFLAGS = -pipe -Wall -W -O2  -g -DQT_THREAD_SUPPORT
LEXFLAGS = 
YACCFLAGS= -d
INCPATH  = -I/usr/qt/mkspecs/default -I. -I. -Iinclude -I$(QTDIR)/include
LINK     = g++
LFLAGS   =  -Wl,-rpath,$(QTDIR)/lib
LIBS     = $(SUBLIBS) -L$(QTDIR)/lib -L/usr/X11R6/lib -lqt-mt -L/usr/X11R6/lib -L/usr/lib -lXxf86vm -lXt -lmng -ljpeg -lpng -lz -lGL -lXmu -lXrender -lXrandr -lXcursor -lXinerama -lXft -lfreetype -lfontconfig -lSM -lICE -ldl -lpthread -lXext -lX11 -lm
AR       = ar cqs
RANLIB   = 
MOC      = $(QTDIR)/bin/moc
UIC      = $(QTDIR)/bin/uic
QMAKE    = qmake
TAR      = tar -cf
GZIP     = gzip -9f
COPY     = cp -f
COPY_FILE= $(COPY)
COPY_DIR = $(COPY) -r
INSTALL_FILE= $(COPY_FILE)
INSTALL_DIR = $(COPY_DIR)
DEL_FILE = rm -f
SYMLINK  = ln -sf
DEL_DIR  = rmdir
MOVE     = mv -f
CHK_DIR_EXISTS= test -d
MKDIR    = mkdir -p

####### Output directory

OBJECTS_DIR = ./

####### Files

HEADERS = include/accountEditItem.h \
		include/accountEditList.h \
		include/accountEditSimple.h \
		include/accountEditType.h \
		include/accountStack.h \
		include/accountTable.h \
		include/balanceElement.h \
		include/balanceSheetEditor.h \
		include/basicEdit.h \
		include/database.h \
		include/dateEdit.h \
		include/goBackLabel.h \
		include/helpStack.h \
		include/journalStack.h \
		include/journalSummary.h \
		include/journalTable.h \
		include/mainStack.h \
		include/moneyEdit.h \
		include/reportStack.h \
		include/tabs.h
SOURCES = src/accountEditItem.cpp \
		src/accountEditList.cpp \
		src/accountEditSimple.cpp \
		src/accountEditType.cpp \
		src/accountStack.cpp \
		src/accountTable.cpp \
		src/balanceElement.cpp \
		src/balanceSheetEditor.cpp \
		src/basicEdit.cpp \
		src/database.cpp \
		src/dateEdit.cpp \
		src/goBackLabel.cpp \
		src/helpStack.cpp \
		src/journalStack.cpp \
		src/journalSummary.cpp \
		src/journalTable.cpp \
		src/main.cpp \
		src/mainStack.cpp \
		src/moneyEdit.cpp \
		src/reportStack.cpp \
		src/tabs.cpp
OBJECTS = accountEditItem.o \
		accountEditList.o \
		accountEditSimple.o \
		accountEditType.o \
		accountStack.o \
		accountTable.o \
		balanceElement.o \
		balanceSheetEditor.o \
		basicEdit.o \
		database.o \
		dateEdit.o \
		goBackLabel.o \
		helpStack.o \
		journalStack.o \
		journalSummary.o \
		journalTable.o \
		main.o \
		mainStack.o \
		moneyEdit.o \
		reportStack.o \
		tabs.o
FORMS = 
UICDECLS = 
UICIMPLS = 
SRCMOC   = include/moc_accountEditList.cpp \
		include/moc_accountStack.cpp \
		include/moc_accountTable.cpp \
		include/moc_balanceElement.cpp \
		include/moc_balanceSheetEditor.cpp \
		include/moc_database.cpp \
		include/moc_goBackLabel.cpp \
		include/moc_helpStack.cpp \
		include/moc_journalStack.cpp \
		include/moc_journalSummary.cpp \
		include/moc_journalTable.cpp \
		include/moc_mainStack.cpp \
		include/moc_reportStack.cpp \
		include/moc_tabs.cpp
OBJMOC = moc_accountEditList.o \
		moc_accountStack.o \
		moc_accountTable.o \
		moc_balanceElement.o \
		moc_balanceSheetEditor.o \
		moc_database.o \
		moc_goBackLabel.o \
		moc_helpStack.o \
		moc_journalStack.o \
		moc_journalSummary.o \
		moc_journalTable.o \
		moc_mainStack.o \
		moc_reportStack.o \
		moc_tabs.o
DIST	   = genledg.pro
QMAKE_TARGET = genledg
DESTDIR  = 
TARGET   = genledg

first: all
####### Implicit rules

.SUFFIXES: .c .o .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(UICDECLS) $(OBJECTS) $(OBJMOC)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJMOC) $(OBJCOMP) $(LIBS)

mocables: $(SRCMOC)
uicables: $(UICDECLS) $(UICIMPLS)

$(MOC): 
	( cd $(QTDIR)/src/moc && $(MAKE) )

Makefile: genledg.pro  /usr/qt/mkspecs/default/qmake.conf /usr/qt/lib/libqt-mt.prl
	$(QMAKE) -o Makefile genledg.pro
qmake: 
	@$(QMAKE) -o Makefile genledg.pro

dist: 
	@mkdir -p .tmp/genledg && $(COPY_FILE) --parents $(SOURCES) $(HEADERS) $(FORMS) $(DIST) .tmp/genledg/ && ( cd `dirname .tmp/genledg` && $(TAR) genledg.tar genledg && $(GZIP) genledg.tar ) && $(MOVE) `dirname .tmp/genledg`/genledg.tar.gz . && $(DEL_FILE) -r .tmp/genledg

mocclean:
	-$(DEL_FILE) $(OBJMOC)
	-$(DEL_FILE) $(SRCMOC)

uiclean:

yaccclean:
lexclean:
clean: mocclean
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) $(TARGET)


FORCE:

####### Compile

accountEditItem.o: src/accountEditItem.cpp include/accountEditItem.h \
		include/accountEditList.h \
		include/accountEditSimple.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o accountEditItem.o src/accountEditItem.cpp

accountEditList.o: src/accountEditList.cpp include/accountEditList.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o accountEditList.o src/accountEditList.cpp

accountEditSimple.o: src/accountEditSimple.cpp include/accountEditSimple.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o accountEditSimple.o src/accountEditSimple.cpp

accountEditType.o: src/accountEditType.cpp include/accountEditType.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o accountEditType.o src/accountEditType.cpp

accountStack.o: src/accountStack.cpp include/accountStack.h \
		include/accountTable.h \
		include/goBackLabel.h \
		include/database.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o accountStack.o src/accountStack.cpp

accountTable.o: src/accountTable.cpp include/accountTable.h \
		include/database.h \
		include/accountEditItem.h \
		include/accountEditSimple.h \
		include/accountEditType.h \
		include/basicEdit.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o accountTable.o src/accountTable.cpp

balanceElement.o: src/balanceElement.cpp include/balanceElement.h \
		include/database.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o balanceElement.o src/balanceElement.cpp

balanceSheetEditor.o: src/balanceSheetEditor.cpp include/balanceSheetEditor.h \
		include/balanceElement.h \
		include/database.h \
		include/accountEditList.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o balanceSheetEditor.o src/balanceSheetEditor.cpp

basicEdit.o: src/basicEdit.cpp include/basicEdit.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o basicEdit.o src/basicEdit.cpp

database.o: src/database.cpp include/database.h \
		include/balanceElement.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o database.o src/database.cpp

dateEdit.o: src/dateEdit.cpp include/dateEdit.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o dateEdit.o src/dateEdit.cpp

goBackLabel.o: src/goBackLabel.cpp include/goBackLabel.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o goBackLabel.o src/goBackLabel.cpp

helpStack.o: src/helpStack.cpp include/helpStack.h \
		include/goBackLabel.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o helpStack.o src/helpStack.cpp

journalStack.o: src/journalStack.cpp include/journalStack.h \
		include/journalTable.h \
		include/journalSummary.h \
		include/goBackLabel.h \
		include/database.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o journalStack.o src/journalStack.cpp

journalSummary.o: src/journalSummary.cpp include/journalSummary.h \
		include/journalTable.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o journalSummary.o src/journalSummary.cpp

journalTable.o: src/journalTable.cpp include/journalTable.h \
		include/accountEditItem.h \
		include/basicEdit.h \
		include/dateEdit.h \
		include/moneyEdit.h \
		include/database.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o journalTable.o src/journalTable.cpp

main.o: src/main.cpp include/tabs.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o src/main.cpp

mainStack.o: src/mainStack.cpp include/mainStack.h \
		include/database.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o mainStack.o src/mainStack.cpp

moneyEdit.o: src/moneyEdit.cpp include/moneyEdit.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moneyEdit.o src/moneyEdit.cpp

reportStack.o: src/reportStack.cpp include/reportStack.h \
		include/goBackLabel.h \
		include/balanceSheetEditor.h \
		include/database.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o reportStack.o src/reportStack.cpp

tabs.o: src/tabs.cpp include/tabs.h \
		include/database.h \
		include/mainStack.h \
		include/accountStack.h \
		include/journalStack.h \
		include/reportStack.h \
		include/helpStack.h \
		include/goBackLabel.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o tabs.o src/tabs.cpp

moc_accountEditList.o: include/moc_accountEditList.cpp  include/accountEditList.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_accountEditList.o include/moc_accountEditList.cpp

moc_accountStack.o: include/moc_accountStack.cpp  include/accountStack.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_accountStack.o include/moc_accountStack.cpp

moc_accountTable.o: include/moc_accountTable.cpp  include/accountTable.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_accountTable.o include/moc_accountTable.cpp

moc_balanceElement.o: include/moc_balanceElement.cpp  include/balanceElement.h include/database.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_balanceElement.o include/moc_balanceElement.cpp

moc_balanceSheetEditor.o: include/moc_balanceSheetEditor.cpp  include/balanceSheetEditor.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_balanceSheetEditor.o include/moc_balanceSheetEditor.cpp

moc_database.o: include/moc_database.cpp  include/database.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_database.o include/moc_database.cpp

moc_goBackLabel.o: include/moc_goBackLabel.cpp  include/goBackLabel.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_goBackLabel.o include/moc_goBackLabel.cpp

moc_helpStack.o: include/moc_helpStack.cpp  include/helpStack.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_helpStack.o include/moc_helpStack.cpp

moc_journalStack.o: include/moc_journalStack.cpp  include/journalStack.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_journalStack.o include/moc_journalStack.cpp

moc_journalSummary.o: include/moc_journalSummary.cpp  include/journalSummary.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_journalSummary.o include/moc_journalSummary.cpp

moc_journalTable.o: include/moc_journalTable.cpp  include/journalTable.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_journalTable.o include/moc_journalTable.cpp

moc_mainStack.o: include/moc_mainStack.cpp  include/mainStack.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_mainStack.o include/moc_mainStack.cpp

moc_reportStack.o: include/moc_reportStack.cpp  include/reportStack.h include/goBackLabel.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_reportStack.o include/moc_reportStack.cpp

moc_tabs.o: include/moc_tabs.cpp  include/tabs.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_tabs.o include/moc_tabs.cpp

include/moc_accountEditList.cpp: $(MOC) include/accountEditList.h
	$(MOC) include/accountEditList.h -o include/moc_accountEditList.cpp

include/moc_accountStack.cpp: $(MOC) include/accountStack.h
	$(MOC) include/accountStack.h -o include/moc_accountStack.cpp

include/moc_accountTable.cpp: $(MOC) include/accountTable.h
	$(MOC) include/accountTable.h -o include/moc_accountTable.cpp

include/moc_balanceElement.cpp: $(MOC) include/balanceElement.h
	$(MOC) include/balanceElement.h -o include/moc_balanceElement.cpp

include/moc_balanceSheetEditor.cpp: $(MOC) include/balanceSheetEditor.h
	$(MOC) include/balanceSheetEditor.h -o include/moc_balanceSheetEditor.cpp

include/moc_database.cpp: $(MOC) include/database.h
	$(MOC) include/database.h -o include/moc_database.cpp

include/moc_goBackLabel.cpp: $(MOC) include/goBackLabel.h
	$(MOC) include/goBackLabel.h -o include/moc_goBackLabel.cpp

include/moc_helpStack.cpp: $(MOC) include/helpStack.h
	$(MOC) include/helpStack.h -o include/moc_helpStack.cpp

include/moc_journalStack.cpp: $(MOC) include/journalStack.h
	$(MOC) include/journalStack.h -o include/moc_journalStack.cpp

include/moc_journalSummary.cpp: $(MOC) include/journalSummary.h
	$(MOC) include/journalSummary.h -o include/moc_journalSummary.cpp

include/moc_journalTable.cpp: $(MOC) include/journalTable.h
	$(MOC) include/journalTable.h -o include/moc_journalTable.cpp

include/moc_mainStack.cpp: $(MOC) include/mainStack.h
	$(MOC) include/mainStack.h -o include/moc_mainStack.cpp

include/moc_reportStack.cpp: $(MOC) include/reportStack.h
	$(MOC) include/reportStack.h -o include/moc_reportStack.cpp

include/moc_tabs.cpp: $(MOC) include/tabs.h
	$(MOC) include/tabs.h -o include/moc_tabs.cpp

####### Install

install:  

uninstall:  

