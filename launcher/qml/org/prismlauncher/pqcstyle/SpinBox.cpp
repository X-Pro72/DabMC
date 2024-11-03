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

#include "SpinBox.h"
#include "PQuickStyleItem.h"

#include <QApplication>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionSpinBox>

PStyleSpinBox::PStyleSpinBox(QQuickItem* parent) : PQuickStyleItem(parent)
{
    m_type = QStringLiteral("spinbox");
}

void PStyleSpinBox::doInitStyleOption()
{
    if (!m_styleoption) {
        m_styleoption = new QStyleOptionSpinBox();
    }

    QStyleOptionSpinBox* opt = qstyleoption_cast<QStyleOptionSpinBox*>(m_styleoption);
    opt->frame = true;
    if (value() & 0x1) {
        opt->activeSubControls = QStyle::SC_SpinBoxUp;
    } else if (value() & (1 << 1)) {
        opt->activeSubControls = QStyle::SC_SpinBoxDown;
    } else {
        opt->activeSubControls = QStyle::SC_None;
    }
    opt->subControls = QStyle::SC_All;
    opt->stepEnabled = {};
    if (value() & (1 << 2)) {
        opt->stepEnabled |= QAbstractSpinBox::StepUpEnabled;
    }
    if (value() & (1 << 3)) {
        opt->stepEnabled |= QAbstractSpinBox::StepDownEnabled;
    }
}

QSize PStyleSpinBox::getContentSize(int width, int height)
{
    QSize size;
    // We have to create a new style option since we might be calling with a QStyleOptionSpinBox
    QStyleOptionFrame frame;
    //+2 to be consistent with the hardcoded verticalmargin in QLineEdit
    int contentHeight = frame.fontMetrics.height() + 2;

    frame.state = m_styleoption->state | QStyle::State_Sunken;
    frame.lineWidth = PQuickStyleItem::style()->pixelMetric(QStyle::PM_DefaultFrameWidth, m_styleoption, nullptr);
    frame.rect = m_styleoption->rect;
    frame.styleObject = this;

    size = PQuickStyleItem::style()->sizeFromContents(QStyle::CT_LineEdit, &frame, QSize(width, qMax(height, contentHeight)));
    size.setWidth(PQuickStyleItem::style()->sizeFromContents(QStyle::CT_SpinBox, m_styleoption, QSize(width + 2, height)).width());
    return size;
}

qreal PStyleSpinBox::baselineOffset() const
{
    QRect r;
    if (const auto option = qstyleoption_cast<const QStyleOptionSpinBox*>(m_styleoption)) {
        r = PQuickStyleItem::style()->subControlRect(QStyle::CC_SpinBox, option, QStyle::SC_SpinBoxEditField);
    }
    return baselineOffsetFromRect(r, false);
}

QRectF PStyleSpinBox::subControlRect(const QString& subcontrolString)
{
    QStyle::SubControl subcontrol = QStyle::SC_None;
    initStyleOption();
    QStyle::ComplexControl control = QStyle::CC_SpinBox;
    if (subcontrolString == QLatin1String("down")) {
        subcontrol = QStyle::SC_SpinBoxDown;
    } else if (subcontrolString == QLatin1String("up")) {
        subcontrol = QStyle::SC_SpinBoxUp;
    } else if (subcontrolString == QLatin1String("edit")) {
        subcontrol = QStyle::SC_SpinBoxEditField;
    }
    return PQuickStyleItem::style()->subControlRect(control, qstyleoption_cast<QStyleOptionComplex*>(m_styleoption), subcontrol);
}

void PStyleSpinBox::doPaint(QPainter* painter)
{
#ifdef Q_OS_MAC
    // macstyle depends on the embedded qlineedit to fill the editfield background
    if (styleName() == QLatin1String("mac")) {
        QRect editRect = PQuickStyleItem::style()->subControlRect(
            QStyle::CC_SpinBox, qstyleoption_cast<QStyleOptionComplex*>(m_styleoption), QStyle::SC_SpinBoxEditField);
        painter->fillRect(editRect.adjusted(-1, -1, 1, 1), m_styleoption->palette.base());
    }
#endif
    PQuickStyleItem::style()->drawComplexControl(QStyle::CC_SpinBox, qstyleoption_cast<QStyleOptionComplex*>(m_styleoption), painter);
}
