import QtQuick 2.0
import QtTest 1.0
import QSyncable 1.0

Item {
    width: 640
    height: 480

    TestCase {
        name: "JsonModelTests"
        when: windowShown

        Component {
            id: jsonModelCreator1
            JsonModel {
                keyField: "id"
                fieldNames: [
                    "id",
                    "value"
                ]
            }
        }

        Repeater {
            id: repeater1
            delegate : Item {
                property var value: model.value
            }
        }

        function test_fieldNames() {
            var jsonModel = jsonModelCreator1.createObject();
            compare(jsonModel.count, 0);

            jsonModel.append({ id: "a", value: 1});
            compare(jsonModel.count, 1);
            var item = jsonModel.get(0);
            compare(item.id , "a");
            compare(item.value, 1);

            repeater1.model = jsonModel;
            compare(repeater1.count, 1);

            jsonModel.source = [ { id:"a", value:2 },  { id:"b", value:3 }]
            compare(jsonModel.count, 2);
            compare(repeater1.count, 2);

            item = repeater1.itemAt(1);
            compare(item.value, 3);
        }

        Component {
            id: jsonModelCreator2
            JsonModel {
                keyField: "id"

                source: [
                    { "id": "a", "field1": 1, "field2": 2}
                ]

                fieldNames: [
                    "id", "field1"
                ]
            }
        }

        Repeater {
            id: repeater2

            delegate: Item {
                property var field1: model.field1
                property var field2: model.field2
            }
        }

        function test_source() {
            var jsonModel = jsonModelCreator2.createObject();
            repeater2.model = jsonModel;

            compare(repeater2.count, 1);
            var item = repeater2.itemAt(0);
            compare(item.field1, 1);
            compare(item.field2, undefined);

        }
    }
}

