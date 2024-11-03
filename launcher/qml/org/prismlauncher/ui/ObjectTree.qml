import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import QtQuick.Window

// import "./components"

Rectangle {
    id: pane
    anchors.fill: parent
    color: palette.window

    signal onInspectPressed

    // The model needs to be a QAbstractItemModel
    property alias treeModel: treeView.model
 
    ColumnLayout {
        anchors {
            fill: parent
            rightMargin: 6
        }

        RowLayout {
            id: topRow

            Layout.fillWidth: true
            implicitHeight: 48

            Controls.Button {
                text: qsTr("Inspect")

                onPressed: pane.onInspectPressed()

                hoverEnabled: true

                Controls.ToolTip {
                    text: qsTr("Print tree to console")
                    visible: parent.hovered
                }
            }

            Controls.Button {
                text: qsTr("Inspect Qml")

                onPressed: {
                    console.log( treeView.model.data(treeView.model.index(0, 0), 0));
                }

                hoverEnabled: true

                Controls.ToolTip {
                    text: qsTr("Print tree to console (QML side)")
                    visible: parent.hovered
                }
            }
        }

        TreeView {
            id: treeView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 10

            selectionModel: ItemSelectionModel {}
            delegate: Controls.TreeViewDelegate {}
        }
    }
}
