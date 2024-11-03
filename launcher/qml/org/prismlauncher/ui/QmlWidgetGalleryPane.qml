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

import QtQuick
import QtQuick.Controls

import QtQuick.Layouts
import QtQuick.Window

ThemedItem {
    id: root
    width: gridLayout.implicitWidth
    height: gridLayout.implicitHeight
    property int minimumWidth: gridLayout.implicitWidth
    property int minimumheight: gridLayout.implicitHeight

    Rectangle {
        id: container
        anchors.fill: parent
        color: root.palette.window
        width: gridLayout.implicitWidth
        height: gridLayout.implicitHeight

        GridLayout {
            id: gridLayout
            columns: 2
            RowLayout {
                Layout.row: 0
                Layout.column: 0
                Layout.columnSpan: 2
                Item {
                    // spacer item
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                CheckBox {
                    id: disableQmlWidgetsCheckbox
                    text: qsTr("Disable &Qml widgets")
                    onClicked: {
                        buttonGroupBox.checked = disableQmlWidgetsCheckbox.checked;
                        simpleInputGroupBox.checked = disableQmlWidgetsCheckbox.checked;
                    }
                }
            }
            GroupBox {
                id: buttonGroupBox
                Layout.row: 1
                Layout.column: 0
                property bool checked
                label: CheckBox {
                    id: buttonGroupCheckBox
                    text: qsTr("Buttons")
                    checked: buttonGroupBox.checked
                }
                RowLayout {
                    anchors.fill: parent
                    enabled: buttonGroupCheckBox.checked
                    ColumnLayout {
                        Button {
                            text: qsTr("Default Push button")
                        }
                        Button {
                            checkable: true
                            checked: true
                            text: qsTr("Toggle Push Button")
                        }
                        Button {
                            flat: true
                            text: qsTr("Flat Push Button")
                        }
                        RowLayout {
                            id: toolLayout

                            ToolButton {
                                text: qsTr("Tool Button")
                            }
                            ToolButton {
                                text: qsTr("Menu Button")

                                onClicked: {
                                    toolMenu.popup();
                                }

                                Menu {
                                    id: toolMenu
                                    MenuItem {
                                        text: qsTr("Option")
                                    }
                                    MenuItem {
                                        checkable: true
                                        text: qsTr("Checkable Option")
                                    }
                                }
                            }
                        }
                        Item {
                            // spacer item
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }
                    }
                    ColumnLayout {
                        RadioButton {
                            checked: true
                            text: qsTr("Radio Button 1")
                        }
                        RadioButton {
                            text: qsTr("Radio Button 2")
                        }
                        RadioButton {
                            text: qsTr("Radio Button 3")
                        }
                        CheckBox {
                            tristate: true
                            checkState: Qt.PartiallyChecked
                            text: qsTr("Tri-state check box")
                        }

                        Item {
                            // spacer item
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }
                    }
                    Item {
                        // spacer item
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
            }
            GroupBox {
                id: simpleInputGroupBox
                Layout.row: 1
                Layout.column: 1
                property bool checked
                label: CheckBox {
                    id: simpleInputGroupCheckBox
                    text: qsTr("Simple Input Widgets")
                    checked: simpleInputGroupBox.checked
                }
                GridLayout {
                    anchors.fill: parent
                    enabled: simpleInputGroupBox.checked

                    TextField {
                        Layout.row: 0
                        Layout.column: 0
                        Layout.rowSpan: 1
                        Layout.columnSpan: 2
                        text: "s3cRe7"
                        echoMode: TextInput.Password
                        // add a clear button?
                    }
                    SpinBox {
                        Layout.row: 1
                        Layout.column: 0
                        Layout.rowSpan: 1
                        Layout.columnSpan: 2
                        value: 50
                    }
                    Slider {
                        Layout.row: 2
                        Layout.column: 0
                        value: 40
                    }
                    ScrollBar {
                        Layout.row: 3
                        Layout.column: 0
                        orientation: Qt.Horizontal
                        position: 0.6
                    }
                    Item {
                        //spacer
                        Layout.row: 4
                        Layout.column: 0
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
            }
        }
    }
}
