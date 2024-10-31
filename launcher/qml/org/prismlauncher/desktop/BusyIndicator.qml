
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
 *    Copyright ©: 2018 Oleg Chernovskiy <adonai@xaker.ru>
 *    Copyright ©: 2022 ivan tkachenko <me@ratijas.tk>
 *
 *    Licensed under LGPL-3.0-only OR GPL-2.0-or-later
 *      
 *          https://community.kde.org/Policies/Licensing_Policy
 */
import QtQuick
import QtQuick.Templates as T
import org.prismlauncher.pqcstyle as PrismStyle
import org.prismlauncher.platform as Platform
import org.prismlauncher.primitives as Primitives

T.BusyIndicator {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    // BusyIndicator doesn't need padding since it has no background.
    // A Control containing a BusyIndicator can have padding instead
    // (e.g., a ToolBar, a Page or maybe a widget in a Plasma panel).
    padding: 0

    hoverEnabled: false

    contentItem: Item {
        /* Binding on `visible` implicitly takes care of `control.visible`,
         * `control.running` and `opacity > 0` at once.
         * Also, don't animate at all if the user has disabled animations,
         * and don't animate when window is hidden (which somehow does not
         * affect items' visibility).
         */
        readonly property bool animationShouldBeRunning:
            visible
            && Window.visibility !== Window.Hidden
            && PrismStyle.Units.longDuration > 1

        /* implicitWidth and implicitHeight won't work unless they come
         * from a child of the contentItem. No idea why.
         */
        implicitWidth: Platform.Units.gridUnit * 2
        implicitHeight: Platform.Units.gridUnit * 2

        // We can't bind directly to opacity, as Animator won't update its value immediately.
        visible: control.running || opacityAnimator.running
        opacity: control.running ? 1 : 0
        Behavior on opacity {
            enabled: Platform.Units.shortDuration > 0
            OpacityAnimator {
                id: opacityAnimator
                duration: Platform.Units.shortDuration
                easing.type: Easing.OutCubic
            }
        }

        // sync all busy animations such that they start at a common place in the rotation
        onAnimationShouldBeRunningChanged: startOrStopAnimation();

        function startOrStopAnimation() {
            if (rotationAnimator.running === animationShouldBeRunning) {
                return;
            }
            if (animationShouldBeRunning) {
                const date = new Date;
                const ms = date.valueOf();
                const startAngle = ((ms % rotationAnimator.duration) / rotationAnimator.duration) * 360;
                rotationAnimator.from = startAngle;
                rotationAnimator.to = startAngle + 360
            }
            rotationAnimator.running = animationShouldBeRunning;
        }

        Primitives.Icon {
            /* Do not use `anchors.fill: parent` in here or else
             * the aspect ratio won't always be 1:1.
             */
            anchors.centerIn: parent
            width: Math.min(parent.width, parent.height)
            height: width

            source: "process-working-symbolic"
            smooth: true

            RotationAnimator on rotation {
                id: rotationAnimator
                from: 0
                to: 360
                // Not using a standard duration value because we don't want the
                // animation to spin faster or slower based on the user's animation
                // scaling preferences; it doesn't make sense in this context
                duration: 2000
                loops: Animation.Infinite
                // Initially false, will be set as appropriate after
                // initialization. Can't be bound declaratively due to the
                // procedural nature of to/from adjustments: order of
                // assignments is crucial, as animator won't use new to/from
                // values while running.
                running: false
            }
        }

        Component.onCompleted: startOrStopAnimation();
    }
}
