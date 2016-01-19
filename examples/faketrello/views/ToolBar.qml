import QtQuick 2.0
import QtQuick.Controls 1.4
import "../components"

Item {
    height: 48

    FlatButton {
        width: label.width + 32
        height: parent.height
        anchors.right: parent.right

        Text {
            id: label
            text: "New List"
            anchors.centerIn: parent
        }

        onClicked: {
            App.addList();
        }
    }
}

