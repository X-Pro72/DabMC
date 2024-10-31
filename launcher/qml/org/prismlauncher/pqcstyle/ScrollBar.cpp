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

#include "ScrollBar.h"
#include "PQuickStyleItem.h"

#include <QApplication>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionSlider>

PStyleScrollBar::PStyleScrollBar(QQuickItem* parent) : PQuickStyleItem(parent)
{
    m_type = QStringLiteral("scrollbar");
}

void PStyleScrollBar::doInitStyleOption()
{
    if (!m_styleoption) {
        m_styleoption = new QStyleOptionSlider();
    }

    QStyleOptionSlider* opt = qstyleoption_cast<QStyleOptionSlider*>(m_styleoption);
    opt->minimum = qMax(0, minimum());
    opt->maximum = qMax(0, maximum());
    opt->pageStep = qMax(0, int(horizontal() ? width() : height()));
    opt->orientation = horizontal() ? Qt::Horizontal : Qt::Vertical;
    if (horizontal()) {
        // mirrored horizontal scrollbars are not something you want to interact with
        m_preventMirroring = true;
    }
    opt->sliderPosition = value();
    opt->sliderValue = value();
    opt->activeSubControls = (activeControl() == QLatin1String("up"))       ? QStyle::SC_ScrollBarSubLine
                             : (activeControl() == QLatin1String("down"))   ? QStyle::SC_ScrollBarAddLine
                             : (activeControl() == QLatin1String("handle")) ? QStyle::SC_ScrollBarSlider
                             : hover()                                      ? QStyle::SC_ScrollBarGroove
                                                                            : QStyle::SC_None;
    if (raised()) {
        opt->state |= QStyle::State_On;
    }

    opt->subControls = QStyle::SC_All;

    setTransient(PQuickStyleItem::style()->styleHint(QStyle::SH_ScrollBar_Transient, m_styleoption));
}

QSize PStyleScrollBar::getContentSize(int width, int height)
{
    QSize size;
    const auto opt = qstyleoption_cast<const QStyleOptionSlider*>(m_styleoption);

    // Copied from QScrollBar
    const int scrollBarExtent = PQuickStyleItem::style()->pixelMetric(QStyle::PM_ScrollBarExtent, opt, nullptr);
    const int scrollBarSliderMin = PQuickStyleItem::style()->pixelMetric(QStyle::PM_ScrollBarSliderMin, opt, nullptr);
    if (opt->orientation == Qt::Horizontal) {
        size = QSize(scrollBarExtent * 2 + scrollBarSliderMin, scrollBarExtent);
    } else {
        size = QSize(scrollBarExtent, scrollBarExtent * 2 + scrollBarSliderMin);
    }
    size = PQuickStyleItem::style()->sizeFromContents(QStyle::CT_ScrollBar, opt, size);
    return size;
}

QRectF PStyleScrollBar::subControlRect(const QString& subcontrolString)
{
    QStyle::SubControl subcontrol = QStyle::SC_None;
    initStyleOption();
    QStyle::ComplexControl control = QStyle::CC_ScrollBar;
    if (subcontrolString == QLatin1String("slider")) {
        subcontrol = QStyle::SC_ScrollBarSlider;
    } else if (subcontrolString == QLatin1String("groove")) {
        subcontrol = QStyle::SC_ScrollBarGroove;
    } else if (subcontrolString == QLatin1String("handle")) {
        subcontrol = QStyle::SC_ScrollBarSlider;
    } else if (subcontrolString == QLatin1String("add")) {
        subcontrol = QStyle::SC_ScrollBarAddPage;
    } else if (subcontrolString == QLatin1String("sub")) {
        subcontrol = QStyle::SC_ScrollBarSubPage;
    }
    return PQuickStyleItem::style()->subControlRect(control, qstyleoption_cast<QStyleOptionComplex*>(m_styleoption), subcontrol);
}

void PStyleScrollBar::doPaint(QPainter* painter)
{
    PQuickStyleItem::style()->drawComplexControl(QStyle::CC_ScrollBar, qstyleoption_cast<QStyleOptionSlider*>(m_styleoption), painter);
}
