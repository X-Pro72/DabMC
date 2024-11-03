# SPDX-FileCopyrightText: 2024 Rachel Powers <508861+Ryex@users.noreply.github.com>
#
# SPDX-License-Identifier: GPL-3.0-only
#
#  Prism Launcher - Minecraft Launcher
#  Copyright (C) 2024 Rachel Powers <508861+Ryex@users.noreply.github.com>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, version 3.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.

qt_add_qml_module(org_prismlauncher_desktop
  URI "org.prismlauncher.desktop"
  VERSION 1.0
  STATIC
  IMPORTS
    "org.prismlauncher.pqcstyle"
    "org.prismlauncher.platform"
  QML_FILES ${DESKTOP_QML_SOURCES}
)
set(DESKTOP_QML_SOURCES
  "Button.qml"
)
qt_target_qml_sources(org_prismlauncher_desktop
  QML_FILES ${DESKTOP_QML_SOURCES}
)
target_link_libraries(org_prismlauncher_desktop
    PUBLIC
    Qt${QT_VERSION_MAJOR}::QuickControls2
    org_prismlauncher_pqcstyle
    org_prismlauncher_platform
)
