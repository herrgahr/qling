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

#include "qling.h"

#include <QApplication>
#include <QLibraryInfo>  //for Qt header locations

#include "llvm/ExecutionEngine/ExecutionEngine.h"

#include "clang/Basic/FileManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/HeaderSearchOptions.h"
#include "clang/Frontend/ASTUnit.h"

#include "cling/Interpreter/ValuePrinter.h"
#include "cling/Interpreter/ValuePrinterInfo.h"
#include "cling/Interpreter/CValuePrinter.h"
#include "cling/MetaProcessor/MetaProcessor.h"

//force linking to these functions
void neverCalled(){
    cling::ValuePrinterInfo VPI(0, 0);
    cling::printValueDefault(llvm::outs(), 0, VPI);
    cling_PrintValue(0, 0, 0);
    cling::flushOStream(llvm::outs());
}

void Qling::init()
{
    m_interpreter.AddIncludePath("../");
    m_interpreter.AddIncludePath("../qt-hack/");

    QString QtIncDir=QLibraryInfo::location(QLibraryInfo::HeadersPath);
    addIncludePath(QtIncDir);
    addIncludePath(QtIncDir+"/QtCore");
    addIncludePath(QtIncDir+"/QtGui");

    m_interpreter.AddIncludePath("/home/thomas/opt/llvm-debug/include");
    m_interpreter.process("#define __HULA__");
    m_interpreter.process("extern \"C\" int q_atomic_decrement(volatile int *ptr);");

    m_interpreter.process("#include \"qatomic.h\"");
    //m_interpreter.processLine("#define CLING_HACK");
    //m_interpreter.processLine("#include \"eigen3-patched/Eigen/Dense\"");

    m_interpreter.getExecutionEngine()->RegisterJITEventListener(&m_jitEventListener);
    connect(&m_jitEventListener,SIGNAL(aboutToExecWrappedFunction()),
            this,SIGNAL(aboutToExec()));
    m_metaProcessor=new cling::MetaProcessor(m_interpreter);
}

namespace{
/* in case we don't want to pass any command line arguments to cling, we still
  * have to pass the application's path as argv[0] in Cling::Interpreter's
  * constructor (i.e. when initializing m_interpreter). To make this work in the
  * initializer, create a static "fake" const char** argv
  */
const char* const* makeArgv(){
    static std::string argv0(QApplication::applicationFilePath().toStdString());
    static const char* argv[1]={argv0.data()};
    return argv;
}
}

Qling::Qling(const char *llvm_install)
    :m_interpreter(1,
                   ::makeArgv(),
                   llvm_install?llvm_install:LLVM_INSTALL)
{
    init();
}

Qling::Qling(int argc, char *argv[], const char *llvm_install)
    :m_interpreter(argc,argv,llvm_install?llvm_install:LLVM_INSTALL)
{
    init();
}

void Qling::addIncludePath(const QString &path)
{
    m_interpreter.AddIncludePath(qPrintable(path));
}

void Qling::include(const QString &header)
{
    process(QString("#include \"%1\"").arg(header));
}

void Qling::includeSystemHeader(const QString &header)
{
    process(QString("#include <%1>").arg(header));
}

void Qling::exportToInterpreter(const QString &typeName, void *obj,const QString& name)
{
    QString stmt;
    QString rawType=typeName.simplified();
    bool exportAsPointer=rawType.endsWith('*');
    //use regexp?
    rawType.remove('&');
    rawType.remove('*');
    rawType=rawType.trimmed();

    //produce sth like:
    //Type& qling=*static_cast<Type*>((void*)47315771);"
    //or
    //Type* qling=static_cast<Type*>((void*)47315771);"
    stmt=QString("%1 %2=%3static_cast<%4*>((void*)%5);")
            .arg(typeName)
            .arg(name)
            .arg(exportAsPointer?' ':'*')
            .arg(rawType)
            .arg(intptr_t(obj));
    process(stmt);
}

void Qling::exportToInterpreter(QObject *obj,const QString& name)
{
    exportToInterpreter(QString("%1*").arg(obj->metaObject()->className()),
                        (void*)&obj,name);
}

void Qling::exportToInterpreter(QObject& obj,const QString& name)
{
    exportToInterpreter(QString("%1&").arg(obj.metaObject()->className()),
                        (void*)&obj,name);
}

void Qling::exportToInterpreter(const QObject& obj,const QString& name)
{
    exportToInterpreter(QString("const %1&").arg(obj.metaObject()->className()),
                        (void*)&obj,name);
}

void Qling::process(const QString &expr)
{
    emit aboutToProcess();
    m_interpreter.process(expr.toStdString());
}

