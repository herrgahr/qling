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


#include <unistd.h>

#include <cassert>
#include <iostream>
#include <string>

#include <QTimerEvent>
#include <QScrollBar>
#include <QElapsedTimer>

#include "consoleoutput.h"
#include <QAction>


static Qt::GlobalColor palDark[10] =
{Qt::black,
 Qt::darkRed,
 Qt::darkGreen,
 Qt::darkYellow,
 Qt::darkBlue,
 Qt::darkMagenta,
 Qt::darkCyan,
 Qt::lightGray
};
static Qt::GlobalColor palLight[10] =
{Qt::gray,
 Qt::red,
 Qt::green,
 Qt::yellow,
 Qt::blue,
 Qt::magenta,
 Qt::cyan,
 Qt::white
};
static Qt::GlobalColor* palF[2]={palDark,palLight};

/** OK, this is an ugly hack. But it kinda works.
 *The reason for this is timer-based polling of the redirection-pipes will hang if cout wants to flush bigger cunks inbetween polls
 *By tying our instance of std::ostream to std::cout, this buffer's sync method will get called before std::cout wants to flush. So do one last-resort polling then
 */
namespace {
static ConsoleOutput* s_instance = 0;
}
int stdout_tie_stream_buf::sync()
{
    if(!s_instance)
        return 0;
    s_instance->poll();
    return 0;
}


ConsoleOutput::ConsoleOutput(QWidget *parent, bool enabled)
    :QTextEdit(parent)
    ,m_enabled(false)
    ,m_timerId(0)
    ,m_mode(CompileMode)
    ,m_nextMode(CompileMode)
    ,m_parseColors(true)
    ,m_parseVar(0)
    ,m_stdout_tie_stream(&m_stdout_tie_stream_buf)
{
    setReadOnly(true);
    setFont(QFont("Monospace"));
    QPalette pal=palette();
    pal.setColor(QPalette::Text,Qt::lightGray);
    pal.setColor(QPalette::Base,Qt::black);
    setPalette(pal);
    enable(enabled);
    resetTimes();
    setUndoRedoEnabled(false);
    setTextInteractionFlags(Qt::TextBrowserInteraction);

    QAction* hideAction = new QAction("hide", this);
    addAction(hideAction);
    connect(hideAction, SIGNAL(triggered()), this, SLOT(hide()));

    QAction* clearAction = new QAction("clear", this);
    addAction(clearAction);
    connect(clearAction, SIGNAL(triggered()), this, SLOT(clear()));

    QAction* biggerAction = new QAction("bigger", this);
    addAction(biggerAction);
    biggerAction->setShortcut(Qt::CTRL + Qt::Key_Plus);
    connect(biggerAction, SIGNAL(triggered()), this, SLOT(bigger()));

    QAction* smallerAction = new QAction("smaller", this);
    addAction(smallerAction);
    smallerAction->setShortcut(Qt::CTRL + Qt::Key_Minus);
    connect(smallerAction, SIGNAL(triggered()), this, SLOT(smaller()));

    setContextMenuPolicy(Qt::ActionsContextMenu);

    s_instance = this;
    std::cout.tie(&m_stdout_tie_stream);
//        std::cout.setf(std::ios::unitbuf);
}

ConsoleOutput::ConsoleOutput(bool enabled)
    :QTextEdit()
    ,m_enabled(false)
    ,m_timerId(0)
    ,m_mode(CompileMode)
    ,m_nextMode(CompileMode)
    ,m_parseColors(true)
    ,m_parseVar(0)
    ,m_stdout_tie_stream(&m_stdout_tie_stream_buf)
{
    setReadOnly(true);
    setFont(QFont("Monospace"));
    QPalette pal=palette();
    pal.setColor(QPalette::Text,Qt::lightGray);
    pal.setColor(QPalette::Base,Qt::black);
    setPalette(pal);
    enable(enabled);
    resetTimes();
    setUndoRedoEnabled(false);
    setTextInteractionFlags(Qt::TextBrowserInteraction);
    s_instance = this;
    std::cout.tie(&m_stdout_tie_stream);
//    std::cout.setf(std::ios::unitbuf);
}

void ConsoleOutput::enable(bool enable)
{
    if(m_enabled == enable)
        return;
    m_enabled = enable;
    if(enable){
        m_pipe[READ] = 0;
        m_pipe[WRITE] = 0;

        pipe(m_pipe);
        m_oldStdOut = dup(fileno(stdout));
        m_oldStdErr = dup(fileno(stderr));
        if (m_oldStdOut == -1 || m_oldStdErr == -1)
            return;

        fflush(stdout);
        fflush(stderr);
        dup2(m_pipe[WRITE], fileno(stdout));
        dup2(m_pipe[WRITE], fileno(stderr));

        m_timerId = startTimer(100);
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
    s_instance = this;
}

void ConsoleOutput::timerEvent(QTimerEvent *e)
{
    if(e->timerId() != m_timerId)
        return;

    poll();
    if(m_captured.isEmpty())
        return;

    // make sure no text is selected, move cursor to end of document
    QTextCursor crs = textCursor();
    crs.clearSelection();
    crs.movePosition(QTextCursor::End);
    setTextCursor(crs);

    if (m_parseColors) {
        printColorCoded(m_captured);
    } else {
        QRegExp rx("\033\[[0-9;]*m");
        QString str = m_captured;
        str.remove(rx);
        textCursor().insertText(str);
    }
    if (document()->lineCount() > 1000) {
        QTextCursor crs = textCursor();
        crs.movePosition(QTextCursor::Start);
        crs.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, document()->lineCount() - 1000);
        crs.removeSelectedText();
    }
    scrollToBottom();
    m_captured.clear();
}

bool ConsoleOutput::poll()
{
    if(!m_enabled)
        return false;
    fflush(stdout);
    fflush(stderr);
    static const int bufSize = 1024;
    static char buf[bufSize];

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(m_pipe[READ],&fdset);

    timeval timeout;

    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;

    if(select(FD_SETSIZE, &fdset, NULL, NULL, &timeout) != 1)
        return false;

    int bytesRead = 0;

    do {
        bytesRead = read(m_pipe[READ], buf, bufSize);
        m_captured += QByteArray(buf, bytesRead);
    } while(bytesRead == bufSize);
    return true;
}

void ConsoleOutput::mouseDoubleClickEvent(QMouseEvent *e)
{
    hide();
}

void ConsoleOutput::write(const QString &str)
{
    show();
    QTextCursor cursor=textCursor();
    cursor.clearSelection();
    if(!cursor.atEnd())
        cursor.movePosition(QTextCursor::End);
    if(m_mode != m_nextMode){
        m_mode = m_nextMode;
        if(m_mode == CompileMode)
            write(QString("\n%1 cling output %1\n").arg(QString(33,'c')));
        else if(m_mode == AppMode)
            write(QString("\n%1 app output %1\n").arg(QString(34,'x')));
    }
    cursor.insertText(str);
}

void ConsoleOutput::enterCompileMode()
{
    m_nextMode = CompileMode;
}

void ConsoleOutput::enterAppMode()
{
    m_nextMode = AppMode;
}

void ConsoleOutput::enableColors(bool b)
{
    m_parseColors = b;
    unsigned highlight = 0;
    unsigned fgIdx = 7;
    unsigned bgIdx = 0;
    QTextCharFormat fmt=textCursor().charFormat();
    fmt.setForeground(palF[highlight][fgIdx]);
    fmt.setBackground(palDark[bgIdx]);
    fmt.setFontWeight(QFont::Normal);
    textCursor().setCharFormat(fmt);
}

void ConsoleOutput::printPerfTimers()
{
    QString html=
            QString("read: <b> %1msec</b><br/>"
                    "print:<b> %2msec</b><br/>"
                    )
            .arg(m_perfTimers[PerfRead])
            .arg(m_perfTimers[PerfPrint]);
    for(unsigned i = P0; i < PerfCount; ++i)
        html += QString("P%1: <b> %2msec</b><br/>").arg(i - P0).arg(m_perfTimers[i]);
    html+="<br/>";
    textCursor().clearSelection();
    textCursor().movePosition(QTextCursor::End);
    textCursor().insertHtml(html);
    scrollToBottom();
}

void ConsoleOutput::setParseVar(unsigned v)
{
    m_parseVar=v;
}

void ConsoleOutput::scrollToBottom()
{
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

void ConsoleOutput::bigger()
{
    QFont fnt(font());
    fnt.setPointSize(fnt.pointSize() + 1);
    setFont(fnt);
}

void ConsoleOutput::smaller()
{
    QFont fnt(font());
    fnt.setPointSize(fnt.pointSize() - 1);
    setFont(fnt);
}

void ConsoleOutput::printColorCoded(const QString &input)
{
    show();
    QTextCursor crs = textCursor();
    QTextCharFormat fmt = crs.charFormat();
    QRegExp rx("\033\[[0-9;]*m");
    unsigned highlight = 0;
    unsigned fgIdx = 7;
    unsigned bgIdx = 0;
    fmt.setForeground(palF[highlight][fgIdx]);
    fmt.setBackground(palDark[bgIdx]);
    fmt.setFontWeight(QFont::Normal);
    crs.setCharFormat(fmt);
    int pos = 0;
    int lastpos = 0;
    unsigned commit = 1;
    QString acum;
    while((pos=rx.indexIn(input,pos)) != -1) {
        QString cap = rx.cap();
        if(cap == QLatin1String("\033[m")) {
            highlight = 0;
            fgIdx = 7;
            bgIdx = 0;
            commit = 1;
        } else {
            foreach(QString str,
                    cap.mid(2, cap.length() - 3).split(';', QString::SkipEmptyParts)) {
                bool ok;
                int i = str.toInt(&ok);
                if(!ok)
                    continue;
                if (i == 1) {
                    commit |= unsigned(highlight - 1);
                    highlight = 1;
                } else if (i >= 30 && i < 38) {
                    commit |= unsigned(fgIdx - (i - 30));
                    fgIdx = i - 30;
                } else if (i >= 40 && i < 48) {
                    commit |= unsigned(bgIdx - (i - 40));
                    bgIdx = i - 40;
                } else if (i == 0) {
                    commit = 1;
                    highlight = 0;
                    fgIdx = 7;
                    bgIdx = 0;
                }
            }
        }

        acum+=input.mid(lastpos, pos - lastpos);
        if(commit) {
            crs.insertText(acum);
            fmt.setForeground(palF[highlight][fgIdx]);
            fmt.setBackground(palDark[bgIdx]);
            fmt.setFontWeight(highlight ? QFont::Bold : QFont::Normal);
            crs.setCharFormat(fmt);
            commit = 0;
            acum.clear();
        }

        pos += rx.matchedLength();
        lastpos = pos;
    }
    crs.insertText(acum + input.mid(lastpos));
}

void ConsoleOutput::time(unsigned timerId, QElapsedTimer &timer)
{
    m_perfTimers[timerId] = timer.nsecsElapsed();
    timer.restart();
}

void ConsoleOutput::timeAccum(unsigned timerId, QElapsedTimer &timer)
{
    m_perfTimers[timerId] += timer.nsecsElapsed();
    timer.restart();
}

void ConsoleOutput::resetTimes()
{
    for (unsigned i = 0; i < PerfCount; ++i)
        m_perfTimers[i] = 0;
}


