import QtQuick 2.0
import QSyncable 1.0

Item {
    id: cardList;
    width: 240;

    property var listUuid
    property var title
    property var cards

    Rectangle {

        anchors.fill: parent
        anchors.margins: 16
        color: "#ccc"

        ListView {
            id: listView

            anchors.fill: parent

            remove: Transition {
                NumberAnimation {
                    property: "opacity";
                    from: 1;
                    to: 0;
                    duration: 200
                    easing.type: Easing.OutQuad;
                }
            }

            removeDisplaced: Transition {
                NumberAnimation {
                    properties: "x,y";
                    duration: 200
                    easing.type: Easing.OutQuad;
                }
            }

            move: Transition {
                NumberAnimation {
                    properties: "x,y";
                    duration: 200
                    easing.type: Easing.OutQuad;
                }

            }

            header: Text {
                x: 12
                width: listView.width
                height: 48
                text: title
                font.bold: true
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
            }

            model: JsonModel {
                keyField: "uuid"
                source: cardList.cards
            }

            delegate: Card {
                listUuid: cardList.listUuid
                cardUuid: model.uuid
                text: model.text
                width: listView.width
            }

        }

    }
}

