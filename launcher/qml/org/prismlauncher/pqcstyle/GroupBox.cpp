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

#include "GroupBox.h"
#include "PQuickStyleItem.h"

#include <QApplication>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionGroupBox>

PStyleGroupBox::PStyleGroupBox(QQuickItem* parent) : PQuickStyleItem(parent)
{
    m_type = QStringLiteral("groupbox");
}

void PStyleGroupBox::doInitStyleOption()
{
    if (!m_styleoption) {
        m_styleoption = new QStyleOptionGroupBox();
    }

    QStyleOptionGroupBox* opt = qstyleoption_cast<QStyleOptionGroupBox*>(m_styleoption);
    opt->text = text();
    opt->lineWidth = PQuickStyleItem::style()->pixelMetric(QStyle::PM_DefaultFrameWidth, m_styleoption, nullptr);
    opt->subControls = QStyle::SC_GroupBoxLabel;
    opt->features = {};
    if (m_properties[QStringLiteral("sunken")].toBool()) {  // Qt draws an ugly line here so I ignore it
        opt->subControls |= QStyle::SC_GroupBoxFrame;
    } else {
        opt->features |= QStyleOptionFrame::Flat;
    }
    if (m_properties[QStringLiteral("checkable")].toBool()) {
        opt->subControls |= QStyle::SC_GroupBoxCheckBox;
    }
}

QSize PStyleGroupBox::getContentSize(int, int)
{
    QSize size;
    QStyleOptionGroupBox* box = qstyleoption_cast<QStyleOptionGroupBox*>(m_styleoption);
    QFontMetrics metrics(box->fontMetrics);
    int baseWidth = metrics.boundingRect(box->text + QLatin1Char(' ')).width();
    int baseHeight = metrics.height() + m_contentHeight;
    if (box->subControls & QStyle::SC_GroupBoxCheckBox) {
        baseWidth += PQuickStyleItem::style()->pixelMetric(QStyle::PM_IndicatorWidth);
        baseWidth += PQuickStyleItem::style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing);
        baseHeight = qMax(baseHeight, PQuickStyleItem::style()->pixelMetric(QStyle::PM_IndicatorHeight));
    }
    size =
        PQuickStyleItem::style()->sizeFromContents(QStyle::CT_GroupBox, m_styleoption, QSize(qMax(baseWidth, m_contentWidth), baseHeight));
    return size;
}

void PStyleGroupBox::doPaint(QPainter* painter)
{
    PQuickStyleItem::style()->drawComplexControl(QStyle::CC_GroupBox, qstyleoption_cast<QStyleOptionComplex*>(m_styleoption), painter);
}
