# Copyright (C) 2011 - 2012 Thomas Gahr <thomas.gahr@physik.uni-muenchen.de>

#   This file is part of qling, a Qt Interface to cling, the llvm-based
#   C++ interpreter.

#   cling: http://root.cern.ch/drupal/content/cling

#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public
#   License as published by the Free Software Foundation; either
#   version 2 of the License, or the version 3 of the License.

#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   General Public License for more details.

#   You should have received a copy of the GNU General Public License
#   along with this program; see the file COPYING.  If not, write to
#   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
#   Boston, MA 02110-1301, USA.

QT       += core gui

TARGET = qling
TEMPLATE = app

#DEFINES+=NO_CONSOLE_REDIRECT

#TODO smarter way to provide llvm-install-dir
INCLUDEPATH+=/home/thomas/opt/llvm-debug/include\
             /home/thomas/opt/llvm-debug/include/clang\
             /home/thomas/opt/llvm-debug/include/cling

QMAKE_CXXFLAGS+=-D_DEBUG -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS\
-D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -O3 -fomit-frame-pointer\
-fno-exceptions -fno-rtti -fPIC -Woverloaded-virtual -Wcast-qual -fPIC -Wno-unused-parameter -Wno-strict-aliasing

LIBS += -L/home/thomas/opt/llvm-debug/lib/
# rdynamic causes symbols to be exported even though this is not a lib
LIBS += -lm -ldl -fPIC -rdynamic\
qt-hack/qatomic_sun.o\
-lcling\
-lclingInterpreter\
-lclingUtils\
-lclangFrontend\
-lclangFrontendTool\
-lclangSerialization\
-lclangDriver\
-lclangCodeGen\
-lclangParse\
-lclangSema\
-lclangEdit\
-lclangAnalysis -lclangRewrite\
-lclangAST -lclangLex -lclangBasic\
-lLLVMLinker\
-lLLVMArchive\
-lLLVMBitReader\
-lLLVMJIT\
#MCJIT
-lLLVMMCJIT\
-lLLVMRuntimeDyld\
-lLLVMObject\
#/MCJIT
-lLLVMExecutionEngine \
-lLLVMX86Disassembler\
-lLLVMX86AsmParser\
-lLLVMX86CodeGen\
-lLLVMX86Desc\
-lLLVMSelectionDAG\
-lLLVMAsmPrinter\
-lLLVMMCParser\
-lLLVMCodeGen\
-lLLVMScalarOpts\
-lLLVMInstCombine\
-lLLVMTransformUtils\
-lLLVMipa\
-lLLVMAnalysis\
-lLLVMTarget\
-lLLVMX86AsmPrinter\
-lLLVMX86Utils\
-lLLVMCore\
-lLLVMX86Info\
-lLLVMMC\
-lLLVMSupport

SOURCES += main.cpp\
        widget.cpp \
    util.cpp \
    consoleoutput.cpp \
    codeinput.cpp \
    codewidget.cpp

HEADERS  += widget.h \
    util.h \
    consoleoutput.h \
    codeinput.h \
    codewidget.h


