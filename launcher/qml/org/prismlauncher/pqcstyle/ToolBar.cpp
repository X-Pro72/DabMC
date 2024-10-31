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
 *  This file incorporates work covered by the following copyright and
 *  permission notice:
 *
 *    Copyright © 2017 Marco Martin <mart@kde.org>
 *    Copyright © 2017 David Edmundson <davidedmundson@kde.org>
 *    Copyright © 2019 David Redondo <david@david-redondo.de>
 *    Copyright © 2023 ivan tkachenko <me@ratijas.tk>
 *
 *    Licensed under LGPL-3.0-only OR GPL-2.0-only OR
 *    GPL-3.0-only OR LicenseRef-KFQF-Accepted-GPL OR
 *    LicenseRef-Qt-Commercial
 *
 *          https://community.kde.org/Policies/Licensing_Policy
 */

#include "ToolBar.h"
#include "PQuickStyleItem.h"

#include <QApplication>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionToolBar>

PStyleToolBar::PStyleToolBar(QQuickItem* parent) : PQuickStyleItem(parent)
{
    m_type = QStringLiteral("toolbar");
}

void PStyleToolBar::doInitStyleOption()
{
    if (!m_styleoption) {
        m_styleoption = new QStyleOptionToolBar();
    }
}

QSize PStyleToolBar::getContentSize(int, int)
{
    return QSize(200, styleName().contains(QLatin1String("windows")) ? 30 : 42);
}

void PStyleToolBar::doPaint(QPainter* painter)
{
    painter->fillRect(m_styleoption->rect, m_styleoption->palette.window().color());
    PQuickStyleItem::style()->drawControl(QStyle::CE_ToolBar, m_styleoption, painter);
    painter->save();
    painter->setPen(styleName() != QLatin1String("fusion") ? m_styleoption->palette.dark().color().darker(120)
                                                           : m_styleoption->palette.window().color().lighter(107));
    painter->drawLine(m_styleoption->rect.bottomLeft(), m_styleoption->rect.bottomRight());
    painter->restore();
}
