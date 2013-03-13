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

#include <iostream>

#include <QString>
#include <QSettings>
#include <QDockWidget>
#include <QApplication>
#include <QLabel>

#include "widget.h"

#include "consoleoutput.h"
#include "codewidget.h"

#include "qling/qling.h"


MainWidget::MainWidget(Qling* qling)
    :QMainWindow()
    ,m_codeWidget(new CodeWidget(qling))
    ,m_interpreter(qling)
    ,m_console(new ConsoleOutput(this, true))
{
    m_console->installEventFilter(this);

    setCentralWidget(m_codeWidget);
    m_codeWidget->setObjectName("codeWidget");

    QDockWidget* consoleDock = new QDockWidget("stdout && stderr");//OK, just noticed I'm programming too much "&&" is clearly nonsense in this string. But I like it :)
    consoleDock->setWidget(m_console);
    consoleDock->setObjectName("consoleDock");
    addDockWidget(Qt::BottomDockWidgetArea,consoleDock);

    connect(qling, SIGNAL(aboutToExec()),
            m_console, SLOT(enterAppMode()));
    connect(qling, SIGNAL(aboutToProcess()),
            m_console, SLOT(enterCompileMode()));

    QSettings s;
    restoreGeometry(s.value("geometry").toByteArray());
    restoreState(s.value("mainWindowState").toByteArray());
}

Qling *MainWidget::qling()
{
    return m_interpreter;
}

void MainWidget::writeToConsole(const char *txt)
{
    writeToConsole(QString(txt));
}

void MainWidget::writeToConsole(const QString& txt)
{
    if(!m_console->isVisible())
        m_console->show();
    m_console->write(txt);
}

void MainWidget::enableConsole(bool enable)
{
    m_console->enable(enable);
}

void MainWidget::clearConsole()
{
    m_console->clear();
}

void MainWidget::enableTiming(bool b)
{
    m_interpreter->enableTiming(b);
}

void MainWidget::enableColors(bool b)
{
    m_console->enableColors(b);
}

void MainWidget::printPerfTimers()
{
    m_console->printPerfTimers();
}

void MainWidget::setParseVar(unsigned v)
{
    m_console->setParseVar(v);
}

void MainWidget::display(const QPixmap &px)
{
    QLabel* label=new QLabel(this);
    label->setWindowFlags(Qt::Window);
    label->setPixmap(px);
    label->show();
}

void MainWidget::closeEvent(QCloseEvent *e)
{
    QSettings s;
    s.setValue("geometry",saveGeometry());
    s.setValue("mainWindowState",saveState());
    QMainWindow::closeEvent(e);
    QApplication::closeAllWindows();
}

