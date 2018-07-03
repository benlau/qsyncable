import QtQuick 2.0
import "../components"

Item {
    id: card

    state: "Default"

    property int normalHeight: 48
    height: normalHeight

    property string listUuid
    property string cardUuid
    property string text
    property int cardIndex: -1

    function inSameList(c1, c2) {
        return c1.ListView.view === c2.ListView.view;
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        drag.target: container
        onReleased: {
            if (card.state === "Dragging") {
                container.Drag.drop();
                card.state = "Default";
            }
        }
    }

    DropArea {
        id: dropArea
        anchors.fill: parent
        enabled: !container.Drag.active
        onEntered: {
            if (card === drag.source) {
                return;
            }

            drag.accept(Qt.MoveAction);
            drag.source.state = "Dragging";

            if (inSameList(card, drag.source) && card.cardIndex >= drag.source.cardIndex) {
                card.state = "DroppingAfter";
            } else {
                card.state = "DroppingBefore";
            }
        }

        onExited: {
            if (card === drag.source) {
                return;
            }

            card.state = "Default";
        }

        onDropped: {
            if (card === drag.source) {
                return;
            }

            card.state = "Dropped"
            card.state = "Default";
            drag.source.state = "Default";

            App.moveCard(drag.source.listUuid,
                         drag.source.cardUuid,
                         card.listUuid,
                         card.cardUuid);
        }
    }

    Rectangle {
        id: container
        color: "#FFFFFF"

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        anchors.topMargin: 8

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

    states: [
        State {
            name: "Default"
        },
        State {
            name: "DroppingBefore"

            PropertyChanges {
                target: card
                height: normalHeight * 2
            }
        },
        State {
            name: "DroppingAfter"

            PropertyChanges {
                target: card
                height: normalHeight * 2
            }

            AnchorChanges {
                target: container
                anchors.bottom: undefined
                anchors.top: card.top
            }
        },
        State {
            name: "Dropped"
        },
        State {
            name: "Dragging"
            PropertyChanges {
                target: card
                height: 0
            }
        }
    ]

    onStateChanged: {
        console.log(card.text, card.state)
    }

    transitions: [
        Transition {
            from: "Default"
            to: "DroppingAfter,DroppingBefore"
            reversible: true

            NumberAnimation {
                target: card
                property: "height"
                duration: 200
                easing.type: Easing.Linear
                alwaysRunToEnd: true
            }
        },
        Transition {
            from: "Default"
            to: "Dragging"
            NumberAnimation {
                target: card
                property: "height"
                duration: 200
                easing.type: Easing.Linear
                alwaysRunToEnd: true
            }
        }
    ]

}

