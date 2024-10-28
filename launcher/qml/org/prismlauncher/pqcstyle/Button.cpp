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

#include "Button.h"
#include "PQuickStyleItem.h"

#include <QApplication>
#include <QPushButton>
#include <QStyle>
#include <QStyleOptionButton>

PStyleButton::PStyleButton(QQuickItem* parent) : PQuickStyleItem(parent)
{
    m_type = QStringLiteral("button");
}

void PStyleButton::doResolvePalette()
{
    m_buttonPalette = QPushButton().palette();
}

QPalette PStyleButton::getResolvedPalette()
{
    return m_buttonPalette;
}

void PStyleButton::doInitStyleOption()
{
    if (!m_styleoption) {
        m_styleoption = new QStyleOptionButton();
    }

    QStyleOptionButton* opt = qstyleoption_cast<QStyleOptionButton*>(m_styleoption);
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
    opt->features = activeControl() == QLatin1String("default") ? QStyleOptionButton::DefaultButton : QStyleOptionButton::None;
    if (m_flat) {
        opt->features |= QStyleOptionButton::Flat;
    }
    const QFont font = qApp->font("QPushButton");
    opt->fontMetrics = QFontMetrics(font);
    bool hasMenu = m_properties[QStringLiteral("menu")].toBool();
    if (hasMenu) {
        opt->features |= QStyleOptionButton::HasMenu;
    }
    updateStyleOption();
}

QSize PStyleButton::getContentSize(int width, int height)
{
    QStyleOptionButton* btn = qstyleoption_cast<QStyleOptionButton*>(m_styleoption);

    int contentWidth = btn->fontMetrics.boundingRect(btn->text).width();
    int contentHeight = btn->fontMetrics.height();

    if (!btn->icon.isNull()) {
        //+4 matches a hardcoded value in QStyle and acts as a margin between the icon and the text.
        contentWidth += btn->iconSize.width() + 4;
        contentHeight = qMax(btn->fontMetrics.height(), btn->iconSize.height());
    }

    int newWidth = qMax(width, contentWidth);
    int newHeight = qMax(height, contentHeight);
    return PQuickStyleItem::style()->sizeFromContents(QStyle::CT_PushButton, m_styleoption, QSize(newWidth, newHeight));
}

qreal PStyleButton::baselineOffset() const
{
    QRect r = PQuickStyleItem::style()->subElementRect(QStyle::SE_PushButtonContents, m_styleoption);
    return baselineOffsetFromRect(r);
}
void PStyleButton::doPaint(QPainter* painter)
{
    PQuickStyleItem::style()->drawControl(QStyle::CE_PushButton, m_styleoption, painter);
}
