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

#include <sstream>

#include <QtGui/QApplication>

#include "llvm/Support/ManagedStatic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/HeaderSearchOptions.h"

#include "cling/Interpreter/Interpreter.h"

#include "widget.h"

/* Short outline of how to include cling into an application
  *
  * Create and configure and instance of cling::Interpreter
  *   (happens here in main.cpp)
  *
  * Create an instance of cling::MetaProcessor.
  *   (happens in codewidget.cpp)
  *
  * Feed code to the interpreter via MetaProcessor::process
  *   (happens in codewidget.cpp, too)
  *
  * That's it :)
  *
  * Note: you can directly feed code to the interpreter without going through
  * the MetaProcessor, but the MetaProcessor provides some comfort such as
  * "dot" commands. I.e. ".I", ".x", ".L" and all the stuff cling provides.
  *
  */

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
    cling::Interpreter interpreter(argc, argv,LLVM_INSTALL);
    if (interpreter.getOptions().Help) {
        return 0;
    }

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

    MainWidget w(interpreter);
    w.show();

    //make MainWidget available to the interpreter so the user can invoke
    //qling.foo(bar);
    interpreter.process("#include \"widget.h\"");
    std::stringstream initWidget;
    initWidget<<"MainWidget& qling=*(static_cast<MainWidget*>((void*)"<<(long)&w<<"));";
    std::string str=initWidget.str();
    interpreter.process(str);


    return a.exec();
}
