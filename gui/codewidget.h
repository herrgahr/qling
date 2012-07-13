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

#ifndef CODEWIDGET_H
#define CODEWIDGET_H

#include <QWidget>

class CodeInput;
class QTextEdit;

class Qling;

/** Widget that contains a line-edit for entering code
  * and a TextEdit to display code entered so far
  *
  * This class also handles the processing of the code, i.e. it has a
  * cling::MetaProcessor member that is used to pass code to the interpreter
  * This might not be the cleanest design...
  *
  */
class CodeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CodeWidget(Qling *qling);

public slots:
    /** pass str to the interpreter*/
    void processCode(const QString& str);

private:
    Qling* m_qling;
    CodeInput* m_codeInput;
    QTextEdit* m_codeOutput;

};

#endif // CODEWIDGET_H
