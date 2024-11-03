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

#include "Edit.h"
#include "PQuickStyleItem.h"

#include <QApplication>
#include <QStyle>
#include <QStyleOptionFrame>

PStyleEdit::PStyleEdit(QQuickItem* parent) : PQuickStyleItem(parent)
{
    m_type = QStringLiteral("edit");
}

void PStyleEdit::doInitStyleOption()
{
    if (!m_styleoption) {
        m_styleoption = new QStyleOptionFrame();
    }

    QStyleOptionFrame* opt = qstyleoption_cast<QStyleOptionFrame*>(m_styleoption);
    opt->lineWidth =
        qMax(1, PQuickStyleItem::style()->pixelMetric(QStyle::PM_DefaultFrameWidth, m_styleoption, nullptr));  // this must be non zero
}

QSize PStyleEdit::getContentSize(int width, int height)
{
    // We have to create a new style option since we might be calling with a QStyleOptionSpinBox
    QStyleOptionFrame frame;
    //+2 to be consistent with the hardcoded verticalmargin in QLineEdit
    int contentHeight = frame.fontMetrics.height() + 2;

    frame.state = m_styleoption->state | QStyle::State_Sunken;
    frame.lineWidth = PQuickStyleItem::style()->pixelMetric(QStyle::PM_DefaultFrameWidth, m_styleoption, nullptr);
    frame.rect = m_styleoption->rect;
    frame.styleObject = this;

    return PQuickStyleItem::style()->sizeFromContents(QStyle::CT_LineEdit, &frame, QSize(width, qMax(height, contentHeight)));
}

qreal PStyleEdit::baselineOffset() const
{
    QRect r = PQuickStyleItem::style()->subElementRect(QStyle::SE_LineEditContents, m_styleoption);
    return baselineOffsetFromRect(r);
}
void PStyleEdit::doPaint(QPainter* painter)
{
    PQuickStyleItem::style()->drawPrimitive(QStyle::PE_PanelLineEdit, m_styleoption, painter);
}
