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

#include <iostream>

#include <QApplication>
#include <QLibraryInfo>  //for Qt header locations
#include <QElapsedTimer>
#include <iostream>
#include <QProcess>
#include <QEventLoop>
#include <QDebug>
#include <QTimer>
#include <QStringList>


#include "llvm/ExecutionEngine/ExecutionEngine.h"

#include "clang/Basic/FileManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/HeaderSearchOptions.h"
#include "clang/Frontend/ASTUnit.h"

#include "cling/Interpreter/ValuePrinter.h"
#include "cling/Interpreter/ValuePrinterInfo.h"
#include "cling/Interpreter/CValuePrinter.h"
#include "cling/MetaProcessor/MetaProcessor.h"

namespace Eigen{
//to be able to use Eigen, we need to provide the cpuid function, which replaces inline asm in eigen-patched/
namespace qlinghack{
void cpuid(int (&abcd)[4], int func, int id) {
#  if defined(__GNUC__) && ( defined(__i386__) || defined(__x86_64__) )
#    if defined(__PIC__) && defined(__i386__)
       // Case for x86 with PIC
         __asm__ __volatile__ ("xchgl %%ebx, %%esi;cpuid; xchgl %%ebx,%%esi": "=a" (abcd[0]), "=S" (abcd[1]), "=c" (abcd[2]), "=d" (abcd[3]) : "a" (func), "c" (id));
#    else
       // Case for x86_64 or x86 w/o PIC
         __asm__ __volatile__ ("cpuid": "=a" (abcd[0]), "=b" (abcd[1]), "=c" (abcd[2]), "=d" (abcd[3]) : "a" (func), "c" (id) );
#    endif
#  elif defined(_MSC_VER)
#    if (_MSC_VER > 1500) && ( defined(_M_IX86) || defined(_M_X64) )
        __cpuidex((int*)abcd,func,id)
#    endif
#  endif

}
}
}


//force linking to these functions
void neverCalled(){
    cling::ValuePrinterInfo VPI(0, 0);
    //cling::printValuePublicDefault(llvm::outs(), 0, VPI);
    //cling::printValuePublic(llvm::outs(), 0, )
    cling_PrintValue(0, 0, 0);
    //cling::flushOStream(llvm::outs());
}

void Qling::init(const char* llvm_install)
{
    m_interpreter.AddIncludePath(".");
    m_interpreter.AddIncludePath("../"); // we're inside bin/ so add ../ as well
    m_interpreter.AddIncludePath("../qt-hack/");

    QString QtIncDir=QLibraryInfo::location(QLibraryInfo::HeadersPath);
    addIncludePath(QtIncDir);
    //add some Qt include paths, added as needed
    addIncludePath(QtIncDir + "/QtCore");
    addIncludePath(QtIncDir + "/QtGui");

    m_interpreter.getExecutionEngine()->RegisterJITEventListener(&m_jitEventListener);
    connect(&m_jitEventListener, SIGNAL(aboutToExecWrappedFunction()),
            this, SIGNAL(aboutToExec()));
    m_metaProcessor = new cling::MetaProcessor(m_interpreter, llvm::outs());

    enableTiming(false);

    m_interpreter.process("#define QLING_QATOMIC_HACK");
    m_interpreter.process("#include \"qt-hack/QtCore/qatomic.h\"");
    m_interpreter.process("#define EIGEN_QLING_HACK");
    QString eigenPath(QLING_BASE_DIR);
    eigenPath += "/eigen-patched";
    addIncludePath(eigenPath);
    process(QString("#define LLVM_INSTALL \"%1\"").arg(LLVM_INSTALL));
}

namespace{
/* in case we don't want to pass any command line arguments to cling, we still
  * have to pass the application's path as argv[0] in Cling::Interpreter's
  * constructor (i.e. when initializing m_interpreter). To make this work in the
  * initializer, create a static "fake" const char** argv
  */
const char** makeArgv(){
    static std::string argv0(QApplication::applicationFilePath().toStdString());
    static char a0[] = "-fcolor-diagnostics";
    static char a1[] = "-std=c++11";
    static const char* argv[3] = {argv0.data(), a0, a1};
    return argv;
}
const char* llvmInstall(const char* llvm_install) {
    return llvm_install ? llvm_install : LLVM_INSTALL;
}
}

Qling::Qling(const char *llvm_install)
    :m_interpreter(3, ::makeArgv(), ::llvmInstall(llvm_install))
{
    init(::llvmInstall(llvm_install));
}

Qling::Qling(int argc, const char *argv[], const char *llvm_install)
    :m_interpreter(argc,argv,::llvmInstall(llvm_install))
{
    init(::llvmInstall(llvm_install));
}

Qling::~Qling()
{
    delete m_metaProcessor;
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
    QString rawType = typeName.simplified();
    bool exportAsPointer = rawType.endsWith('*');
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
            .arg(exportAsPointer ? ' ' : '*')
            .arg(rawType)
            .arg(intptr_t(obj));
    process(stmt);
}

void Qling::exportToInterpreter(QObject *obj,const QString& name)
{
    exportToInterpreter(QString("%1*").arg(obj->metaObject()->className()),
                        (void*)&obj, name);
}

void Qling::exportToInterpreter(QObject& obj,const QString& name)
{
    exportToInterpreter(QString("%1&").arg(obj.metaObject()->className()),
                        (void*)&obj, name);
}

void Qling::exportToInterpreter(const QObject& obj,const QString& name)
{
    exportToInterpreter(QString("const %1&").arg(obj.metaObject()->className()),
                        (void*)&obj, name);
}

int Qling::process(const QString &expr)
{
    emit aboutToProcess();
    cling::Interpreter::CompilationResult compilationResult;
    if(m_timing){
        QElapsedTimer timer;
        timer.start();
        int ret = m_metaProcessor->process(qPrintable(expr), compilationResult, nullptr);
        std::cout<<"Elapsed time: "<<timer.elapsed()<<std::endl;
        return ret;
    }
    //unfortunately, MetaProcessor::process does not indicate if there was an
    //error - it only returns "expected indentation", i.e. >0 if the input
    //was incomplete
    int indent = m_metaProcessor->process(qPrintable(expr), compilationResult, nullptr);
    //dumbed-down version: just look for the string. If it's a false positive
    //(for whatever reason) then moc will just fail and nothing is lost
    if(!indent && expr.contains("Q_OBJECT"))
        moc(expr);
    return indent;
}

int Qling::processUserInput(const QString &expr)
{
    int indent = process(expr);
    return indent;
}

void Qling::processUserInputMultiLine(const QString &expr)
{
    QString ppStuff;
    QStringList code;
    for(const QString& str: expr.split('\n')) {
        if(str.startsWith('#')) {
            ppStuff += str;
            ppStuff += '\n';
        } else {
            code.append(str + '\n');
        }
    }
    if(!ppStuff.isEmpty())
        process(ppStuff);
    if(!code.isEmpty()) {
        for(const QString& cd: code)
            process(cd);
    }
}

void Qling::enableTiming(bool b)
{
    m_timing = b;
}

void Qling::moc(const QString &input)
{
    emit aboutToMoc();

    std::cout<<"MOCing input...\n";
    QString mocPath = QLibraryInfo::location(QLibraryInfo::BinariesPath)
            +QString("/moc");
    QProcess mocProcess;
    QEventLoop el;

    m_mocOutput.clear();

    mocProcess.start(mocPath);
    connect(&mocProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(mocWrote()));
    connect(&mocProcess, SIGNAL(finished(int)), &el, SLOT(quit()));
    mocProcess.write(input.toAscii());
    //moc reads until EOF so close the write-channel
    mocProcess.closeWriteChannel();

    //if something goes wrong, cancel the event-loop after 10s
    QTimer::singleShot(10000, &el, SLOT(quit()));
    el.exec();

    if(!m_mocOutput.isEmpty()){
        std::cout<<"... moc is done\n";
        m_interpreter.declare(m_mocOutput.toStdString());
    }else{
        std::cout<<"... moc failed\n";
    }

    emit doneMocing();
}

void Qling::mocWrote()
{
    QProcess* p = qobject_cast<QProcess*>(sender());
    if(!p)
        return;
    QByteArray ba = p->readAll();
    m_mocOutput = QString(ba);
}

void Qling::mocDone()
{
    QProcess* p = qobject_cast<QProcess*>(sender());
    if(!p)
        return;
}

