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

#include "ItemRow.h"
#include "PQuickStyleItem.h"

#include <QApplication>
#include <QPainter>
#include <QPixmapCache>
#include <QStyle>
#include <QStyleOptionViewItem>
#include <QWidget>

PStyleItemRow::PStyleItemRow(QQuickItem* parent) : PQuickStyleItem(parent)
{
    m_type = QStringLiteral("itemrow");
}

void PStyleItemRow::doInitStyleOption()
{
    if (!m_styleoption) {
        m_styleoption = new QStyleOptionViewItem();
    }

    QStyleOptionViewItem* opt = qstyleoption_cast<QStyleOptionViewItem*>(m_styleoption);
    opt->features = {};
    if (activeControl() == QLatin1String("alternate")) {
        opt->features |= QStyleOptionViewItem::Alternate;
    }
}

QSize PStyleItemRow::getContentSize(int width, int height)
{
    return PQuickStyleItem::style()->sizeFromContents(QStyle::CT_ItemViewItem, m_styleoption, QSize(width, height));
}

void PStyleItemRow::doPaint(QPainter* painter)
{
    QPixmap pixmap;
    // Only draw through style once
    const QString pmKey = QLatin1String("itemrow") % QString::number(m_styleoption->state, 16) % activeControl();
    if (!QPixmapCache::find(pmKey, &pixmap) || pixmap.width() < width() || height() != pixmap.height()) {
        int newSize = width();
        pixmap = QPixmap(newSize, height());
        pixmap.fill(Qt::transparent);
        QPainter pixpainter(&pixmap);
        PQuickStyleItem::style()->drawPrimitive(QStyle::PE_PanelItemViewRow, m_styleoption, &pixpainter);
        if ((styleName() == QLatin1String("mac") || !PQuickStyleItem::style()->styleHint(QStyle::SH_ItemView_ShowDecorationSelected)) &&
            selected()) {
            QPalette pal = QApplication::palette("QAbstractItemView");
            pal.setCurrentColorGroup(m_styleoption->palette.currentColorGroup());
            pixpainter.fillRect(m_styleoption->rect, pal.highlight());
        }
        QPixmapCache::insert(pmKey, pixmap);
    }
    painter->drawPixmap(0, 0, pixmap);
}
