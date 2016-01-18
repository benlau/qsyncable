#include <QVariantList>
#include <QTest>
#include "qsdiffrunner.h"
#include "qsyncabletests.h"
#include "qslistmodel.h"

QSyncableTests::QSyncableTests(QObject *parent) : QObject(parent)
{

}

void QSyncableTests::patch()
{
    QSPatch c1,c2;

    c1.setType(QSPatch::Insert);
    c2.setType(QSPatch::Move);

    QCOMPARE(c1 == c2, false);

    c1.setType(QSPatch::Move);

    QCOMPARE(c1 == c2, true);

    QVariantMap d1;
    d1["key"] = "1";

    c1.setData(d1);

    QCOMPARE(c1 == c2, false);
}

void QSyncableTests::patch_merge()
{
    QSPatch c1,c2,c3,c4;
    QVariantMap a,b;
    a["id"] = "a";
    b["id"] = "b";
    QVariantList data;

    c1 = QSPatch(QSPatch::Move);
    c2 = QSPatch(QSPatch::Insert);

    QVERIFY(!c1.canMerge(c2));

    c1 = QSPatch(QSPatch::Remove,0,0);
    c2 = QSPatch(QSPatch::Remove,1,1);

    QVERIFY(c1.canMerge(c2));
    QVERIFY(c2.canMerge(c1));

    c3 = c1.merge(c2);
    c4 = c2.merge(c1);

    QCOMPARE(c3.type(), QSPatch::Remove);
    QCOMPARE(c3.from(), 0);
    QCOMPARE(c3.to(), 1);

    QVERIFY(c3 == c4);

    c2 = QSPatch(QSPatch::Remove,2,2);

    QVERIFY(!c1.canMerge(c2));
    QVERIFY(!c2.canMerge(c1));

    /* Merge move */
    c1 = QSPatch(QSPatch::Move, 1, 0, 1);
    c2 = QSPatch(QSPatch::Move, 2, 1, 1);
    QVERIFY(c1.canMerge(c2));
    QVERIFY(!c2.canMerge(c1));

    c3 = c1.merge(c2);
    c4 = c2.merge(c1);

    QCOMPARE(c3.type(), QSPatch::Move);
    QCOMPARE(c3.from(), 1);
    QCOMPARE(c3.to(), 0);
    QCOMPARE(c3.count(), 2);

    QCOMPARE(c4.type(), QSPatch::Null);

    /* Merge insert */
    data.clear();
    data << a << b;

    c1 = QSPatch(QSPatch::Insert, 0, 0, 1, a);
    c2 = QSPatch(QSPatch::Insert, 1, 1, 1, b);

    QVERIFY(c1.canMerge(c2));
    QVERIFY(!c2.canMerge(c1));

    c3 = c1.merge(c2);

    QCOMPARE(c3.type(), QSPatch::Insert);
    QCOMPARE(c3.from(), 0);
    QCOMPARE(c3.to(), 1);
    QCOMPARE(c3.count(), 2);
    QVERIFY(c3.data() == data);

    /* Merge insert at same position */

    c1 = QSPatch(QSPatch::Insert, 0, 0, 1, a);
    c2 = QSPatch(QSPatch::Insert, 0, 0, 1, b);

    QVERIFY(c1.canMerge(c2));
    QVERIFY(c2.canMerge(c1));

    c3 = c1.merge(c2);

    QCOMPARE(c3.type(), QSPatch::Insert);
    QCOMPARE(c3.from(), 0);
    QCOMPARE(c3.to(), 1);
    QCOMPARE(c3.count(), 2);

}

void QSyncableTests::diffRunner()
{
    QFETCH(QVariantList, previous);
    QFETCH(QVariantList, current);
    QFETCH(QString, keyField);
    QFETCH(QList<QSPatch>, changes);

    QList<QSPatch> expectedChanges = changes;

    QSDiffRunner runner;
    runner.setKeyField(keyField);

    QList<QSPatch> result = runner.compare(previous, current);

    if (changes.size() != result.size()) {
        for (int i = 0 ; i < result.size() ; i++) {
            qDebug() << result.at(i);
        }
    }

    QCOMPARE(expectedChanges.size(), result.size());

    for (int i = 0; i < changes.size(); i++) {
        QSPatch expected = changes.at(i);
        QSPatch real = result.at(i);
        if (!(expected == real))  {
            qDebug() << expected;
            qDebug() << real;
        }
        QVERIFY(expected == real);
    }

    QSListModel* model = new QSListModel();

    model->setStorage(previous);
    QVERIFY(runner.patch(model, result));
    QVariantList modelList = model->storage();;

    if (modelList != current) {
        qDebug() << modelList;
        qDebug() << current;
    }
    QVERIFY(modelList == current);

    model->deleteLater();
}

void QSyncableTests::diffRunner_data()
{
    QTest::addColumn<QVariantList>("previous");
    QTest::addColumn<QVariantList>("current");
    QTest::addColumn<QString>("keyField");
    QTest::addColumn<QList<QSPatch> >("changes");

    QVariantList previous;
    QVariantList current;
    QList<QSPatch> changes;
    QSPatch c1;

    QVariantMap a,b,c,d,e;
    QVariantMap tmp;

    a["id"] = "a";
    b["id"] = "b";
    c["id"] = "c";
    d["id"] = "d";
    e["id"] = "e";

    previous << a << b << c;
    current << a << b << c;

    QTest::newRow("No Changes") << previous << current << "id" << changes;

    /* Remove first element */
    previous.clear();current.clear();changes.clear();
    previous << a << b << c;
    current << b << c;
    changes << QSPatch(QSPatch::Remove, 0, 0, 1);
    QTest::newRow("Remove first element") << previous << current << "id" << changes;

    /* Remove all element */
    previous.clear();current.clear();changes.clear();
    c1 = QSPatch();
    previous << a << b << c;
    changes << QSPatch(QSPatch::Remove, 0, 2, 3);
    QTest::newRow("Remove all element") << previous << current << "id" << changes;

    /* Add new element to end */

    previous.clear();current.clear();changes.clear();
    previous << a << b << c;
    current << a << b << c << d;
    changes << QSPatch(QSPatch::Insert, 3, 3, 1, d);
    QTest::newRow("Add new element to end") << previous << current << "id" << changes;

    /* Add 2 elements to middle */

    previous.clear();current.clear();changes.clear();
    previous << a << b;
    current << a << c << d << b;
    changes << QSPatch(QSPatch::Insert, 1, 2, 2, QVariantList() << c << d);
    QTest::newRow("Add 2 elements to middle") << previous << current << "id" << changes;

    /* Add 2 elements to differnt block */

    previous.clear();current.clear();changes.clear();
    previous << a << b << c;
    current << a << d << b << e << c;
    changes << QSPatch(QSPatch::Insert, 1, 1, 1, d)
            << QSPatch(QSPatch::Insert, 3, 3, 1, e);
    QTest::newRow("Add 2 elements to differnt block") << previous << current << "id" << changes;

    /* Move from last to first */
    previous.clear();
    previous << a << b << c;
    current.clear();
    current << c << a << b;
    changes.clear();
    changes << QSPatch(QSPatch::Move,2,0,1);

    QTest::newRow("Move from last to first") << previous << current << "id" << changes;

    /* Move 2 from first to last */
    previous.clear();
    previous << a << b << c;
    current.clear();
    current << c << a << b;
    changes.clear();
    changes << QSPatch(QSPatch::Move,2,0,1);

    QTest::newRow("Move 2 from first to last") << previous << current << "id" << changes;


    /* Move 2 from last to first */
    previous.clear();
    previous << a << b << c;
    current.clear();
    current << b << c << a;
    changes.clear();
    changes << QSPatch(QSPatch::Move,1,0,2);

    QTest::newRow("Move 2 from last to first") << previous << current << "id" << changes;

    /* Move 2 from last to middle */
    previous.clear();
    previous << a << b << c << d;
    current.clear();
    current << a << c << d << b;
    changes.clear();
    changes << QSPatch(QSPatch::Move,2,1,2);

    QTest::newRow("Move 2 from last to middle") << previous << current << "id" << changes;


    /* Update 2 elements*/
    previous.clear();
    previous << a << b << c;
    a["value"] = "a";
    b["value"] = "b";
    current.clear();
    current << a << b << c;

    tmp.clear();
    tmp["value"] = "a";

    changes.clear();
    changes << QSPatch(QSPatch::Update,0,0,1,tmp);

    tmp["value"] = "b";
    changes  << QSPatch(QSPatch::Update,1,1,1,tmp);

    QTest::newRow("Update 2 elements") << previous << current << "id" << changes;

}

void QSyncableTests::diffRunner_noKeyField()
{
    QFETCH(QVariantList, from);
    QFETCH(QVariantList, to);

    QSDiffRunner runner;
    QList<QSPatch> patches = runner.compare(from, to);


    QSListModel listModel;
    listModel.setStorage(from);
    runner.patch(&listModel, patches);

    QVERIFY(listModel.storage() == to);
}

void QSyncableTests::diffRunner_noKeyField_data()
{
    QTest::addColumn<QVariantList>("from");
    QTest::addColumn<QVariantList>("to");

    QVariantMap a,b,c,d,e;

    a["id"] = "a";
    b["id"] = "b";
    c["id"] = "c";
    d["id"] = "d";
    e["id"] = "e";

    QTest::newRow("Shifted")
           << (QVariantList() << a << b << c << d)
           << (QVariantList() << b << c << d << a);

    QTest::newRow("Remove")
           << (QVariantList() << a << b << c << d)
           << (QVariantList() << a << b << c);

    QTest::newRow("Add")
           << (QVariantList() << a << b << c << d)
           << (QVariantList() << a << b << c << d << e);
}

void QSyncableTests::listModel()
{
    QSListModel* model = new QSListModel();

    QVariantList expected;
    QVariantMap a,b,c,d;
    a["id"] = "a";
    b["id"] = "b";
    c["id"] = "c";
    d["id"] = "d";

    model->setStorage(QVariantList() << a << b);

    model->insert(1, QVariantList() << c << d);

    expected << a << c << d << b;

    QVERIFY(expected == model->storage());


    delete model;
}

