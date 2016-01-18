import QtQuick 2.0
import QSyncable 1.0

Item {
    id: cardList;
    width: 240;

    property var title
    property var cards

    Rectangle {

        anchors.fill: parent
        anchors.margins: 16
        color: "#ccc"

        ListView {
            id: listView

            anchors.fill: parent

            header: Text {
                width: listView.width
                height: 48
                text: title
                font.bold: true
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }

            model: JsonModel {
                keyField: "uuid"
                source: cardList.cards
            }

            delegate: Card {
                text: model.text
                width: listView.width
            }

        }

    }
}

