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

#include "Slider.h"
#include "PQuickStyleItem.h"

#include <QApplication>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionSlider>

PStyleSlider::PStyleSlider(QQuickItem* parent) : PQuickStyleItem(parent)
{
    m_type = QStringLiteral("button");
}

void PStyleSlider::doInitStyleOption()
{
    if (!m_styleoption) {
        m_styleoption = new QStyleOptionSlider();
    }

    QStyleOptionSlider* opt = qstyleoption_cast<QStyleOptionSlider*>(m_styleoption);
    opt->orientation = horizontal() ? Qt::Horizontal : Qt::Vertical;
    opt->upsideDown = !horizontal();

    int min = minimum();
    int max = std::max(min, maximum());

    opt->minimum = min;
    opt->maximum = max;
    opt->sliderPosition = value();
    opt->singleStep = step();

    if (opt->singleStep) {
        qreal numOfSteps = (opt->maximum - opt->minimum) / opt->singleStep;
        // at least 5 pixels between tick marks
        qreal extent = horizontal() ? width() : height();
        if (numOfSteps && (extent / numOfSteps < 5)) {
            opt->tickInterval = qRound((5 * numOfSteps / extent) + 0.5) * step();
        } else {
            opt->tickInterval = opt->singleStep;
        }

    } else {  // default Qt-components implementation
        opt->tickInterval = opt->maximum != opt->minimum ? 1200 / (opt->maximum - opt->minimum) : 0;
    }

    opt->sliderValue = value();
    opt->subControls = QStyle::SC_SliderGroove | QStyle::SC_SliderHandle;
    opt->tickPosition = activeControl() == QLatin1String("ticks") ? QSlider::TicksBelow : QSlider::NoTicks;
    if (opt->tickPosition != QSlider::NoTicks) {
        opt->subControls |= QStyle::SC_SliderTickmarks;
    }

    opt->activeSubControls = QStyle::SC_SliderHandle;
}

QSize PStyleSlider::getContentSize(int, int)
{
    QSize size;
    const auto opt = qstyleoption_cast<const QStyleOptionSlider*>(m_styleoption);
    const auto tickPosition = opt->tickPosition;

    // Copied from QSlider
    const int SliderLength = 84, TickSpace = 5;
    int thick = style()->pixelMetric(QStyle::PM_SliderThickness, m_styleoption, nullptr);
    if (tickPosition & QSlider::TicksAbove) {
        thick += TickSpace;
    }
    if (tickPosition & QSlider::TicksBelow) {
        thick += TickSpace;
    }
    int w = thick;
    int h = SliderLength;
    if (opt->orientation == Qt::Horizontal) {
        w = SliderLength;
        h = thick;
    }
    size = PQuickStyleItem::style()->sizeFromContents(QStyle::CT_Slider, m_styleoption, QSize(w, h));
    return size;
}

qreal PStyleSlider::baselineOffset() const
{
    QRect r;
    r = m_styleoption->rect;
    r.adjust(0, 0, 0, -2);
    return baselineOffsetFromRect(r);
}

QRectF PStyleSlider::subControlRect(const QString& subcontrolString)
{
    QStyle::SubControl subcontrol = QStyle::SC_None;
    initStyleOption();
    QStyle::ComplexControl control = QStyle::CC_Slider;
    if (subcontrolString == QLatin1String("handle")) {
        subcontrol = QStyle::SC_SliderHandle;
    } else if (subcontrolString == QLatin1String("groove")) {
        subcontrol = QStyle::SC_SliderGroove;
    }
    return PQuickStyleItem::style()->subControlRect(control, qstyleoption_cast<QStyleOptionComplex*>(m_styleoption), subcontrol);
}

void PStyleSlider::doPaint(QPainter* painter)
{
    PQuickStyleItem::style()->drawComplexControl(QStyle::CC_Slider, qstyleoption_cast<QStyleOptionComplex*>(m_styleoption), painter);
}
