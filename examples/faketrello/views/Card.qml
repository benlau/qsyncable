import QtQuick 2.0

Item {
    id: card

    height: 48

    property var text

    Rectangle {
        color: "#FFFFFF"
        anchors.fill: parent
        anchors.margins: 8

        Text {
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            text: card.text
            anchors.centerIn: parent
        }

    }

}

