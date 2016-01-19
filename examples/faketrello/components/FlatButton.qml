import QtQuick 2.0

Rectangle {
    id: button
    color: mouseArea.pressed ? "#1F000000" : "#00000000"

    signal clicked

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            button.clicked();
        }
    }

}

