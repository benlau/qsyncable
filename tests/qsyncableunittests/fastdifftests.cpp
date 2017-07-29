#include <QTest>
#include <qslistmodel.h>
#include "immutabletype1.h"
#include "immutabletype2.h"
#include "fastdifftests.h"
#include "priv/qsimmutablewrapper_p.h"
#include "priv/qsfastdiffrunneralgo_p.h"
#include "immutabletype3.h"
#include "qsfastdiffrunner.h"

template <typename T>
QVariantList convertList(QList<T> list) {
    QSImmutableWrapper<T> wrapper;
    QVariantList res;
    for (int i = 0 ; i < list.size();i++) {
        res << wrapper.convert(list[i]);
    }
    return res;
};

FastDiffTests::FastDiffTests(QObject *parent) : QObject(parent)
{
    auto ref = [=]() {
          QTest::qExec(this, 0, 0); // Autotest detect available test cases of a QObject by looking for "QTest::qExec" in source code
    };
    Q_UNUSED(ref);
}

void FastDiffTests::test_QSImmutable_wrapper()
{
    {
        ImmutableType1 v1, v2;
        QVERIFY(!v1.isSharedWith(v2));
        v1 = v2;
        QVERIFY(v1.isSharedWith(v2));
    }

    {
        QSImmutableWrapper<ImmutableType1> wrapper1;
        QSImmutableWrapper<ImmutableType2> wrapper2;
        QSImmutableWrapper<ImmutableType3> wrapper3;

        ImmutableType1 v1, v2;
        ImmutableType3 v3;

        QVERIFY(!wrapper1.isShared(v1,v2));
        v1 = v2;
        QVERIFY(wrapper1.isShared(v1,v2));

        v1.setId("a");
        QVariantMap map = wrapper1.convert(v1);
        QVERIFY(map["id"] == v1.id());

        QCOMPARE(wrapper1.hasKey(), true);
        QCOMPARE(wrapper2.hasKey(), false);

        v3.setValue(10);

        QCOMPARE(wrapper1.key(v1), QString("a"));
        QCOMPARE(wrapper3.key(v3), QString("10"));

        v2.setId("b");
        QVariantMap diff = wrapper1.diff(v1,v2);
        QCOMPARE(diff.size(), 1);
        QCOMPARE(diff["id"].toString(), QString("b"));

    }

    {
        QSImmutableWrapper<QVariantMap> wrapper;
        QVariantMap v1, v2;
        v1["value1"] = 9;
        QVERIFY(!wrapper.isShared(v1,v2));
        v1 = v2;
        QVERIFY(wrapper.isShared(v1,v2));

        v1["value1"] = 10;
        QVariantMap map = wrapper.convert(v1);
        QVERIFY(map == v1);

        QVERIFY(!wrapper.hasKey());
        wrapper.keyField = "value1";
        QVERIFY(wrapper.hasKey());
        QCOMPARE(wrapper.key(v1), QString::number(10));
    }
}

void FastDiffTests::test_QSFastDiffRunner()
{
    QFETCH(QList<ImmutableType1>, previous);
    QFETCH(QList<ImmutableType1>, current);
    QFETCH(QList<QSPatch>, changes);

    QList<QSPatch> expectedChanges = changes;

    QSFastDiffRunner<ImmutableType1> runner;


    QList<QSPatch> patches = runner.compare(previous, current);

    if (changes.size() != patches.size()) {
        qDebug() << expectedChanges;
        qDebug() << patches;
    }

    QCOMPARE(expectedChanges.size(), patches.size());

    for (int i = 0; i < changes.size(); i++) {
        QSPatch expected = changes.at(i);
        QSPatch real = patches.at(i);
        if (!(expected == real))  {
            qDebug() << "Expected" << expectedChanges;
            qDebug() << "Actual" << patches;
        }
        QVERIFY(expected == real);
    }

    QSListModel model;
    model.setStorage(convertList(previous));

    runner.patch(&model, patches);

    QVariant currentList = convertList(current);
    if (currentList != model.storage()) {
        qDebug() << "from" << convertList(previous);
        qDebug() << "to" << currentList;
        qDebug() << patches;
    }

    QVERIFY(currentList == model.storage());

}

void FastDiffTests::test_QSFastDiffRunner_data()
{
    QTest::addColumn<QList<ImmutableType1>>("previous");
    QTest::addColumn<QList<ImmutableType1>>("current");
    QTest::addColumn<QList<QSPatch> >("changes");

    QList<ImmutableType1> previous;
    QList<ImmutableType1> current;
    QList<QSPatch> changes;

    QSPatch c1;

    ImmutableType1 a,b,c,d,e,f;
    a.setId("a");
    b.setId("b");
    c.setId("c");
    d.setId("d");
    e.setId("e");
    f.setId("f");
    QSImmutableWrapper<ImmutableType1> wrapper;

    /* End of preparation */

    previous << a << b << c;
    current << a << b << c;

    QTest::newRow("No Changes") << previous << current << changes;

    previous.clear();current.clear();changes.clear();
    previous << a << b << c;
    current << b << c;
    changes << QSPatch(QSPatch::Remove, 0, 0, 1);
    QTest::newRow("Remove first element") << previous << current << changes;

    /* Remove all element */
    previous.clear();current.clear();changes.clear();
    c1 = QSPatch();
    previous << a << b << c;
    changes << QSPatch(QSPatch::Remove, 0, 2, 3);
    QTest::newRow("Remove all element") << previous << current << changes;

    /* Remove two elements from different position*/
    previous.clear();current.clear();changes.clear();
    previous << a << b << c << d << e;
    current << a << c << e;
    changes << QSPatch(QSPatch::Remove, 1, 1, 1)
            << QSPatch(QSPatch::Remove, 2, 2, 1);
    QTest::newRow("Remove two elements from different position") << previous << current << changes;

    previous.clear();current.clear();changes.clear();

    current << a << b << c;
    changes << QSPatch(QSPatch::Insert,0,2,3, convertList(QList<ImmutableType1>() << a << b << c));
    QTest::newRow("Add 3 elements to empty list") << previous << current << changes;

    /* Add new element to end */

    previous.clear();current.clear();changes.clear();
    previous << a << b << c;
    current << a << b << c << d;
    changes << QSPatch(QSPatch::Insert, 3, 3, 1, wrapper.convert(d));
    QTest::newRow("Add new element to end") << previous << current << changes;

    /* Add 2 elements to middle */

    previous.clear();current.clear();changes.clear();
    previous << a << b;
    current << a << c << d << b;
    changes << QSPatch(QSPatch::Insert, 1, 2, 2, convertList(QList<ImmutableType1>() << c << d));
    QTest::newRow("Add 2 elements to middle") << previous << current << changes;

    /* Add 2 elements to differnt block */

    previous.clear();current.clear();changes.clear();
    previous << a << b << c;
    current << a << d << b << e << c;
    changes << QSPatch(QSPatch::Insert, 1, 1, 1, wrapper.convert(d))
            << QSPatch(QSPatch::Insert, 3, 3, 1, wrapper.convert(e));
    QTest::newRow("Add 2 elements to differnt block") << previous << current << changes;

    /* Move from last to first */
    previous.clear();
    previous << a << b << c;
    current.clear();
    current << c << a << b;
    changes.clear();
    changes << QSPatch(QSPatch::Move,2,0,1);

    QTest::newRow("Move from last to first") << previous << current << changes;


    /* Move 2 from first to last */
    previous.clear();
    previous << a << b << c;
    current.clear();
    current << c << a << b;
    changes.clear();
    changes << QSPatch(QSPatch::Move,2,0,1);

    QTest::newRow("Move 2 from first to last") << previous << current << changes;

    /* Move 2 from last to first */
    previous.clear();
    previous << a << b << c;
    current.clear();
    current << b << c << a;
    changes.clear();
    changes << QSPatch(QSPatch::Move,1,0,2);

    QTest::newRow("Move 2 from last to first") << previous << current << changes;

    /* Move 2 from last to middle */
    previous.clear();
    previous << a << b << c << d;
    current.clear();
    current << a << c << d << b;
    changes.clear();
    changes << QSPatch(QSPatch::Move,2,1,2);

    QTest::newRow("Move 2 from last to middle") << previous << current << changes;

    /* Remove, Insert, Move */
    previous.clear();
    current.clear();
    changes.clear();

    previous << a << b << c << d << e;
    current << a << f << d << e << c;
    changes << QSPatch(QSPatch::Remove,1,1,1)
            << QSPatch(QSPatch::Insert,1,1,1,wrapper.convert(f))
            << QSPatch(QSPatch::Move,3,2,2);
    QTest::newRow("Remove, Insert, Move") << previous << current << changes;

    /* Update 2 elements */

    changes.clear();
    previous.clear();
    previous << a << b << c;
    a.setValue("a");
    b.setValue("b");
    current.clear();
    current << a << b << c;

    QVariantMap tmp;
    tmp["value"] = "a";

    changes.clear();
    changes << QSPatch(QSPatch::Update,0,0,1,tmp);

    tmp["value"] = "b";
    changes  << QSPatch(QSPatch::Update,1,1,1,tmp);

    QTest::newRow("Update 2 elements") << previous << current << changes;
}
