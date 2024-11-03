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

#include "Tab.h"
#include "PQuickStyleItem.h"

#include <QApplication>
#include <QStyle>
#include <QStyleOptionTab>

PStyleTab::PStyleTab(QQuickItem* parent) : PQuickStyleItem(parent)
{
    m_type = QStringLiteral("tab");
}

void PStyleTab::doInitStyleOption()
{
    if (!m_styleoption) {
        m_styleoption = new QStyleOptionTab();
    }

    QStyleOptionTab* opt = qstyleoption_cast<QStyleOptionTab*>(m_styleoption);
    opt->text = text();

    if (m_iconDirty || opt->icon.isNull()) {
        opt->icon = iconFromIconProperty();
        m_iconDirty = false;
    }

    auto iconSize = QSize(m_properties[QStringLiteral("iconWidth")].toInt(), m_properties[QStringLiteral("iconHeight")].toInt());
    if (iconSize.isEmpty()) {
        int e = PQuickStyleItem::style()->pixelMetric(QStyle::PM_TabBarIconSize, m_styleoption, nullptr);
        if (iconSize.width() <= 0) {
            iconSize.setWidth(e);
        }
        if (iconSize.height() <= 0) {
            iconSize.setHeight(e);
        }
    }
    opt->iconSize = iconSize;

    if (m_properties.value(QStringLiteral("hasFrame")).toBool()) {
        opt->features |= QStyleOptionTab::HasFrame;
    }

    QString orientation = m_properties.value(QStringLiteral("orientation")).toString();
    QString position = m_properties.value(QStringLiteral("tabpos")).toString();
    QString selectedPosition = m_properties.value(QStringLiteral("selectedpos")).toString();

    opt->shape = orientation == QLatin1String("Bottom") ? QTabBar::RoundedSouth : QTabBar::RoundedNorth;
    if (position == QLatin1String("beginning")) {
        opt->position = QStyleOptionTab::Beginning;
    } else if (position == QLatin1String("end")) {
        opt->position = QStyleOptionTab::End;
    } else if (position == QLatin1String("only")) {
        opt->position = QStyleOptionTab::OnlyOneTab;
    } else {
        opt->position = QStyleOptionTab::Middle;
    }

    if (selectedPosition == QLatin1String("next")) {
        opt->selectedPosition = QStyleOptionTab::NextIsSelected;
    } else if (selectedPosition == QLatin1String("previous")) {
        opt->selectedPosition = QStyleOptionTab::PreviousIsSelected;
    } else {
        opt->selectedPosition = QStyleOptionTab::NotAdjacent;
    }
}

QSize PStyleTab::getContentSize(int width, int height)
{
    QStyleOptionTab* tab = qstyleoption_cast<QStyleOptionTab*>(m_styleoption);

    int contentWidth = tab->fontMetrics.boundingRect(tab->text).width();
    int contentHeight = tab->fontMetrics.height();

    if (!tab->icon.isNull()) {
        //+4 matches a hardcoded value in QStyle and acts as a margin between the icon and the text.
        contentWidth += tab->iconSize.width() + 4;
        contentHeight = qMax(contentHeight, tab->iconSize.height());
    }

    contentWidth += PQuickStyleItem::style()->pixelMetric(QStyle::PM_TabBarTabHSpace, tab);
    contentHeight += PQuickStyleItem::style()->pixelMetric(QStyle::PM_TabBarTabVSpace, tab);

    const int newWidth = qMax(width, contentWidth);
    const int newHeight = qMax(height, contentHeight);

    return PQuickStyleItem::style()->sizeFromContents(QStyle::CT_TabBarTab, m_styleoption, QSize(newWidth, newHeight));
}

static bool isKeyFocusReason(Qt::FocusReason reason)
{
    return reason == Qt::TabFocusReason || reason == Qt::BacktabFocusReason || reason == Qt::ShortcutFocusReason;
}

void PStyleTab::doPaint(QPainter* painter)
{
    if (!isKeyFocusReason(m_focusReason)) {
        m_styleoption->state &= ~QStyle::State_HasFocus;
    }
    PQuickStyleItem::style()->drawControl(QStyle::CE_TabBarTab, m_styleoption, painter);
}
