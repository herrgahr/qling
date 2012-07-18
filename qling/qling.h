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

#ifndef QLING_H
#define QLING_H

#include <QObject>


#include "cling/Interpreter/Interpreter.h"
#include "jiteventlistener.h"
#include "tests.h"

namespace cling{
class MetaProcessor;
}

//TODO write unit-test!!!
class Qling : public QObject
{
    Q_OBJECT
    void init();
public:
    explicit Qling(const char *llvm_install=0);
    Qling(int argc, const char *argv[],const char* llvm_install=0);
    virtual ~Qling();

    void addIncludePath(const QString& path);

    //shorthand-functions for #including stuff
    /** #include "header"*/
    void include(const QString& header);
    /** #include <header>*/
    void includeSystemHeader(const QString& header);

    //TODO test these!!!
    void exportToInterpreter(const QString& typeName, void* obj, const QString& name);

    template<typename T>
    void exportToInterpreter(const QString& typeName, T obj, const QString& name){
        exportToInterpreter(typeName,(void*)&obj,name);
    }

    template<typename T>
    void exportToInterpreter(const QString& typeName, T* obj, const QString& name){
        exportToInterpreter(typeName,(void*)obj,name);
    }

    void exportToInterpreter(QObject* obj,const QString& name);
    void exportToInterpreter(QObject& obj,const QString& name);
    void exportToInterpreter(const QObject& obj,const QString& name);

public slots:
    int process(const QString& expr);
    int processUserInput(const QString& expr);
    void enableTiming(bool b);
signals:
    void aboutToProcess();
    void aboutToExec();
    void aboutToMoc();
    void doneMocing();

private slots:
    void mocWrote();
    void mocDone();

private:
    void moc(const QString& input);
    //private member variables
    //testing-stuff
    ConstructorExtractor* m_ConstructorExtractor;//don't own, don't delete
    QObjectMacroFinder* m_QObjectMacroFinder;//don't own, don't delete
    cling::Interpreter m_interpreter;
    cling::MetaProcessor* m_metaProcessor;
    JitEventListener m_jitEventListener;
    QString m_mocOutput;
    bool m_timing;
};

#endif //QLING_H
