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

#include "Frame.h"
#include "PQuickStyleItem.h"

#include <QApplication>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionFrame>

PStyleFrame::PStyleFrame(QQuickItem* parent) : PQuickStyleItem(parent)
{
    m_type = QStringLiteral("frame");
}

void PStyleFrame::doInitStyleOption()
{
    if (!m_styleoption) {
        m_styleoption = new QStyleOptionFrame();
    }

    QStyleOptionFrame* opt = qstyleoption_cast<QStyleOptionFrame*>(m_styleoption);
    opt->frameShape = QFrame::StyledPanel;
    opt->lineWidth = PQuickStyleItem::style()->pixelMetric(QStyle::PM_DefaultFrameWidth, m_styleoption, nullptr);
    opt->midLineWidth = opt->lineWidth;
}

void PStyleFrame::doPaint(QPainter* painter)
{
    m_styleoption->state |= QStyle::State_Sunken;
    m_styleoption->state &= ~QStyle::State_Raised;
    PQuickStyleItem::style()->drawControl(QStyle::CE_ShapedFrame, m_styleoption, painter);
}
