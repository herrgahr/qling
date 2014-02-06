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

#QMAKE_LIBDIR_QT is set to /usr/lib (or alike) and needs to be unset. Otherwise
#the linker command looks something like:
#g++ -foo -bar -L/usr/lib -baz -L/your/llvm/install/path/configured/in/LLVM_INSTALL -lllvmLibFoo -lclangLibBar
# which causes the llvm & clang libs in /usr/lib to be picked up before the ones in LLVM_INSTALL, i.e. the ones
# used to build the cling libs. This leads to linker errors, of course.
#...so unset here and manually add -L/usr/lib to LIBS later
#I did not find a more elegant way to circumvent this problem - suggestions welcome :)
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

#taken from llvm/tools/cling/tools/driver/Makefile, variable USEDLIBS:
#clangFrontend.a clangSerialization.a clangDriver.a clangCodeGen.a clangParse.a clangSema.a clangEdit.a clangAnalysis.a clangAST.a clangLex.a clangBasic.a
#the following LIBS+=... directive should reflect the libs stated there.

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



