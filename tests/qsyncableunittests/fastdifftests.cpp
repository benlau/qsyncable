#include <QTest>
#include "immutabletype1.h"
#include "fastdifftests.h"
#include "priv/qsimmutablewrapper_p.h"

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
        QSImmutableWrapper<ImmutableType1> wrapper;
        ImmutableType1 v1, v2;
        QVERIFY(!wrapper.fastCompare(v1,v2));
        v1 = v2;
        QVERIFY(wrapper.fastCompare(v1,v2));

        v1.setValue1(10);
        QVariantMap map = wrapper.convert(v1);
        QVERIFY(map["value1"] == v1.value1());
    }

    {
        QSImmutableWrapper<QVariantMap> wrapper;
        QVariantMap v1, v2;
        QVERIFY(!wrapper.fastCompare(v1,v2));
        v1 = v2;
        // QVariantMap do not support fastCompare
        QVERIFY(!wrapper.fastCompare(v1,v2));

        v1["value1"] = 10;
        QVariantMap map = wrapper.convert(v1);
        QVERIFY(map == v1);
    }

}
