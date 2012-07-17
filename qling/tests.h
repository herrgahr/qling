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

#ifndef TESTS_H
#define TESTS_H

/* nope, these are not unit-tests
  * do not expect anything but hackish stuff in this file, only for testing
  * experimental stuff
  */

#ifdef PATCHED_CLING

#include "clang/AST/ASTConsumer.h"
#include "clang/Lex/PPCallbacks.h"


#include <vector>
#include <string>

struct ConstructorExtractor : public clang::ASTConsumer {
    ConstructorExtractor();
    virtual ~ConstructorExtractor();
    bool HandleTopLevelDecl(clang::DeclGroupRef DGR);
    void clear();
    std::vector<std::string> m_contructorNames;
    std::vector<std::string> m_classDecls;
    std::vector<std::string> m_baseTypes;
    std::vector<std::string> m_qobjectDerived;
    std::vector<clang::TagDecl*> m_qobjectDerivedDecls;
};

struct QObjectMacroFinder : public clang::PPCallbacks{
    void MacroExpands(const clang::Token &MacroNameTok,
                      const clang::MacroInfo *MI,
                      clang::SourceRange Range);
    std::vector<std::string> m_QObjectTokens;
    void clear();
};
#else
struct ConstructorExtractor{
    void clear(){}
};
struct QObjectMacroFinder{
    void clear(){}
};
#endif //PATCHED_CLING

#endif // TESTS_H
