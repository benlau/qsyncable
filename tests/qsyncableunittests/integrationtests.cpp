#include <QTest>
#include <QSListModel>
#include <QSortFilterProxyModel>
#include <QSDiffRunner>
#include <QtShell>
#include "QQmlApplicationEngine"
#include "automator.h"
#include "integrationtests.h"

using namespace QSyncable;

IntegrationTests::IntegrationTests(QObject *parent) : QObject(parent)
{

}

void IntegrationTests::sortFilterProxyModel()
{
    QSListModel listModel;

    QVariantList list;

    for (int i = 0 ; i < 5;i++) {
        QVariantMap map;
        map["id"] = i;
        map["value"] = i;
        map["label"] = QString("Item %1").arg(i);
        list << map;
    }

    listModel.setStorage(list);

    QHash<int , QByteArray> roles = listModel.roleNames();

    QVERIFY(listModel.count() == 5);

    QSortFilterProxyModel proxyModel;

    proxyModel.setSourceModel(&listModel);
    QCOMPARE(proxyModel.rowCount() , 5);

    proxyModel.setFilterRole(roles.key("label"));
    proxyModel.setFilterFixedString("Item 4");
    QCOMPARE(proxyModel.rowCount() , 1);

    QCOMPARE(proxyModel.data(proxyModel.index(0,0),roles.key("id")).toInt() , 4);

    proxyModel.setFilterFixedString("");
    QCOMPARE(proxyModel.rowCount() , 5);

    proxyModel.setSortRole(roles.key("value"));
    QCOMPARE(proxyModel.rowCount() , 5);

    proxyModel.sort(0, Qt::DescendingOrder);
    QCOMPARE(proxyModel.data(proxyModel.index(0,0),roles.key("id")).toInt(), 4);

    proxyModel.sort(0, Qt::AscendingOrder);
    QCOMPARE(proxyModel.data(proxyModel.index(0,0),roles.key("id")).toInt(), 0);

    // Reverse the value of list
    for (int i = 0 ; i < list.size(); i++) {
        QVariantMap map = list[i].toMap();
        map["value"] = 5 - i;
        list[i] = map;
    }

    QSDiffRunner runner;
    QSPatchSet patches;

    patches = runner.compare(listModel.storage(), list);
    runner.patch(&listModel, patches);

    QCOMPARE(proxyModel.data(proxyModel.index(0,0),roles.key("id")).toInt(), 4);

}


void IntegrationTests::test_assign()
{
    QQmlApplicationEngine engine;

    engine.load(QUrl(QString(SRCDIR) + "/SampleData1.qml"));
    Automator automator(&engine);

    QObject* root = automator.findObject("Root");
    QVERIFY(root);

    /* assign(map, QObject) */

    QVariantMap data;
    QSyncable::assign(data, root);

    QVERIFY(data["objectName"] == "Root");
    QVERIFY(data["value1"].toInt() == 1);
    QVERIFY(data["value2"].toString() == "2");
    QVERIFY(data["value3"].toBool());

    QVERIFY(data["value4"].type() == QVariant::Map);
    QVERIFY(data["value4"].toMap()["value1"].toInt() == 5);

    /* assign(QObject, map) */
    data.clear();
    data["value1"] = 99;
    QVariantMap value4;
    value4["value1"] = 32;
    data["value4"] = value4;

    QSyncable::assign(root, data);
    QVERIFY(root->property("value1").toInt() == 99);
    QVERIFY(root->property("value4").value<QObject*>()->property("value1").toInt() == 32);

    /* assign(QObject, QJSvalue)*/
    QString content = QtShell::cat(QString(SRCDIR) + "/SampleData1.json");
    QJSValue value = engine.evaluate(content);

    QSyncable::assign(root, value);

    QCOMPARE(root->property("value1").toInt(), 10);
    QVERIFY(root->property("value2").toString() == "11");
    QVERIFY(root->property("value3").toBool() == false);
    QCOMPARE(root->property("value4").value<QObject*>()->property("value1").toInt(), 21);
}

void IntegrationTests::test_get()
{
    QQmlApplicationEngine engine;

    engine.load(QUrl(QString(SRCDIR) + "/SampleData1.qml"));
    Automator automator(&engine);

    QObject* root = automator.findObject("Root");
    QVERIFY(root);

    /* get(QObject*, QString) */
    QVariant value = QSyncable::get(root, "value4.value1");
    QCOMPARE(value.toInt(), 5);

    value = QSyncable::get(root,"value4.valueX", QString("Not Found"));
    QVERIFY(value.toString() == "Not Found");

    /* get(QVarnaintMap, QString) */

    QVariantMap source;
    assign(source, root);
    value = QSyncable::get(source, "value2");
    QVERIFY(value.toString() == "2");

    value = QSyncable::get(source, "valueX");
    QVERIFY(value.isNull());

}

void IntegrationTests::test_set()
{
    QVariantMap data;
    QSyncable::set(data,"value1", 1);
    QVERIFY(data.contains("value1"));
    QVERIFY(data["value1"].toInt() == 1);

    QSyncable::set(data,"value2","value2");
    QVERIFY(data.contains("value1"));
    QVERIFY(data["value1"].toInt() == 1);
    QVERIFY(data.contains("value2"));
    QVERIFY(data["value2"].toString() == "value2");

    QSyncable::set(data,"value3.value1",2);

    QVariantMap value3 = data["value3"].toMap();
    QVERIFY(value3["value1"].toInt() == 2);

    /* Override value */
    data["value4"] = true;
    QSyncable::set(data,"value4.value1",3);
    QVariant value4 = data["value4"];
    QVERIFY(value4.canConvert<QVariantMap>());

    QVERIFY(value4.toMap()["value1"].toInt() == 3);
}

void IntegrationTests::test_pick()
{
    QQmlApplicationEngine engine;

    engine.load(QUrl(QString(SRCDIR) + "/SampleData1.qml"));
    Automator automator(&engine);

    QObject* root = automator.findObject("Root");
    QVERIFY(root);

    /* QSyncable::pick(QObject*, paths) */

    QVariantMap data = QSyncable::pick(root, QStringList()
                                       << "value1"
                                       << "value4.value1");

    QCOMPARE(data.size(), 2);
    QVERIFY(data.contains("value1"));
    QVERIFY(!data.contains("value2"));
    QVERIFY(data.contains("value4"));

    QVERIFY(data["value4"].toMap()["value1"].toInt() == 5);

    // Pick an QObject
    data = QSyncable::pick(root, QStringList() << "value4");
    QVERIFY(data["value4"].type() == QVariant::Map);

    /* QSyncable::pick(QVariant, paths) */
    QVariantMap source;
    QSyncable::assign(source, root);

    data = QSyncable::pick(source, QStringList() << "value1" << "value4.value1");

    QCOMPARE(data.size(), 2);
    QVERIFY(data.contains("value1"));
    QVERIFY(!data.contains("value2"));
    QVERIFY(data.contains("value4"));

    QVERIFY(data["value4"].toMap()["value1"].toInt() == 5);

}

void IntegrationTests::test_omit()
{
    QQmlApplicationEngine engine;

    engine.load(QUrl(QString(SRCDIR) + "/SampleData1.qml"));
    Automator automator(&engine);

    QObject* root = automator.findObject("Root");
    QVERIFY(root);

    QVariantMap data1;

    QSyncable::assign(data1, root);
    QVariantMap properties;
    properties["value1"] = true;
    properties["value3"] = false; // omit do not care the content

    QVariantMap data2 = QSyncable::omit(data1, properties);

    QVERIFY(!data2.contains("value1"));
    QVERIFY(data2.contains("value2"));
    QVERIFY(!data2.contains("value3"));
    QVERIFY(data2.contains("value4"));
    QVERIFY(data2["value4"].type() == QVariant::Map);
}


