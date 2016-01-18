import QtQuick 2.3
import QtQuick.Window 2.2

Window {
    width: 640
    height: 480
    visible: true

    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }

    Text {
        text: qsTr("Hello World")
        anchors.centerIn: parent
    }

    Repeater {
        model: CardListStore
        delegate: Item {
            width: 480
            height: 48
            Text {
                text: model.uuid;
            }
            Component.onCompleted:  {
                console.log("completed");
            }
        }
    }
}

