import QtQuick 2.0
import QSyncable 1.0
import "../components"

Item {
    id: cardList;
    width: 240;

    property var listUuid
    property var title
    property var cards

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 16
        color: "#ccc"
        height: Math.min(listView.contentHeight + 16, cardList.height )

        Behavior on height {
            NumberAnimation {
                property: "height"
                duration: 200
                easing.type: Easing.OutQuad
            }
        }

        ListView {
            id: listView
            clip: true

            anchors.fill: parent

            add: Transition {

                NumberAnimation {
                    property: "opacity";
                    from: 0;
                    to: 1;
                    duration: 200
                    easing.type: Easing.OutQuad;
                }
            }

            addDisplaced: Transition {
                NumberAnimation {
                    properties: "x,y";
                    duration: 200
                    easing.type: Easing.OutQuad;
                }
            }

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

            moveDisplaced: Transition {
                NumberAnimation {
                    properties: "x,y";
                    duration: 200
                    easing.type: Easing.OutQuad;
                }
            }

            header: Item {
                width: listView.width
                height: 48

                Text {
                    anchors.fill: parent
                    anchors.leftMargin: 12

                    text: title
                    font.bold: true
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                }

                FlatButton {
                    width: 48
                    height: 48
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right

                    Image {
                        width: 24
                        height: 24
                        source: "../img/plus.png"
                        anchors.centerIn: parent
                    }

                    onClicked: {
                        App.addCard(listUuid);
                    }
                }
            }

            model: JsonListModel {
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

