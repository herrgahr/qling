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

#ifndef CODEINPUT_H
#define CODEINPUT_H

#include <QTextEdit>
#include <QStringList>

/** LineEdit that keeps a history of stuff entered
  *
  * history is saved via the QSettings interface so use
  * QApplication::setApplicationName and QApplication::setOrganizationName
  * to uniquely identify the settings
  */
class CodeInput : public QTextEdit
{
    Q_OBJECT
public:
    explicit CodeInput(QWidget *parent = 0);
    ~CodeInput();

    bool eventFilter(QObject *, QEvent *);

    /** set maximum number of entries in history.
      *
      * Default: 50
      * Note: The current history - if longer than s - will not be truncated
      * until the next entry is added
      */
    void setMaxHistorySize(int s);
    int maxHistorySize()const;
    QString text()const;
    bool isEmpty()const;

    void fitSizeToText();
    //void scrollToBottom();

protected:
    void showEvent(QShowEvent *);

signals:
    void entered(const QString& str);

public slots:
    void enableMultiLineMode(bool e);

private:
    void submit();
    void addToHistory(const QString& str);
    void moveInHistory(int dir);
    QStringList m_history;
    int m_historyPos;
    int m_maxHistorySize;

    //used in eventFilter to check if text has changed
    QString m_oldText;
    //cache to save text that has neither been submitted nor committed to
    //history
    QString m_unsubmittedText;
    int m_yMargin;
    bool m_multiLineEnabled;
};

#endif // CODEINPUT_H
