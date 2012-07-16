QT       += core gui

TARGET = qling
TEMPLATE = app
DESTDIR=bin/
LLVM_INSTALL=$$(LLVM_INSTALL)
isEmpty(LLVM_INSTALL):error(The environment variable LLVM_INSTALL has to be defined!)
!exists($${LLVM_INSTALL}/bin/llvm-config):error($${LLVM_INSTALL}/bin/llvm-config not found!)


#DEFINES+=NO_CONSOLE_REDIRECT
#DEFINES+=PATCHED_CLING

DEFINES+=LLVM_INSTALL=\\\"$${LLVM_INSTALL}\\\"

QMAKE_CXXFLAGS+=$$system($${LLVM_INSTALL}/bin/llvm-config --cxxflags)\
-Wno-unused-parameter -Wno-strict-aliasing


LIBS+=$$system($${LLVM_INSTALL}/bin/llvm-config --ldflags)

# rdynamic causes symbols to be exported even though this is not a lib
LIBS += -lm -ldl -fPIC -rdynamic\
qt-hack/qatomic_sun.o\
-lclingMetaProcessor\
-lclingInterpreter\
-lclingUtils

LIBS+=\
-lclangFrontend -lclangSerialization -lclangDriver -lclangCodeGen\
-lclangParse -lclangSema -lclangAnalysis -lclangRewrite\
-lclangAST -lclangLex -lclangBasic\
-lclangEdit

 #for JitEventListener
LIBS+=$$system($${LLVM_INSTALL}/bin/llvm-config --libs)

SOURCES +=\
    main.cpp\
    gui/widget.cpp \
    gui/consoleoutput.cpp \
    gui/codeinput.cpp \
    gui/codewidget.cpp \
    qling/jiteventlistener.cpp \
    qling/qling.cpp \
    qling/tests.cpp

HEADERS  +=\
    gui/widget.h \
    gui/consoleoutput.h \
    gui/codeinput.h \
    gui/codewidget.h \
    qling/jiteventlistener.h \
    qling/qling.h \
    qling/tests.h



