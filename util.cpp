/* Copyright (C) 2006 - 2011 Thomas Gahr <thomas.gahr@physik.uni-muenchen.de>

   This file is part of the Trojita Qt IMAP e-mail client,
   http://trojita.flaska.net/

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

#include "util.h"
#include <QString>
#include <QWidget>
#include <iostream>
extern "C"{
//QString qString(const char* c)
//{
//    return QString(c);
//}

void setWindowTitle(QWidget *w, const char *c){
    w->setWindowTitle(QString(c));
}

//int q_atomic_decrement(volatile int *ptr)
//{
//    int ret;
//    asm volatile("lock\n"
//                 "decl %0\n"
//                 "setne %1"
//                 : "=a" (ptr), "=qm" (ret)
//                 :
//                 : "memory");
//    return ret;
//}

/*
    Q_CORE_EXPORT int q_atomic_decrement(volatile int *ptr);

q_atomic_decrement:
        lock
        decl (%rdi)
        setne %al
        ret
        .size q_atomic_decrement,.-q_atomic_decrement

        .globl q_atomic_test_and_set_int
        .type q_atomic_test_and_set_int, @function
        .section .text, "ax"
        .align 16


inline int QBasicAtomicInt::fetchAndAddOrdered(int valueToAdd)
{
    asm volatile("lock\n"
                 "xaddl %0,%1"
                 : "=r" (valueToAdd), "+m" (_q_value)
                 : "0" (valueToAdd)
                 : "memory");
    return valueToAdd;
}

  */

}

//#include "llvm/Function.h"
//#include "llvm/Support/raw_ostream.h"
//void JEL::NotifyFunctionEmitted(const llvm::Function &f, void *, size_t, const llvm::JITEventListener::EmittedFunctionDetails &)
//{
//    llvm::errs()<<"Function emitted\n";
//    if(f.hasName()){
//        llvm::errs()<<"Function has name: "<<f.getName()<<"\n";
//    }else
//        llvm::errs()<<"(has no name)\n";
//}

//void JEL::NotifyFreeingMachineCode(void *)
//{
//}
