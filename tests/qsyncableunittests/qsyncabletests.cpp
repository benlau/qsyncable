#include <QVariantList>
#include <QTest>
#include "qsdiffrunner.h"
#include "qsyncabletests.h"
#include "qslistmodel.h"

static QStringList convert(const QVariantList& list) {
    QStringList res;

    for (int i = 0 ; i < list.size() ; i++) {
        res << list.at(i).toMap()["id"].toString();
    }

    return res;
}

static QVariantList convert(const QStringList& list) {
    QVariantList res;

    for (int i = 0 ; i < list.size() ; i++) {
        QVariantMap item;
        item["id"] = list.at(i);
        res << item;
    }

    return res;

}

QSyncableTests::QSyncableTests(QObject *parent) : QObject(parent)
{

     qsrand( QDateTime::currentDateTime().toTime_t());
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
    QVariantMap a,b,c;
    a["id"] = "a";
    b["id"] = "b";
    c["id"] = "c";

    QVariantList data;

    c1 = QSPatch(QSPatch::Move);
    c2 = QSPatch(QSPatch::Insert);

    QVERIFY(!c1.canMerge(c2));

    c1 = QSPatch(QSPatch::Remove,0,0);
    c2 = QSPatch(QSPatch::Remove,1,1);

    QVERIFY(c1.canMerge(c2));
    QVERIFY(c2.canMerge(c1));

    c3 = c1.merged(c2);
    c4 = c2.merged(c1);

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

    c3 = c1.merged(c2);
    c4 = c2.merged(c1);

    QCOMPARE(c3.type(), QSPatch::Move);
    QCOMPARE(c3.from(), 1);
    QCOMPARE(c3.to(), 0);
    QCOMPARE(c3.count(), 2);

    QCOMPARE(c4.type(), QSPatch::Null);

    /* Merge seq of insert */
    data.clear();
    data << a << b;

    c1 = QSPatch(QSPatch::Insert, 0, 0, 1, a);
    c2 = QSPatch(QSPatch::Insert, 1, 1, 1, b);
    c3 = QSPatch(QSPatch::Insert, 2, 2, 1, c);

    QVERIFY(c1.canMerge(c2));
    QVERIFY(!c2.canMerge(c1));

    c4 = c1.merged(c2);

    QCOMPARE(c4.type(), QSPatch::Insert);
    QCOMPARE(c4.from(), 0);
    QCOMPARE(c4.to(), 1);
    QCOMPARE(c4.count(), 2);
    QVERIFY(c4.data() == data);

    QVERIFY(c4.canMerge(c3));
    c4 = c4.merged(c3);


    QCOMPARE(c4.type(), QSPatch::Insert);
    QCOMPARE(c4.from(), 0);
    QCOMPARE(c4.to(), 2);
    QCOMPARE(c4.count(), 3);

    /* Merge insert at same position */

    c1 = QSPatch(QSPatch::Insert, 0, 0, 1, a);
    c2 = QSPatch(QSPatch::Insert, 0, 0, 1, b);

    QVERIFY(c1.canMerge(c2));
    QVERIFY(c2.canMerge(c1));

    c3 = c1.merged(c2);

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
        qDebug() << expectedChanges;
        qDebug() << result;
    }

    QCOMPARE(expectedChanges.size(), result.size());

    for (int i = 0; i < changes.size(); i++) {
        QSPatch expected = changes.at(i);
        QSPatch real = result.at(i);
        if (!(expected == real))  {
            qDebug() << expectedChanges;
            qDebug() << result;
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

    QVariantMap a,b,c,d,e,f;
    QVariantMap tmp;

    a["id"] = "a";
    b["id"] = "b";
    c["id"] = "c";
    d["id"] = "d";
    e["id"] = "e";
    f["id"] = "f";

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

    /* Remove two elements from different position*/
    previous.clear();current.clear();changes.clear();
    previous << a << b << c << d << e;
    current << a << c << e;
    changes << QSPatch(QSPatch::Remove, 1, 1, 1)
            << QSPatch(QSPatch::Remove, 2, 2, 1);
    QTest::newRow("Remove two elements from different position") << previous << current << "id" << changes;

    /* Add 3 elements to empty list*/
    previous.clear();current.clear();changes.clear();

    current << a << b << c;
    changes << QSPatch(QSPatch::Insert,0,2,3, QVariantList() << a << b << c);
    QTest::newRow("Add 3 elements to empty list") << previous << current << "id" << changes;

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

    /* Remove, Insert, Move */
    previous.clear();
    current.clear();
    changes.clear();

    previous << a << b << c << d << e;
    current << a << f << d << e << c;
    changes << QSPatch(QSPatch::Remove,1,1,1)
            << QSPatch(QSPatch::Insert,1,1,1,f)
            << QSPatch(QSPatch::Move,3,2,2);
    QTest::newRow("Remove, Insert, Move") << previous << current << "id" << changes;

}

void QSyncableTests::diffRunner_failedCase()
{
    QFETCH(QString, from);
    QFETCH(QString, to);

    QVariantList fList = convert(from.split(","));
    QVariantList tList = convert(to.split(","));


    QSListModel listModel;

    listModel.setStorage(fList);

    QSDiffRunner runner;
    runner.setKeyField("id");

    QList<QSPatch> patches = runner.compare(fList, tList);
    runner.patch(&listModel, patches);

    if (tList != listModel.storage()) {
        qDebug() << "from" << from;
        qDebug() << "to" << to;
        qDebug() << "actual" << convert(listModel.storage()).join(",");
        qDebug() << patches;
    }

    QVERIFY(tList == listModel.storage());
}

void QSyncableTests::diffRunner_failedCase_data()
{
    QTest::addColumn<QString>("from");
    QTest::addColumn<QString>("to");

//    QTest::newRow("1") << "1,2,3,4,5,6,7"
//                       << "3,1,2,4,5,6,7";

    QTest::newRow("2") << "0,1,2,3,4,5,6,7,8,9"
                       << "1,11,2,3,12,4,5,6,10,7,8,0,9";

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

void QSyncableTests::diffRunner_invalidKey()
{
    QVariantMap a,b,c,d,e;

    a["id"] = "a";
    b["id"] = "b";
    c["id"] = "c";
    d["id"] = "d";
    e["id"] = "e";

    QVariantList from,to;
    from << a << b << c << d;
    to << d << b << c << a;

    QSDiffRunner runner;
    runner.setKeyField("uuid"); // Set a wrong key field
    QList<QSPatch> patches = runner.compare(from, to);
    QCOMPARE(patches.size() , 2);

    QSListModel listModel;
    listModel.setStorage(from);
    runner.patch(&listModel, patches);

    QVERIFY(listModel.storage() == to);
}

void QSyncableTests::diffRunner_random()
{
    QVariantList from;
    int count = 10;
    for (int i = 0 ; i < count;i++) {
        QVariantMap item;
        item["id"] = i;
        item["value"] = i;
        from << item;
    }

    QVariantList to = from;
    int nextId = count;
    QVariantMap item;

    for (int i = 0 ; i < 10 ;i++) {
        int type = qrand()  % 4;
        int f = qrand() % count;
        int t = qrand() % count;
        switch (type) {
        case 0:
            item = to[i].toMap();
            item["value"] = item["value"].toInt() + 1;
            to[i] = item;
            break;
        case 1:
            to.removeAt(f);
            break;
        case 2:
            item = QVariantMap();
            item["id"] = nextId++;
            item["value"] = nextId;
            to.insert(f,item);
        case 3:
            to.move(f,t);
            break;
        }
    }

    QSListModel listModel;

    listModel.setStorage(from);

    QSDiffRunner runner;
    runner.setKeyField("id");

    QList<QSPatch> patches = runner.compare(from, to);
    runner.patch(&listModel, patches);

    if (to != listModel.storage()) {
        qDebug() << "from" << convert(from).join(",");
        qDebug() << "to" << convert(to).join(",");
        qDebug() << "actual" << convert(listModel.storage()).join(",");
        qDebug() << patches;

    }

    QVERIFY(to == listModel.storage());
}

void QSyncableTests::listModel_insert()
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

void QSyncableTests::listModel_roleNames()
{
    QSListModel* model = new QSListModel();
    QCOMPARE(model->roleNames().size(), 0);
    delete model;

    QVariantMap item;
    item["id"] = "test";
    item["value"] = "test";

    model = new QSListModel();
    model->append(item);
    QCOMPARE(model->roleNames().size(), 2);
    delete model;

    model = new QSListModel();
    model->insert(0, item);
    QCOMPARE(model->roleNames().size(), 2);
    delete model;

    model = new QSListModel();
    model->insert(0, QVariantList() << item << item);
    QCOMPARE(model->roleNames().size(), 2);
    delete model;
}

