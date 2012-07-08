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

#ifndef WIDGET_H
#define WIDGET_H

//#include <unistd.h>
//#include <fcntl.h>
//#include <stdio.h>
//#include <string>
//#include <sys/time.h>

#include <QtGui/QMainWindow>

//#include "bridge.h"

class ConsoleOutput;
class CodeWidget;

class QDockWidget;

namespace cling{
    class Interpreter;
}
class MainWidget : public QMainWindow
{
    Q_OBJECT

public:
    MainWidget(cling::Interpreter& interpreter);
    ~MainWidget();

    //void init();

public slots:
    void writeToConsole(const char* txt);
    void writeToConsole(const QString& txt);
    void enableConsole(bool enable);
protected:
    void closeEvent(QCloseEvent *);

private:
    ConsoleOutput* m_console;
    QDockWidget* m_consoleDock;
    CodeWidget* m_codeWidget;
    QDockWidget* m_gdbDock;
//    Gdb& m_gdb;
};

#endif // WIDGET_H
