#include <QTest>
#include <QSListModel>
#include <QSortFilterProxyModel>
#include <QSDiffRunner>
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
