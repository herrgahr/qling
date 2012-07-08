/* Copyright (C) 2011 - 2012 Thomas Gahr <thomas.gahr@physik.uni-muenchen.de>

   This file is part of qling, a Qt Interface to cling, the llvm-based
   C++ interpreter.

   cling: http://root.cern.ch/drupal/content/cling

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or the version 3 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <QtGui/QApplication>
#include "widget.h"
//------------------------------------------------------------------------------
// CLING - the C++ LLVM-based InterpreterG :)
// version: $Id: cling.cpp 40347 2011-07-23 21:12:28Z axel $
// author:  Lukasz Janyst <ljanyst@cern.ch>
//------------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include "clang/Basic/LangOptions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/HeaderSearchOptions.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/FileManager.h"

#include "llvm/Support/Signals.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/ManagedStatic.h"

#include "cling/Interpreter/Interpreter.h"
#include "cling/UserInterface/UserInterface.h"

//need to include this to force linking libLLVMMCJIT
//#include "llvm/ExecutionEngine/MCJIT.h"

#include "util.h"

#include <string>

#include "llvm/Pass.h"
#include "llvm/Function.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

extern "C"{

void foo(){
    std::cout<<"foo called\n";
}

void bar(){
    std::cout<<"bar called\n";
}


int q_atomic_test_and_set_int(volatile int *ptr, int expected, int newval);
int q_atomic_test_and_set_ptr(volatile void *ptr, void *expected, void *newval);
int q_atomic_increment(volatile int *ptr);
int q_atomic_decrement(volatile int *ptr);
int q_atomic_set_int(volatile int *ptr, int newval);
void *q_atomic_set_ptr(volatile void *ptr, void *newval);
int q_atomic_fetch_and_add_int(volatile int *ptr, int value);
void *q_atomic_fetch_and_add_ptr(volatile void *ptr, qptrdiff value);

}//extern "C"

//------------------------------------------------------------------------------
// Let the show begin
//------------------------------------------------------------------------------
int main( int argc, char **argv )
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("qling");
    QApplication::setOrganizationName("qling");

    llvm::llvm_shutdown_obj shutdownTrigger;

    //llvm::sys::PrintStackTraceOnErrorSignal();
    //llvm::PrettyStackTraceProgram X(argc, argv);

    //---------------------------------------------------------------------------
    // Set up the interpreter
    //---------------------------------------------------------------------------
    cling::Interpreter interpreter(argc, argv,"/home/thomas/opt/llvm-debug/");
    if (interpreter.getOptions().Help) {
        return 0;
    }

    clang::CompilerInstance* CI = interpreter.getCI();

    interpreter.AddIncludePath(".");
    interpreter.AddIncludePath("./qt-hack/");
    interpreter.AddIncludePath("/usr/include/qt4");
    interpreter.AddIncludePath("/usr/include/qt4/QtCore");
    interpreter.AddIncludePath("/usr/include/qt4/QtGui");
    interpreter.AddIncludePath("/home/thomas/opt/llvm-debug/include");
    interpreter.process("#include \"llvm/Support/raw_ostream.h\"");
    interpreter.process("#define __HULA__");
    interpreter.process("extern \"C\" int q_atomic_decrement(volatile int *ptr);");

    interpreter.process("#include \"qatomic.h\"");
    //interpreter.processLine("#define CLING_HACK");
    //interpreter.processLine("#include \"eigen3-patched/Eigen/Dense\"");

    for (size_t I = 0, N = interpreter.getOptions().LibsToLoad.size();
         I < N; ++I) {
        interpreter.loadFile(interpreter.getOptions().LibsToLoad[I]);
    }


    MainWidget w(interpreter);
    w.show();

    //make MainWidget available to the interpreter so the user can invoke
    //gui.foo(bar);
    interpreter.process("#include \"widget.h\"");
    std::stringstream initWidget;
    initWidget<<"MainWidget& gui=*(static_cast<MainWidget*>((void*)"<<(long)&w<<"));";
    std::string str=initWidget.str();
    interpreter.process(str);


    return a.exec();
}
