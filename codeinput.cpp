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

CodeInput::CodeInput(QWidget *parent)
    :QLineEdit(parent)
    ,m_maxHistorySize(50)
{
    installEventFilter(this);
    QSettings s;
    m_history=s.value("history").toStringList();
    m_historyPos=m_history.size();
}

CodeInput::~CodeInput()
{
    QSettings s;
    s.setValue("history",m_history);
}

/** handle key-up, key-down and enter/return events
  * return true upon any of these events to prevent
  * QLineEdit from processing them further
  */
bool CodeInput::eventFilter(QObject *, QEvent *e)
{
    if(e->type()!=QEvent::KeyRelease && e->type()!=QEvent::KeyRelease)
        return false;
    QKeyEvent* ke=static_cast<QKeyEvent*>(e);
    switch(ke->key()){
    case Qt::Key_Up:
        moveInHistory(-1);
        return true;
    case Qt::Key_Down:
        moveInHistory(1);
        return true;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if(!text().isEmpty()){
            addToHistory(text());
            emit entered(text());
            clear();
        }
        return true;
    default:
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
        clear();
        return;
    }
    setText(m_history[m_historyPos]);
}
