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

#pragma once

#include "PQuickStyleItem.h"

class PStyleButton : public PQuickStyleItem {
    Q_OBJECT
    QML_ELEMENT

   public:
    PStyleButton(QQuickItem* parent = nullptr);
    ~PStyleButton() = default;

   public:
    void doInitStyleOption() override;
    void doPaint(QPainter* painter) override;

    QSize getContentSize(int width, int height) override;
    QPalette getResolvedPalette() override;

   protected:
    const char* classNameForItem() const override { return "QPushButton"; }
    void doResolvePalette() override;

    qreal baselineOffset() const override;

   private:
    QPalette m_buttonPalette;
};
