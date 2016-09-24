#include <QTest>
#include <QSListModel>
#include <QSortFilterProxyModel>
#include <QSDiffRunner>
#include "QQmlApplicationEngine"
#include "automator.h"
#include "integrationtests.h"

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

    engine.load(QUrl(QString(SRCDIR) + "/test_Assign.qml"));
    Automator automator(&engine);

    QObject* root = automator.findObject("Root");
    QVERIFY(root);

    QVariantMap data;
    QSyncable::assign(data, root);

    QVERIFY(data["objectName"] == "Root");
    QVERIFY(data["value1"].toInt() == 1);
    QVERIFY(data["value2"].toString() == "2");
    QVERIFY(data["value3"].toBool());

    QVERIFY(data["value4"].type() == QVariant::Map);
    QVERIFY(data["value4"].toMap()["value1"].toInt() == 5);

    data.clear();
    QSyncable::assign(data, root, QStringList() << "value1" << "value4");

    QCOMPARE(data.size(), 2);
    QVERIFY(data.contains("value1"));
    QVERIFY(!data.contains("value2"));
    QVERIFY(data.contains("value4"));


}


