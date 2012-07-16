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

#include "tests.h"

/* nope, these are not unit-tests
  * do not expect anything but hackish stuff in this file, only for testing
  * experimental stuff
  */

#ifdef PATCHED_CLING

#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclGroup.h"
#include "clang/AST/Decl.h"
#include "clang/AST/CXXInheritance.h"
#include "clang/AST/Mangle.h"
#include "clang/Sema/Scope.h"
#include "clang/Lex/Token.h"

using namespace clang;

bool baseIsQObject(const CXXBaseSpecifier *Specifier, CXXBasePath &, void *){
  return Specifier->getType().getAsString()=="class QObject";
}

bool ConstructorExtractor::HandleTopLevelDecl(DeclGroupRef DGR)
{
    if(DGR.isSingleDecl()){
        Decl* decl=DGR.getSingleDecl();
        if(isa<clang::TagDecl>(decl)){
            clang::TagDecl* tagDecl=dyn_cast<clang::TagDecl>(decl);
            if(!tagDecl->isClass() && !tagDecl->isStruct())
                return true;
            if(!tagDecl->getDefinition())
                return true;
            m_classDecls.push_back(tagDecl->getNameAsString());
            CXXRecordDecl* recDecl=dyn_cast<CXXRecordDecl>(tagDecl);
            if(!recDecl->getNumBases())
                return true;
            CXXBaseSpecifier baseSpec=*recDecl->bases_begin();
            std::string baseName=baseSpec.getType().getAsString();

            m_baseTypes.push_back(baseName);
            clang::CXXBasePaths basePaths;
            if(recDecl->lookupInBases(baseIsQObject,0,basePaths)){
                m_qobjectDerived.push_back(tagDecl->getNameAsString());
                m_qobjectDerivedDecls.push_back(tagDecl);
            }
        }
    }
    for (DeclGroupRef::iterator I = DGR.begin(), E = DGR.end(); I != E; ++I){
        FunctionDecl* FD = dyn_cast<FunctionDecl>(*I);

        if (FD) {
          if (FD->getNameAsString().find("__cling_Un1Qu3")!=llvm::StringRef::npos)
            continue;
          CXXConstructorDecl* ConstrDecl=dyn_cast<CXXConstructorDecl>(FD);
          //if this is a constructor and it is a definition (has a body)
          if(ConstrDecl && ConstrDecl->hasBody()){
              std::string mangledName;
              llvm::raw_string_ostream RawStr(mangledName);
              llvm::OwningPtr<MangleContext>
                Mangle(ConstrDecl->getASTContext().createMangleContext());
              clang::CXXCtorType ctorType;
              if(!ConstrDecl->getParent()->isAbstract())
                  ctorType=clang::Ctor_Complete;
              else
                  ctorType=clang::Ctor_Base;
              Mangle->mangleCXXCtor(ConstrDecl,
                                    ctorType,
                                    RawStr);
              RawStr.flush();
              m_contructorNames.push_back(mangledName);
          }
        }
    }
    return true;
}

void ConstructorExtractor::clear()
{
    m_contructorNames.clear();
    m_classDecls.clear();
    m_baseTypes.clear();
    m_qobjectDerived.clear();
    m_qobjectDerivedDecls.clear();
}

ConstructorExtractor::ConstructorExtractor()
{
}

ConstructorExtractor::~ConstructorExtractor()
{
}

void QObjectMacroFinder::MacroExpands(const Token &MacroNameTok, const MacroInfo *MI, SourceRange Range)
{
    std::string token(MacroNameTok.getIdentifierInfo()->getName());
    if(token=="Q_OBJECT"){
        //llvm::errs()<<token<<"\n";
        m_QObjectTokens.push_back(token);
    }
}

void QObjectMacroFinder::clear()
{
    m_QObjectTokens.clear();
}

#endif //PATCHED_CLING

