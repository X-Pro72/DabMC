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
 *    Copyright © 2023 ivan tkachenko <me@ratijas.tk>
 *
 *    Licensed under LGPL-3.0-only OR GPL-2.0-or-later
 *      
 *          https://community.kde.org/Policies/Licensing_Policy
 */


import QtQuick
import QtQuick.Templates as T
import org.prismlauncher.pqcstyle as PrismStyle

T.Button {
    id: controlRoot

    implicitWidth: Math.max((text && display !== T.AbstractButton.IconOnly ?
        implicitBackgroundWidth : implicitHeight) + leftInset + rightInset,
        implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    baselineOffset: background ? background.y + background.baselineOffset : 0

    hoverEnabled: Qt.styleHints.useHoverEffects

    PrismStyle.MnemonicData.enabled: enabled && visible
    PrismStyle.MnemonicData.controlType: PrismStyle.MnemonicData.ActionElement
    PrismStyle.MnemonicData.label: display !== T.AbstractButton.IconOnly ? text : ""
    Shortcut {
        //in case of explicit & the button manages it by itself
        enabled: !(RegExp(/\&[^\&]/).test(controlRoot.text))
        sequence: controlRoot.PrismStyle.MnemonicData.sequence
        onActivated: controlRoot.clicked()
    }
    background: PrismStyle.PStyleButton {
        control: controlRoot
        sunken: controlRoot.down
        on: controlRoot.checkable && controlRoot.checked
        flat: controlRoot.flat
        hover: controlRoot.hovered
        text: controlRoot.PrismStyle.MnemonicData.mnemonicLabel
        hasFocus: controlRoot.activeFocus || controlRoot.highlighted
        activeControl: controlRoot.Accessible.defaultButton ? "default" : ""
        properties: {
            "icon": controlRoot.display !== T.AbstractButton.TextOnly
                ? (controlRoot.icon.name !== "" ? controlRoot.icon.name : controlRoot.icon.source) : null,
            "iconColor": Qt.colorEqual(controlRoot.icon.color, "transparent") ? palette.text : controlRoot.icon.color,
            "iconWidth": controlRoot.icon.width,
            "iconHeight": controlRoot.icon.height,

            "menu": controlRoot.Accessible.role === Accessible.ButtonMenu
        }
    }
}

