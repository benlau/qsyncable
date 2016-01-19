import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import "./views"

Window {
    id: window
    width: 640
    height: 480
    visible: true

    ColumnLayout {
        anchors.fill: parent

        ToolBar {
            Layout.fillWidth: true
        }

        Board {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

    }

}

