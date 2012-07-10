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

#include "codeinput.h"

#include <QKeyEvent>
#include <QSettings>
#include <QAbstractTextDocumentLayout>

CodeInput::CodeInput(QWidget *parent)
    :QTextEdit(parent)
    ,m_maxHistorySize(50)
    ,m_multiLineEnabled(false)
{
    installEventFilter(this);
    QSettings s;
    m_history=s.value("history").toStringList();
    m_historyPos=m_history.size();
    setFont(QFont("Monospace"));
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(this,SIGNAL(textChanged()),this,SLOT(fitSizeToText()));
}

CodeInput::~CodeInput()
{
    QSettings s;
    s.setValue("history",m_history);
}

namespace{
int calcIndent(QTextCursor c){
    //select text from beginning to cursor position
    c.movePosition(QTextCursor::Start,QTextCursor::KeepAnchor);
    QString txt=c.selectedText();
    int indent=txt.count('{')-txt.count('}');
    return qMax(0,indent);
}
QString getIndentString(QTextCursor c){
    return QString(2*::calcIndent(c),' ');
}
}

/** handle key-up, key-down and enter/return events
  * return true upon any of these events to prevent
  * QLineEdit from processing them further
  */
bool CodeInput::eventFilter(QObject *, QEvent *e)
{
    if(e->type()!=QEvent::KeyPress && e->type()!=QEvent::KeyRelease)
        return false;
    QKeyEvent* ke=static_cast<QKeyEvent*>(e);
    QString currentText=text();

    switch(ke->key()){
    case Qt::Key_Up:
        if(!m_multiLineEnabled || (ke->modifiers()==Qt::ControlModifier)){
            if(e->type()==QEvent::KeyRelease)
                moveInHistory(-1);
            return true;
        }
        return false;
    case Qt::Key_Down:
        if(!m_multiLineEnabled || (ke->modifiers()==Qt::ControlModifier)){
            if(e->type()==QEvent::KeyRelease)
                moveInHistory(1);
            return true;
        }
        return false;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if(currentText.isEmpty())
            return true;
        if(!m_multiLineEnabled || (ke->modifiers()==Qt::ControlModifier)){
            if(e->type()==QEvent::KeyRelease)
                return true;
            if(currentText.endsWith("\n"))
                currentText.remove(currentText.length()-1,1);
            submit();
            return true;
        }else if(e->type()==QEvent::KeyRelease){
            if(!m_multiLineEnabled)
                return false;

            //sloppy auto-indent
            QTextCursor c=textCursor();
            textCursor().insertText(::getIndentString(c));

        }
        return false;
    default:
        /* upon key-release - when QTextEdit already processed the input -
          *we might need to unindent the line if the first character was
          *a curly brace
          * (sloppy auto-indent)
          */
        if(e->type() == QEvent::KeyRelease){
            enum BraceType{Left,Right,None};
            BraceType braceType=None;
            if(ke->text()==QString("}"))
                braceType=Right;
            else if(ke->text()==QString("{"))
                braceType=Left;
            else return false;

            QTextCursor c=textCursor();
            c.select(QTextCursor::LineUnderCursor);
            QString t1=c.selectedText();
            int braceIndex=-1;
            if(t1.simplified()[0]=='}')
                braceIndex=t1.indexOf('}');
//            else if(t1.simplified()[0]=='{')
//                braceIndex=t1.indexOf('{');
            if(braceIndex>=0){
                t1.remove(0,braceIndex);
                QString indent(2*::calcIndent(textCursor()),' ');
                t1=indent+t1;
                c.removeSelectedText();
                c.insertText(t1);
            }
        }
        return false;
    }
}

void CodeInput::setMaxHistorySize(int s)
{
    //only values >=0 make sense
    m_maxHistorySize=qMax(0,s);
}

int CodeInput::maxHistorySize() const
{
    return m_maxHistorySize;
}

QString CodeInput::text() const
{
    return document()->toPlainText();
}

bool CodeInput::isEmpty() const
{
    return document()->isEmpty();
}

void CodeInput::fitSizeToText()
{
    int th=document()->documentLayout()->documentSize().toSize().height();
    setFixedHeight(th+m_yMargin);
}

void CodeInput::showEvent(QShowEvent *e)
{
    QTextEdit::showEvent(e);
    int wh=viewport()->height();
    int h=height();
    m_yMargin=h-wh;
    fitSizeToText();
}

void CodeInput::addToHistory(const QString &str)
{
    if(m_history.isEmpty() || m_history.back()!=str)
        m_history.append(str);

    //truncate to m_maxHistorySize
    //Need while-loop here in case m_maxHistory has been changed since last call
    while(m_history.size()>m_maxHistorySize)
        m_history.pop_front();

    m_historyPos=m_history.size();
    QSettings s;
    s.setValue("history",m_history);
}

void CodeInput::moveInHistory(int dir)
{
    m_historyPos+=dir;

    //if we just moved past m_history's bounds undo change and return
    if(m_historyPos<0 || m_historyPos>m_history.size()){
        m_historyPos-=dir;
        return;
    }

    //if we're past m_history's last entry, clear text
    if(m_historyPos==m_history.size()){
        //clear();
        setText(m_unsubmittedText);
        //fitSizeToText();
        return;
    }
    //if there is unsubmitted text in the line-edit, cache it
    if(m_historyPos==m_history.size()-1 && dir<0)
        m_unsubmittedText=text();
    setText(m_history[m_historyPos]);

    //I'd expect this to move the cursor to the end of the currently shown
    //text - yet it doesn't. Y u no work?
    QTextCursor c=textCursor();
    c.movePosition(QTextCursor::End);
    setTextCursor(c);

}

void CodeInput::submit()
{
    if(isEmpty())
        return;
    bool collectPreprocessor=false;
    bool submitChunk=false;
    QString chunk;
    QStringList split=text().split('\n');
    while(!split.isEmpty()){
        if(split[0][0]=='#'){
            if(!collectPreprocessor){
                submitChunk=!chunk.isEmpty();
                collectPreprocessor=true;
            }
        }else{
            if(collectPreprocessor){
                submitChunk=true;
                collectPreprocessor=false;
            }
        }
        if(submitChunk){
            //chunk will have one trailing '\n' that the user did not enter - remove
            chunk.remove(chunk.length()-1,1);
            emit entered(chunk);
            chunk=QString();
            submitChunk=false;
        }
        chunk+=split[0]+'\n';
        split.pop_front();
    }

    //chunk will have one trailing '\n' that the user did not enter - remove
    chunk.remove(chunk.length()-1,1);
    emit entered(chunk);
    addToHistory(text());
    clear();
    m_unsubmittedText=QString();
}

void CodeInput::enableMultiLineMode(bool e)
{
    m_multiLineEnabled=e;
}
