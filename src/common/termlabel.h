/* ****************************************************************************
  This file is part of Lokalize

  Copyright (C) 2007 by Nick Shaforostoff <shafff@ukr.net>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License or (at your option) version 3 or any later version
  accepted by the membership of KDE e.V. (or its successor approved
  by the membership of KDE e.V.), which shall act as a proxy 
  defined in Section 14 of version 3 of the license.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

**************************************************************************** */

#ifndef TERMLABEL_H
#define TERMLABEL_H

#include <QLabel>
#include <kaction.h>
#include "glossary.h"
#include "project.h"

namespace GlossaryNS {
/**
 * flowlayout item
 */
class TermLabel: public QLabel//QPushButton
{
    Q_OBJECT
public:
    TermLabel(QAction* a=0): m_termIndex(-1),m_action(a){};
    ~TermLabel(){}

    /**
     * @param term is the term matched
     * @param entry is a whole entry
     */
    void setText(const QString& term, int entry, bool capFirst);
    void mousePressEvent (QMouseEvent* /* event*/);

public slots:
    void insert();
//     bool event(QEvent *event);
signals:
    void insertTerm(const QString&);

private:
    int m_termIndex;
    bool m_capFirst;
    QAction* m_action; //used only for shortcut purposes
};






}
#endif
