import QtQuick 2.0
import QtTest 1.0
import QSyncable 1.0

Item {
    width: 640
    height: 480

    TestCase {
        name: "QSyncableTests"
        when: windowShown

        Item {
            id: target1
            property int value1 : 0;
            property real value2 : 0;
            property string value3: "";
        }

        function test_assign() {
            QSyncable.assign(target1, {
                                value1: 1,
                                value2: 2.0,
                                value3: "3"
                             });
            compare(target1.value1, 1);
            compare(target1.value2, 2.0);
            compare(target1.value3, "3");
        }

    }
}

