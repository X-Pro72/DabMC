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

#include "RadioButton.h"
#include "PQuickStyleItem.h"

#include <QApplication>
#include <QStyle>
#include <QStyleOptionButton>

PStyleRadioButton::PStyleRadioButton(QQuickItem* parent) : PQuickStyleItem(parent)
{
    m_type = QStringLiteral("radiobutton");
}

void PStyleRadioButton::doInitStyleOption()
{
    if (!m_styleoption) {
        m_styleoption = new QStyleOptionButton();
    }

    QStyleOptionButton* opt = qstyleoption_cast<QStyleOptionButton*>(m_styleoption);
    if (!on()) {
        opt->state |= QStyle::State_Off;
    }
    if (m_properties.value(QStringLiteral("partiallyChecked")).toBool()) {
        opt->state |= QStyle::State_NoChange;
    }
    opt->text = text();

    if (m_iconDirty || opt->icon.isNull()) {
        opt->icon = iconFromIconProperty();
        m_iconDirty = false;
    }

    auto iconSize = QSize(m_properties[QStringLiteral("iconWidth")].toInt(), m_properties[QStringLiteral("iconHeight")].toInt());
    if (iconSize.isEmpty()) {
        int e = PQuickStyleItem::style()->pixelMetric(QStyle::PM_ButtonIconSize, m_styleoption, nullptr);
        if (iconSize.width() <= 0) {
            iconSize.setWidth(e);
        }
        if (iconSize.height() <= 0) {
            iconSize.setHeight(e);
        }
    }
    opt->iconSize = iconSize;
}

QSize PStyleRadioButton::getContentSize(int, int)
{
    auto contentType = QStyle::CT_RadioButton;
    QStyleOptionButton* btn = qstyleoption_cast<QStyleOptionButton*>(m_styleoption);
    QSize contentSize = btn->fontMetrics.size(Qt::TextShowMnemonic, btn->text);
    if (!btn->icon.isNull()) {
        contentSize.setWidth(contentSize.width() + btn->iconSize.width());
        contentSize.setHeight(std::max(contentSize.height(), btn->iconSize.height()));
    }
    return PQuickStyleItem::style()->sizeFromContents(contentType, m_styleoption, contentSize);
}

qreal PStyleRadioButton::baselineOffset() const
{
    QRect r = PQuickStyleItem::style()->subElementRect(QStyle::SE_RadioButtonContents, m_styleoption);
    return baselineOffsetFromRect(r);
}
void PStyleRadioButton::doPaint(QPainter* painter)
{
    PQuickStyleItem::style()->drawControl(QStyle::CE_RadioButton, m_styleoption, painter);
}
