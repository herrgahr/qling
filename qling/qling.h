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

namespace cling{
class MetaProcessor;
}
class Qling : public QObject
{
    Q_OBJECT
    void init();
public:
    explicit Qling(const char *llvm_install=0);
    Qling(int argc, char *argv[],const char* llvm_install=0);

    void addIncludePath(const QString& path);

    //shorthand-functions for #including stuff
    /** #include "header"*/
    void include(const QString& header);
    /** #include <header>*/
    void includeSystemHeader(const QString& header);

    void exportToInterpreter(const QString& typeName, void* obj, const QString& name);

    template<typename T>
    void exportToInterpreter(const QString& typeName, T obj, const QString& name){
        exportToInterpreter(typeName,(void*)&obj,name);
    }

    template<typename T>
    void exportToInterpreter(const QString& typeName, T* obj, const QString& name){
        exportToInterpreter(typeName,(void*)&obj,name);
    }

    void exportToInterpreter(QObject* obj,const QString& name);
    void exportToInterpreter(QObject& obj,const QString& name);
    void exportToInterpreter(const QObject& obj,const QString& name);

    //TODO: remove after refactoring!!!
    cling::Interpreter& interpreter(){return m_interpreter;}

public slots:
    void process(const QString& expr);
signals:
    void aboutToProcess();
    void aboutToExec();

private:

    //private member variables
    cling::Interpreter m_interpreter;
    cling::MetaProcessor* m_metaProcessor;
    JitEventListener m_jitEventListener;

};

#endif //QLING_H