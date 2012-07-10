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

#ifndef CONSOLEOUTPUT_H
#define CONSOLEOUTPUT_H

#include <QTextEdit>
#include <QTextStream>
#include <QFile>

#ifdef NO_CONSOLE_REDIRECT
#include <QLabel>
struct ConsoleOutput:public QLabel{
    ConsoleOutput(){
        setAlignment(Qt::AlignCenter);
        setTextFormat(Qt::RichText);
        setText("<h1>Console disabled</h1>");
    }
};
#else

/** Console-widget
  *
  * Redirects stdout & stderr via pipe-magic. Only tested on linux.
  * I have no idea about working with pipes from C/C++, just found usable code
  * in some internet forum and used that. It works, but if anybody knows how to
  * do this more correct/elegant or spots an error, feel free to improve this
  * code!
  */
class ConsoleOutput : public QTextEdit
{
    Q_OBJECT
public:
    explicit ConsoleOutput(bool enabled=true);
    ~ConsoleOutput();
    void enable(bool enable);

protected:
    void timerEvent(QTimerEvent *);

signals:

public slots:
    void write(const QString& str);
    void enterCompileMode();
    void enterAppMode();

private:
    enum Mode{AppMode,CompileMode};
    bool m_enabled;
    int m_timerId;
    enum PIPES { READ, WRITE };
    int m_pipe[2];
    int m_oldStdOut;
    int m_oldStdErr;
    std::string m_captured;
    QTextStream m_fileStream;
    QFile m_file;
    Mode m_mode;
};

#endif

#endif // CONSOLEOUTPUT_H
