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

private:
    bool m_enabled;
    int m_timerId;
    enum PIPES { READ, WRITE };
    int m_pipe[2];
    int m_oldStdOut;
    int m_oldStdErr;
    std::string m_captured;
    QTextStream m_fileStream;
    QFile m_file;
};

#endif

#endif // CONSOLEOUTPUT_H
