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

#ifndef WIDGET_H
#define WIDGET_H

#include <QtGui/QMainWindow>

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
