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

qt_add_qml_module(org_prismlauncher_data
  URI "org.prismlauncher.data"
  VERSION 1.0
  STATIC
)
target_include_directories(org_prismlauncher_data PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/org.prismlauncher.data")
set(QML_DATA_CPP_SOURCES
)
target_sources(org_prismlauncher_data PRIVATE ${QML_DATA_CPP_SOURCES})

target_link_libraries(org_prismlauncher_data
    PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Quick
)
