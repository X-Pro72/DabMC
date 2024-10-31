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

qt_add_qml_module(org_prismlauncher_pqcstyle
  URI "org.prismlauncher.pqcstyle"
  VERSION 1.0
  STATIC
  SOURCES ${PQCSTYLE_CPP_SOURCES}
)
target_include_directories(org_prismlauncher_pqcstyle PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/org.prismlauncher.pqcstyle")

set(PQCSTYLE_CPP_SOURCES
  "PQuickStyleItem.h"
  "PQuickStyleItem.cpp"
  "PStylePalette.cpp"
  "PStylePalette.h"
  "PQuickPadding.h"
  "MnemonicData.h"
  "MnemonicData.cpp"

  # items
  "Button.cpp"
  "Button.h"
  "CheckBox.cpp"
  "CheckBox.h"
  "ComboBox.cpp"
  "ComboBox.h"
  "ComboBoxItem.cpp"
  "ComboBoxItem.h"
  "DelayButton.cpp"
  "DelayButton.h"
  "Dial.cpp"
  "Dial.h"
  "Edit.cpp"
  "Edit.h"
  "FocusFrame.cpp"
  "FocusFrame.h"
  "FocusRect.cpp"
  "FocusRect.h"
  "Frame.cpp"
  "Frame.h"
  "GroupBox.cpp"
  "GroupBox.h"
  "Header.cpp"
  "Header.h"
  "Item.cpp"
  "Item.h"
  "ItemBranchIndicator.cpp"
  "ItemBranchIndicator.h"
  "ItemRow.cpp"
  "ItemRow.h"
  "Menu.cpp"
  "Menu.h"
  "MenuBar.cpp"
  "MenuBar.h"
  "MenuBarItem.cpp"
  "MenuBarItem.h"
  "MenuItem.cpp"
  "MenuItem.h"
  "ProgressBar.cpp"
  "ProgressBar.h"
  "RadioButton.cpp"
  "RadioButton.h"
  "ScrollBar.cpp"
  "ScrollBar.h"
  "Slider.cpp"
  "Slider.h"
  "SpinBox.cpp"
  "SpinBox.h"
  "Splitter.cpp"
  "Splitter.h"
  "Tab.cpp"
  "Tab.h"
  "TabFrame.cpp"
  "TabFrame.h"
  "ToolBar.cpp"
  "ToolBar.h"
  "ToolButton.cpp"
  "ToolButton.h"
)
# TODO: add these classes if needed
# Widget
# ScrollAreaCorner
# StatusBar

target_sources(org_prismlauncher_pqcstyle PRIVATE ${PQCSTYLE_CPP_SOURCES})
target_link_libraries(org_prismlauncher_pqcstyle
    PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Quick
)
