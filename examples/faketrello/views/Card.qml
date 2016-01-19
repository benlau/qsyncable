import QtQuick 2.0
import "../components"

Item {
    id: card

    height: 48

    property string listUuid
    property string cardUuid
    property string text

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
    }

    Rectangle {
        id: container
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

        FlatButton {
            width: height
            height: parent.height
            anchors.right: parent.right

            opacity: mouseArea.containsMouse ? 1 : 0
            enabled: mouseArea.containsMouse

            Image {
                id: close
                width: 24
                height: 24
                source: "../img/close.png"
                anchors.centerIn: parent
            }

            onClicked: {
                App.removeCard(listUuid, cardUuid);
            }

            Behavior on opacity {
                NumberAnimation {
                    property: "opacity";
                    duration: 200
                    easing.type: Easing.OutQuad;
                }
            }

        }

    }

}

