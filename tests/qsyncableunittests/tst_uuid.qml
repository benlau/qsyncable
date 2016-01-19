import QtQuick 2.0
import QtTest 1.0
import QSyncable 1.0

Item {
    width: 640
    height: 480

    TestCase {
        name: "UuidTests"
        when: windowShown

        function test_create() {
            var uuid = Uuid.create();
            console.log(uuid);

            compare(uuid !== "", true);
            compare(uuid.length > 8, true);
            compare(uuid.indexOf("{") , -1);
            compare(uuid.indexOf("}") , -1);

            var table = {};

            for (var i = 0 ; i < 100 ; i++) {
                uuid = Uuid.create();
                compare(table.hasOwnProperty(uuid), false);
                table[uuid] = true;
            }

        }
    }
}

