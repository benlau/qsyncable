#include <QVariantList>
#include <QTest>
#include "qsdiffrunner.h"
#include "qsyncabletests.h"

QSyncableTests::QSyncableTests(QObject *parent) : QObject(parent)
{

}

void QSyncableTests::change()
{
    QSChange c1,c2;

    c1.setType(QSChange::Insert);
    c2.setType(QSChange::Move);

    QCOMPARE(c1 == c2, false);

    c1.setType(QSChange::Move);

    QCOMPARE(c1 == c2, true);

    QVariantMap d1;
    d1["key"] = "1";

    c1.setData(d1);

    QCOMPARE(c1 == c2, false);
}

void QSyncableTests::diffRunner()
{
    QFETCH(QVariantList, previous);
    QFETCH(QVariantList, current);
    QFETCH(QString, keyField);
    QFETCH(QList<QSChange>, changes);

    QSDiffRunner runner;
    runner.setKeyField(keyField);

    QList<QSChange> result = runner.run(previous, current);

    QCOMPARE(changes.size(), result.size());

    for (int i = 0; i < changes.size(); i++) {
        QSChange expected = changes.at(i);
        QSChange real = result.at(i);
        QVERIFY(expected == real);
    }

}

void QSyncableTests::diffRunner_data()
{
    QTest::addColumn<QVariantList>("previous");
    QTest::addColumn<QVariantList>("current");
    QTest::addColumn<QString>("keyField");
    QTest::addColumn<QList<QSChange> >("changes");

    QVariantList previous;
    QVariantList current;
    QVariantList keyField;
    QList<QSChange> changes;

    QVariantMap a,b,c;
    a["id"] = "a";
    b["id"] = "b";
    c["id"] = "c";

    previous << a << b << c;
    current << a << b << c;

    QTest::newRow("No Changes") << previous << current << "id" << changes;
}

