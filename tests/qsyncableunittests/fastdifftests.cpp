#include <QTest>
#include "immutabletype1.h"
#include "immutabletype2.h"
#include "fastdifftests.h"
#include "priv/qsimmutablewrapper_p.h"
#include "priv/qsfastdiffrunneralgo_p.h"
#include "immutabletype3.h"
#include "qsfastdiffrunner.h"

FastDiffTests::FastDiffTests(QObject *parent) : QObject(parent)
{

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

        v1.setValue("a");
        QVariantMap map = wrapper1.convert(v1);
        QVERIFY(map["value"] == v1.value());

        QCOMPARE(wrapper1.hasKey(), true);
        QCOMPARE(wrapper2.hasKey(), false);

        v3.setValue(10);

        QCOMPARE(wrapper1.key(v1), QString("a"));
        QCOMPARE(wrapper3.key(v3), QString("10"));

        v2.setValue("b");
        QVariantMap diff = wrapper1.diff(v1,v2);
        QCOMPARE(diff.size(), 1);
        QCOMPARE(diff["value"].toString(), QString("b"));

    }

    {
        QSImmutableWrapper<QVariantMap> wrapper;
        QVariantMap v1, v2;
        QVERIFY(!wrapper.isShared(v1,v2));
        v1 = v2;
        // QVariantMap do not support fastCompare
        QVERIFY(!wrapper.isShared(v1,v2));

        v1["value1"] = 10;
        QVariantMap map = wrapper.convert(v1);
        QVERIFY(map == v1);
    }
}

void FastDiffTests::test_QSFastDiffRunnerAlgo()
{
    QSFastDiffRunnerAlgo<QVariantMap> algo;
    Q_UNUSED(algo);

}

void FastDiffTests::test_QSFastDiffRunner()
{
    QFETCH(QList<ImmutableType1>, previous);
    QFETCH(QList<ImmutableType1>, current);
    QFETCH(QList<QSPatch>, changes);

    QList<QSPatch> expectedChanges = changes;

    QSFastDiffRunner<ImmutableType1> runner;


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
            qDebug() << "Expected" << expectedChanges;
            qDebug() << "Actual" << result;
        }
        QVERIFY(expected == real);
    }

    /* @TODO - apply to model */
}

void FastDiffTests::test_QSFastDiffRunner_data()
{
    QTest::addColumn<QList<ImmutableType1>>("previous");
    QTest::addColumn<QList<ImmutableType1>>("current");
    QTest::addColumn<QList<QSPatch> >("changes");

    QList<ImmutableType1> previous;
    QList<ImmutableType1> current;
    QList<QSPatch> changes;

    ImmutableType1 a,b,c,d,e,f;
    a.setValue("a");
    b.setValue("b");
    c.setValue("c");
    d.setValue("d");
    e.setValue("e");
    f.setValue("f");

    /* End of preparation */

    previous << a << b << c;
    current << a << b << c;

    QTest::newRow("No Changes") << previous << current << changes;

    previous.clear();current.clear();changes.clear();
    previous << a << b << c;
    current << b << c;
    changes << QSPatch(QSPatch::Remove, 0, 0, 1);
    QTest::newRow("Remove first element") << previous << current << changes;

}
