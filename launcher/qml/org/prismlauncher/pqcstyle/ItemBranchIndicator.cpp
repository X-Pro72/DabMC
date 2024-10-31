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

#include "ItemBranchIndicator.h"
#include "PQuickStyleItem.h"

#include <QApplication>
#include <QStyle>
#include <QStyleOption>

PStyleItemBranchIndicator::PStyleItemBranchIndicator(QQuickItem* parent) : PQuickStyleItem(parent)
{
    m_type = QStringLiteral("itembranchindicator");
}

void PStyleItemBranchIndicator::doInitStyleOption()
{
    if (!m_styleoption) {
        m_styleoption = new QStyleOption;
    }

    if (m_properties.value(QStringLiteral("isItem")).toBool()) {
        m_styleoption->state = QStyle::State_Item;
    }
    if (m_properties.value(QStringLiteral("hasChildren")).toBool()) {
        m_styleoption->state |= QStyle::State_Children;
    }
    if (m_properties.value(QStringLiteral("hasSibling")).toBool()) {  // Even this one could go away
        m_styleoption->state |= QStyle::State_Sibling;
    }
    if (m_on) {
        m_styleoption->state |= QStyle::State_Open;
    }
}
QRectF PStyleItemBranchIndicator::subControlRect(const QString&)
{
    initStyleOption();
    QStyleOption opt;
    opt.rect = QRect(0, 0, implicitWidth(), implicitHeight());
    return PQuickStyleItem::style()->subElementRect(QStyle::SE_TreeViewDisclosureItem, &opt, nullptr);
}

void PStyleItemBranchIndicator::doPaint(QPainter* painter)
{
    PQuickStyleItem::style()->drawPrimitive(QStyle::PE_IndicatorBranch, m_styleoption, painter);
}
