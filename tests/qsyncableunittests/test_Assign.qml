import QtQuick 2.0

QtObject {
    id: root
    objectName: "Root"

    property int value1 : 1

    property string value2: "2"

    property bool value3 : true

    // Nested object
    property var value4: QtObject {
        property int value1 : 4 + 1
    }

}
