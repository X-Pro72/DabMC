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

# turns out this ECM module in unreliable,
# we need qt6 > 6.2 and preferably > 6.5 anyway
# include(ECMQmlModule)

qt_policy(SET QTP0001 NEW)

add_subdirectory(org/prismlauncher/pqcstyle)
add_subdirectory(org/prismlauncher/desktop)
add_subdirectory(org/prismlauncher/data)
add_subdirectory(org/prismlauncher/ui)

target_link_libraries(Launcher_logic
    PUBLIC
    org_prismlauncher_pqcstyle
    org_prismlauncher_desktop
    org_prismlauncher_data
    org_prismlauncher_ui
)

target_sources(Launcher_logic PUBLIC qtquickcontrols2.qrc)

