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

#include "Header.h"
#include "PQuickStyleItem.h"

#include <QApplication>
#include <QStyle>
#include <QStyleOptionHeader>

PStyleHeader::PStyleHeader(QQuickItem* parent) : PQuickStyleItem(parent)
{
    m_type = QStringLiteral("header");
}

void PStyleHeader::doInitStyleOption()
{
    if (!m_styleoption) {
        m_styleoption = new QStyleOptionHeader();
    }

    QStyleOptionHeader* opt = qstyleoption_cast<QStyleOptionHeader*>(m_styleoption);
    opt->text = text();
    opt->textAlignment = static_cast<Qt::AlignmentFlag>(m_properties.value(QStringLiteral("textalignment")).toInt());
    opt->sortIndicator = activeControl() == QLatin1String("down") ? QStyleOptionHeader::SortDown
                         : activeControl() == QLatin1String("up") ? QStyleOptionHeader::SortUp
                                                                  : QStyleOptionHeader::None;
    QString headerpos = m_properties.value(QStringLiteral("headerpos")).toString();
    if (headerpos == QLatin1String("beginning")) {
        opt->position = QStyleOptionHeader::Beginning;
    } else if (headerpos == QLatin1String("end")) {
        opt->position = QStyleOptionHeader::End;
    } else if (headerpos == QLatin1String("only")) {
        opt->position = QStyleOptionHeader::OnlyOneSection;
    } else {
        opt->position = QStyleOptionHeader::Middle;
    }
    opt->orientation = m_properties.value(QStringLiteral("orientation")).value<Qt::Orientation>();
    if (opt->orientation == Qt::Orientation(0)) {
        opt->orientation = Qt::Horizontal;
    }
    const QFont font = qApp->font("QHeaderView");
    opt->fontMetrics = QFontMetrics(font);
}

QSize PStyleHeader::getContentSize(int width, int height)
{
    return PQuickStyleItem::style()->sizeFromContents(QStyle::CT_HeaderSection, m_styleoption, QSize(width, height));
}

void PStyleHeader::doPaint(QPainter* painter)
{
    PQuickStyleItem::style()->drawControl(QStyle::CE_Header, m_styleoption, painter);
}
