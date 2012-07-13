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

#include "gui/widget.h"
#include "qling/qling.h"

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

    Qling qling;

    MainWidget w(&qling);
    w.show();

    //make MainWidget available to the interpreter so the user can invoke
    //qling.foo(bar);
    qling.include("gui/widget.h");
    qling.exportToInterpreter(w,"qling");


    return a.exec();
}
