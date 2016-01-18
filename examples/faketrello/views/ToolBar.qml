import QtQuick 2.0
import QtQuick.Controls 1.4

Item {
    height: 48

    Button {
        height: parent.height
        anchors.right: parent.right
        text: "Add"

        onClicked: {
            App.addList();
        }
    }
}

