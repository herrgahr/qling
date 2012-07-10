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

#include "codewidget.h"
#include "codeinput.h"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QCheckBox>

#include "clang/Frontend/CompilerInstance.h"
#include <cling/MetaProcessor/MetaProcessor.h>
#include <clang/Frontend/ASTUnit.h>

CodeWidget::CodeWidget(cling::Interpreter& interpreter)
    :QWidget()
    ,m_interpreter(interpreter)
    ,m_metaProcessor(new cling::MetaProcessor(interpreter))
{
    setContentsMargins(0,0,0,0);
    QVBoxLayout* layout=new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    m_codeOutput=new QTextEdit;
    m_codeOutput->setReadOnly(true);
    m_codeOutput->setFont(QFont("Monospace"));
    layout->addWidget(m_codeOutput);

    QHBoxLayout* hl=new QHBoxLayout;
    QCheckBox* enableMultiLine=new QCheckBox("multi-line");
    hl->addWidget(enableMultiLine);

    m_codeInput=new CodeInput;
    hl->addWidget(m_codeInput);
    connect(enableMultiLine,SIGNAL(toggled(bool)),
            m_codeInput,SLOT(enableMultiLineMode(bool)));
    connect(enableMultiLine,SIGNAL(released()),
            m_codeInput,SLOT(setFocus()));
    layout->addLayout(hl);
    m_codeInput->setFocus();
    connect(m_codeInput,SIGNAL(entered(QString)),this,SLOT(processCode(QString)));
}


void CodeWidget::processCode(const QString& str)
{
    static int lastIndent=0;
    QTextCursor cursor=m_codeOutput->textCursor();

    //user might've selected some text so clear selection
    cursor.clearSelection();

    if(!cursor.atEnd())
        cursor.movePosition(QTextCursor::End);

    //submit code
    emit aboutToProcessCode();
    int indent = m_metaProcessor->process(str.toStdString().c_str());

    QTextCharFormat format=cursor.charFormat();
    QString indentString;

    if(indent > 0){
        //use yellow background to mark code that is part of incomplete input
        format.setBackground(Qt::yellow);
        for(int i=0;i<qMin(lastIndent,indent);++i)
            indentString+=QString("  ");
    }else if(lastIndent>0)
        format.setBackground(palette().base());
    lastIndent=indent;
    cursor.insertText(indentString+str+QString("\n"),format);
    m_codeOutput->verticalScrollBar()->setValue(m_codeOutput->verticalScrollBar()->maximum());
}

