import QtQuick 2.0
import "../components"

Item {
    id: card

    property int normalHeight: 48
    height: (dropArea.containsDrag && !dropArea.sameList) ? normalHeight * 2 : normalHeight

    property string listUuid
    property string cardUuid
    property string text

    Behavior on height {
        NumberAnimation {
            duration: 200
            easing.type: Easing.OutQuad;
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        drag.target: container
        onReleased: {
            container.Drag.drop();
        }
    }

    DropArea {
        id: dropArea
        anchors.fill: parent
        property bool sameList: true
        onEntered: {

            sameList = (card.listUuid  === drag.source.listUuid);
            drag.accept(Qt.MoveAction);

            if (card.listUuid !== drag.source.listUuid) {
                return;
            }

            if (card.cardUuid === drag.source.cardUuid) {
                return;
            }

            App.moveCard(drag.source.listUuid,
                         drag.source.cardUuid,
                         card.listUuid,
                         card.cardUuid);
        }

        onDropped: {
            if (!sameList) {
                drop.accept(Qt.MoveAction);
                App.moveCard(drag.source.listUuid,
                             drag.source.cardUuid,
                             card.listUuid,
                             card.cardUuid);
            }
        }
    }

    Rectangle {
        id: container
        color: "#FFFFFF"

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8

        width: card.width - 16
        height: normalHeight - 16

        Drag.active: mouseArea.drag.active
        Drag.hotSpot.x: 32
        Drag.hotSpot.y: 32
        Drag.source: card

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

        states: State {
            when: mouseArea.drag.active
            ParentChange {
                target: container;
                parent: window.contentItem
            }

            PropertyChanges {
                target: container
                z: 10000;
            }

            AnchorChanges {
                target: container;
                anchors.verticalCenter: undefined;
                anchors.horizontalCenter: undefined;
                anchors.bottom: undefined
            }
        }

    }
}

