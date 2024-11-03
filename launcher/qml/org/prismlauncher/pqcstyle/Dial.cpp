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

#include "Dial.h"
#include "PQuickStyleItem.h"

#include <QApplication>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionSlider>

PStyleDial::PStyleDial(QQuickItem* parent) : PQuickStyleItem(parent)
{
    m_type = QStringLiteral("dial");
}

void PStyleDial::doInitStyleOption()
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

void PStyleDial::doPaint(QPainter* painter)
{
    PQuickStyleItem::style()->drawComplexControl(QStyle::CC_Dial, qstyleoption_cast<QStyleOptionComplex*>(m_styleoption), painter);
}
