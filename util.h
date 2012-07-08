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

#ifndef UTIL_H
#define UTIL_H

#include "llvm/ExecutionEngine/JITEventListener.h"

//#include <QtCore/QString>
class QWidget;
extern "C"{

//QString qString(const char* c);
void setWindowTitle(QWidget* w,const char* c);
void foo();
}

//class JEL:public llvm::JITEventListener
//{
//    virtual void NotifyFunctionEmitted (const llvm::Function &, void *, size_t, const EmittedFunctionDetails &);
//    virtual void NotifyFreeingMachineCode (void *);
//};

#endif // UTIL_H
