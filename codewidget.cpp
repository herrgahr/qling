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

#include "codewidget.h"
#include "codeinput.h"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QScrollBar>

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
    m_codeInput=new CodeInput;
    layout->addWidget(m_codeInput);
    m_codeInput->setFocus();
    connect(m_codeInput,SIGNAL(entered(QString)),this,SLOT(processCode(QString)));
}


void CodeWidget::processCode(const QString& str)
{
    static int lastIndent=0;
    QTextCursor cursor=m_codeOutput->textCursor();
    if(!cursor.atEnd())
        cursor.movePosition(QTextCursor::End);
    QTextCharFormat format=cursor.charFormat();
    int indent = m_metaProcessor->process(str.toStdString().c_str());
    QString indentString;
    if(indent > 0){
        format.setBackground(Qt::yellow);
        for(int i=0;i<qMin(lastIndent,indent);++i)
            indentString+=QString("  ");
    }else if(lastIndent>0)
        format.setBackground(palette().base());
    lastIndent=indent;
    cursor.insertText(indentString+str+QString("\n"),format);
    m_codeOutput->verticalScrollBar()->setValue(m_codeOutput->verticalScrollBar()->maximum());
}

