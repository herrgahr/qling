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

#ifndef NO_CONSOLE_REDIRECT

#include <cassert>
#include <iostream>
#include <string>

#include <QTimerEvent>
#include <QScrollBar>

#include "consoleoutput.h"

ConsoleOutput::ConsoleOutput(bool enabled)
    :QTextEdit()
    ,m_enabled(false)
    ,m_timerId(0)
{
    setReadOnly(true);
    setFont(QFont("Monospace"));
    QPalette pal=palette();
    pal.setColor(QPalette::Text,Qt::lightGray);
    pal.setColor(QPalette::Base,Qt::black);
    setPalette(pal);
    enable(enabled);
}

void ConsoleOutput::enable(bool enable)
{
    if(m_enabled==enable)
        return;
    m_enabled=enable;
    if(enable){
        m_pipe[READ] = 0;
        m_pipe[WRITE] = 0;

        pipe(m_pipe);
        m_oldStdOut=dup(fileno(stdout));
        m_oldStdErr=dup(fileno(stderr));
        if (m_oldStdOut == -1 || m_oldStdErr == -1)
            return;

        fflush(stdout);
        fflush(stderr);
        dup2(m_pipe[WRITE], fileno(stdout));
        dup2(m_pipe[WRITE], fileno(stderr));

        m_timerId=startTimer(100);
        m_file.setFileName("log");
        m_file.open(QFile::WriteOnly);
        m_fileStream.setDevice(&m_file);
    }else{
        killTimer(m_timerId);
        fflush(stdout);
        fflush(stderr);
        dup2(m_oldStdOut, fileno(stdout));
        dup2(m_oldStdErr, fileno(stderr));
        if (m_oldStdOut > 0)
            ::close(m_oldStdOut);
        if (m_oldStdErr > 0)
            ::close(m_oldStdErr);
        if (m_pipe[READ] > 0)
            ::close(m_pipe[READ]);
        if (m_pipe[WRITE] > 0)
            ::close(m_pipe[WRITE]);
        m_file.close();
    }
}

ConsoleOutput::~ConsoleOutput()
{
    enable(false);
}

void ConsoleOutput::timerEvent(QTimerEvent *e)
{
    if(e->timerId()!=m_timerId)
        return;
    assert(m_enabled&&"Console-timer called though it's not enabled.");
    dup2(m_pipe[WRITE], fileno(stdout));
    dup2(m_pipe[WRITE], fileno(stderr));
    fflush(stdout);
    fflush(stderr);
    m_captured.clear();

    std::string buf;
    const int bufSize = 1024;

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(m_pipe[READ],&fdset);

    timeval timeout;

    timeout.tv_sec=0;
    timeout.tv_usec=10000;

    buf.resize(bufSize);
    int bytesRead = 0;
    if(select(FD_SETSIZE,&fdset,NULL,NULL,&timeout)!=1)
        return;
    //if (!eof(m_pipe[READ]))
    //{
            bytesRead = read(m_pipe[READ], &(*buf.begin()), bufSize);
    //}
    while(bytesRead == bufSize)
    {
        m_captured += buf;
        bytesRead = 0;
        //if (!eof(m_pipe[READ]))
        //{
            bytesRead = read(m_pipe[READ], &(*buf.begin()), bufSize);
        //}
    }
//    append("Whoopwhoop");;
    if (bytesRead > 0)
    {
        buf.resize(bytesRead);
        m_captured += buf;
    }
//#define HULA
#ifdef HULA
    QString str=QString::fromStdString(m_captured);
    QString result;
    QStringList snippets=str.split('\033');
    foreach(QString snippet,snippets){
        int indexOfM=snippet.indexOf('m');
        QString colorCode=snippet.left(indexOfM);
        result+=snippet.right(snippet.length()-indexOfM-1);
    }

    m_textEdit->append(result);

#else
    QString str=QString::fromStdString(m_captured);
    QStringList lines=str.split('\n');
    for (int j=0;j<lines.size();++j){
        const QString& line=lines[j];
        QString result;
        QStringList snippets=line.split('\033');
        if(snippets.size()==1)
            result=line;
        else{
            for(int i=0;i<snippets.size();++i){
                const QString& snippet=snippets[i];
                int indexOfM=snippet.indexOf('m');
                QString colorCode=snippet.left(indexOfM);
                result+=snippet.right(snippet.length()-indexOfM-1);
                //foreach(QString snippet,snippets){
    //            int indexOfM=snippet.indexOf('m');
    //            QString colorCode=snippet.left(indexOfM);
    //            result+=snippet.right(snippet.length()-indexOfM-1);
            }
        }
        write(result);
        m_fileStream<<result;
        if(j!=lines.size()-1/* || line.isEmpty()*/){
            QTextCursor cursor=textCursor();
            cursor.insertText("\n");
            m_fileStream<<"\n";
        }
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());
    }
//    if(!lines.isEmpty() && lines.back().isEmpty())
//        textCursor().insertText("\n");
#endif
}

void ConsoleOutput::write(const QString &str)
{
    QTextCursor cursor=textCursor();
    if(!cursor.atEnd())
        cursor.movePosition(QTextCursor::End);
    cursor.insertText(str);
}

#endif //NO_CONSOLE_REDIRECT
