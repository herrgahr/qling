QT       += core gui webkit

TARGET = qling
TEMPLATE = app
DESTDIR=bin/
LLVM_INSTALL=$$(LLVM_INSTALL)
isEmpty(LLVM_INSTALL):error(The environment variable LLVM_INSTALL has to be defined!)
!exists($${LLVM_INSTALL}/bin/llvm-config):error($${LLVM_INSTALL}/bin/llvm-config not found!)

INCLUDEPATH+=$${LLVM_INSTALL}/../llvm/tools


DEFINES+=LLVM_INSTALL=\\\"$${LLVM_INSTALL}\\\"
DEFINES+=QLING_BASE_DIR=\\\"$${PWD}\\\"

QMAKE_CXXFLAGS+=$$system($${LLVM_INSTALL}/bin/llvm-config --cxxflags)\
-Wno-unused-parameter -Wno-strict-aliasing -std=c++11
QMAKE_CXXFLAGS_RELEASE += -fno-omit-frame-pointer -g

message($$QMAKE_LIBDIR_QT)
QMAKE_LIBDIR_QT=""

# rdynamic causes symbols to be exported even though this is not a lib
LIBS += -lm -ldl -fPIC -rdynamic

LIBS+=$$system($${LLVM_INSTALL}/bin/llvm-config --ldflags)

LIBS +=\
$$PWD/qt-hack/qatomic_sun.o\
-lclingMetaProcessor\
-lclingInterpreter\
-lclingUtils

LIBS+=\
-lclangFrontend \
-lclangSerialization \
-lclangDriver \
-lclangCodeGen \
-lclangParse \
-lclangSema \
-lclangEdit \
-lclangAnalysis \
-lclangAST \
-lclangLex \
-lclangBasic

LIBS+=$$system($${LLVM_INSTALL}/bin/llvm-config --libs)


LIBS += -lncurses

LIBS += -L/usr/lib

#clangFrontend.a clangSerialization.a clangDriver.a clangCodeGen.a clangParse.a clangSema.a clangEdit.a clangAnalysis.a clangAST.a clangLex.a clangBasic.a

 #for JitEventListener

LIBS+=-ldl

SOURCES +=\
    main.cpp\
    gui/widget.cpp \
    gui/consoleoutput.cpp \
    gui/codeinput.cpp \
    gui/codewidget.cpp \
    qling/jiteventlistener.cpp \
    qling/qling.cpp

HEADERS  +=\
    gui/widget.h \
    gui/consoleoutput.h \
    gui/codeinput.h \
    gui/codewidget.h \
    qling/jiteventlistener.h \
    qling/qling.h



