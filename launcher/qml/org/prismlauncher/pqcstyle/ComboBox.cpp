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

#include "ComboBox.h"
#include "PQuickStyleItem.h"

#include <QApplication>
#include <QStyle>
#include <QStyleOptionComboBox>
#include <QPainter>

PStyleComboBox::PStyleComboBox(QQuickItem* parent) : PQuickStyleItem(parent)
{
    m_type = QStringLiteral("combobox");
}

void PStyleComboBox::doInitStyleOption()
{
    if (!m_styleoption) {
        m_styleoption = new QStyleOptionComboBox();
    }

    QStyleOptionComboBox* opt = qstyleoption_cast<QStyleOptionComboBox*>(m_styleoption);

    const QFont font = qApp->font("QComboBox");
    opt->fontMetrics = QFontMetrics(font);
    opt->currentText = text();
    opt->editable = m_properties[QStringLiteral("editable")].toBool();

    const QVariant icon = m_properties[QStringLiteral("currentIcon")];
    if (icon.canConvert<QIcon>()) {
        opt->currentIcon = icon.value<QIcon>();
    } else if (icon.canConvert<QString>()) {
        opt->currentIcon = iconFromIconProperty(icon.value<QString>(), m_properties[QStringLiteral("iconColor")].value<QColor>());
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
    opt->frame = !m_flat;
}

QSize PStyleComboBox::getContentSize(int width, int height)
{
    QStyleOptionComboBox* opt = qstyleoption_cast<QStyleOptionComboBox*>(m_styleoption);
    int newWidth = qMax(width, m_contentWidth) + (opt->currentIcon.isNull() ? 0 : opt->iconSize.width());
    int newHeight = qMax(height, opt->fontMetrics.height());
    return PQuickStyleItem::style()->sizeFromContents(QStyle::CT_ComboBox, m_styleoption, QSize(newWidth, newHeight));
}

qreal PStyleComboBox::baselineOffset() const
{
    QRect r;
    if (const auto option = qstyleoption_cast<const QStyleOptionComboBox*>(m_styleoption)) {
        r = PQuickStyleItem::style()->subControlRect(QStyle::CC_ComboBox, option, QStyle::SC_ComboBoxEditField);
        if (styleName() != QLatin1String("mac")) {
            r.adjust(0, 0, 0, 1);
        }
    }
    return baselineOffsetFromRect(r);
}
void PStyleComboBox::doPaint(QPainter* painter)
{
    PQuickStyleItem::style()->drawComplexControl(QStyle::CC_ComboBox, qstyleoption_cast<QStyleOptionComplex*>(m_styleoption), painter);
    // This is needed on macOS as it will use the painter color and ignore the palette
    QPen pen = painter->pen();
    painter->setPen(m_styleoption->palette.text().color());
    PQuickStyleItem::style()->drawControl(QStyle::CE_ComboBoxLabel, m_styleoption, painter);
    painter->setPen(pen);
}
