import QtQuick 2.0

Item {
    id: card

    height: 48

    property string listUuid
    property string cardUuid
    property string text

    Rectangle {
        color: "#FFFFFF"
        anchors.fill: parent
        anchors.margins: 8

        Text {
            anchors.margins: 8

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft

            text: card.text
            anchors.fill: parent
        }

        Rectangle {
            width: height
            height: parent.height
            anchors.right: parent.right
            color: mouseArea.pressed ? "#1F000000" : "#00000000"

            Image {
                id: close
                width: 24
                height: 24
                source: "../img/close.png"
                anchors.centerIn: parent
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    App.removeCard(listUuid, cardUuid);
                }
            }

        }

    }

}

