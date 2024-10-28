
// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2024 Rachel Powers <508861+Ryex@users.noreply.github.com>
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2024 Rachel Powers <508861+Ryex@users.noreply.github.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <QObject>

#include <qqmlregistration.h>
#include <qtmetamacros.h>

class PQuickPadding : public QObject {
    Q_OBJECT
    QML_ANONYMOUS

    Q_PROPERTY(int left READ left WRITE setLeft NOTIFY leftChanged)
    Q_PROPERTY(int top READ top WRITE setTop NOTIFY topChanged)
    Q_PROPERTY(int right READ right WRITE setRight NOTIFY rightChanged)
    Q_PROPERTY(int bottom READ bottom WRITE setBottom NOTIFY bottomChanged)

    int m_left;
    int m_top;
    int m_right;
    int m_bottom;

   public:
    PQuickPadding(QObject* parent = nullptr) : QObject(parent), m_left(0), m_top(0), m_right(0), m_bottom(0) {}

    int left() const { return m_left; }
    int top() const { return m_top; }
    int right() const { return m_right; }
    int bottom() const { return m_bottom; }

   public slots:
    void setLeft(int arg)
    {
        if (m_left != arg) {
            m_left = arg;
            Q_EMIT leftChanged();
        }
    }
    void setTop(int arg)
    {
        if (m_top != arg) {
            m_top = arg;
            Q_EMIT topChanged();
        }
    }
    void setRight(int arg)
    {
        if (m_right != arg) {
            m_right = arg;
            Q_EMIT rightChanged();
        }
    }
    void setBottom(int arg)
    {
        if (m_bottom != arg) {
            m_bottom = arg;
            Q_EMIT bottomChanged();
        }
    }

   signals:
    void leftChanged();
    void topChanged();
    void rightChanged();
    void bottomChanged();
};
