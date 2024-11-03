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

#include "ComboBoxItem.h"
#include "PQuickStyleItem.h"

#include <QApplication>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionMenuItem>

PStyleComboBoxItem::PStyleComboBoxItem(QQuickItem* parent) : PQuickStyleItem(parent)
{
    m_type = QStringLiteral("comboboxitem");
}

void PStyleComboBoxItem::doInitStyleOption()
{
    if (!m_styleoption) {
        m_styleoption = new QStyleOptionMenuItem();
    }

    QStyleOptionMenuItem* opt = qstyleoption_cast<QStyleOptionMenuItem*>(m_styleoption);
    // For GTK style. See below, in setElementType()
    setProperty("_q_isMenuPopupItem", true);

    PQuickStyleItem::MenuItemType type = static_cast<PQuickStyleItem::MenuItemType>(m_properties[QStringLiteral("type")].toInt());
    if (type == PQuickStyleItem::ScrollIndicatorType) {
        int scrollerDirection = m_properties[QStringLiteral("scrollerDirection")].toInt();
        opt->menuItemType = QStyleOptionMenuItem::Scroller;
        opt->state |= scrollerDirection == Qt::UpArrow ? QStyle::State_UpArrow : QStyle::State_DownArrow;
    } else if (type == PQuickStyleItem::SeparatorType) {
        opt->menuItemType = QStyleOptionMenuItem::Separator;
    } else {
        opt->text = text();

        if (type == PQuickStyleItem::MenuType) {
            opt->menuItemType = QStyleOptionMenuItem::SubMenu;
        } else {
            opt->menuItemType = QStyleOptionMenuItem::Normal;

            QString shortcut = m_properties[QStringLiteral("shortcut")].toString();
            if (!shortcut.isEmpty()) {
                opt->text += QLatin1Char('\t') + shortcut;
                opt->reservedShortcutWidth = qMax(opt->reservedShortcutWidth, qRound(textWidth(shortcut)));
            }

            if (m_properties[QStringLiteral("checkable")].toBool()) {
                opt->checked = on();
                QVariant exclusive = m_properties[QStringLiteral("exclusive")];
                opt->checkType = exclusive.toBool() ? QStyleOptionMenuItem::Exclusive : QStyleOptionMenuItem::NonExclusive;
            }
        }

        if (m_iconDirty || opt->icon.isNull()) {
            opt->icon = iconFromIconProperty();
            m_iconDirty = false;
        }

        setProperty("_q_showUnderlined", m_hints[QStringLiteral("showUnderlined")].toBool());

        const QFont font = qApp->font("QComboMenuItem");
        opt->font = font;
        opt->fontMetrics = QFontMetrics(font);
        m_font = opt->font;
    }
}

QSize PStyleComboBoxItem::getContentSize(int width, int height)
{
    QSize size;
    if (static_cast<QStyleOptionMenuItem*>(m_styleoption)->menuItemType == QStyleOptionMenuItem::Scroller) {
        size.setHeight(PQuickStyleItem::style()->pixelMetric(QStyle::PM_MenuScrollerHeight, nullptr, nullptr));
    } else {
        size = PQuickStyleItem::style()->sizeFromContents(QStyle::CT_MenuItem, m_styleoption, QSize(width, height));
    }
    return size;
}

void PStyleComboBoxItem::doPaint(QPainter* painter)
{
    QStyle::ControlElement menuElement = static_cast<QStyleOptionMenuItem*>(m_styleoption)->menuItemType == QStyleOptionMenuItem::Scroller
                                             ? QStyle::CE_MenuScroller
                                             : QStyle::CE_MenuItem;
    PQuickStyleItem::style()->drawControl(menuElement, m_styleoption, painter);
}
