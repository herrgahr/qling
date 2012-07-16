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

#include "widget.h"

#include "consoleoutput.h"
#include "codewidget.h"

#include "qling/qling.h"


MainWidget::MainWidget(Qling* qling)
    :QMainWindow()
    ,m_console(new ConsoleOutput)
    ,m_consoleDock(new QDockWidget("stdout && stderr"))
    ,m_codeWidget(new CodeWidget(qling))
{

    setCentralWidget(m_codeWidget);
    m_codeWidget->setObjectName("codeWidget");

    m_consoleDock->setWidget(m_console);
    m_consoleDock->setObjectName("consoleDock");
    addDockWidget(Qt::BottomDockWidgetArea,m_consoleDock);

    connect(qling,SIGNAL(aboutToExec()),
            m_console,SLOT(enterAppMode()));
    connect(qling,SIGNAL(aboutToProcess()),
            m_console,SLOT(enterCompileMode()));

    QSettings s;
    restoreGeometry(s.value("geometry").toByteArray());
    restoreState(s.value("mainWindowState").toByteArray());
}

void MainWidget::writeToConsole(const char *txt)
{
    writeToConsole(QString(txt));
}

void MainWidget::writeToConsole(const QString& txt)
{
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

void MainWidget::closeEvent(QCloseEvent *e)
{
    QSettings s;
    s.setValue("geometry",saveGeometry());
    s.setValue("mainWindowState",saveState());
    QMainWindow::closeEvent(e);
    QApplication::closeAllWindows();
}

