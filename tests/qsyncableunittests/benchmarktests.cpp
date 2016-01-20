#include <QTest>
#include <QUuid>
#include "benchmarktests.h"
#include "qsdiffrunner.h"

static QVariantList create(int size) {

    QVariantList result;

    for (int i = 0 ; i < size ; i++) {
        QVariantMap item;
        item["id"] = QUuid::createUuid().toString().replace(QRegExp("[{}]"), "");
        item["value"] =  qrand();
        result << item;
    }

    return result;
}

BenchmarkTests::BenchmarkTests(QObject *parent) : QObject(parent)
{
#if 0
    sizes << 100 << 1000 << 10000 << 100000;
#else
    sizes << 100000;
#endif
}

void BenchmarkTests::changeAll()
{
    QFETCH(int, size);

    QVariantList from = create(size);
    QVERIFY(from.size() == size);

    QVariantList to = from;
    for (int i = 0 ; i < to.count() ; i++) {
        QVariantMap item = to.at(i).toMap();
        item["value"] = item["value"].toInt() + 1;
        to[i] = item;
    }

    QBENCHMARK {
        QSDiffRunner runner;
        runner.setKeyField("id");
        runner.compare(from, to);
    }

}

void BenchmarkTests::changeAll_data()
{
    QTest::addColumn<int>("size");

    foreach(int size, sizes) {
        QTest::newRow(QString("%1").arg(size).toLocal8Bit().constData()) << size;
    }

}

void BenchmarkTests::setup()
{
    QTest::addColumn<int>("size");

    foreach(int size, sizes) {
        QTest::newRow(QString("%1").arg(size).toLocal8Bit().constData()) << size;
    }

}

void BenchmarkTests::noChange()
{
    QFETCH(int, size);

    QVariantList from = create(size);
    QVERIFY(from.size() == size);

    QVariantList to = from;

    QBENCHMARK {
        QSDiffRunner runner;
        runner.setKeyField("id");
        runner.compare(from, to);
    }

}

void BenchmarkTests::noChange_data()
{
    QTest::addColumn<int>("size");

    foreach(int size, sizes) {
        QTest::newRow(QString("%1").arg(size).toLocal8Bit().constData()) << size;
    }
}

void BenchmarkTests::changeOne()
{
    QFETCH(int, size);

    QVariantList from = create(size);
    QVERIFY(from.size() == size);

    QVariantList to = from;
    QVariantMap item = to.at(0).toMap();
    item["value"] = item["value"].toInt() + 1;
    to[0] = item;

    QVERIFY(from != to);

    QBENCHMARK {
        QSDiffRunner runner;
        runner.setKeyField("id");
        runner.compare(from, to);
    }
}

void BenchmarkTests::changeOne_data()
{
    QTest::addColumn<int>("size");

    foreach(int size, sizes) {
        QTest::newRow(QString("%1").arg(size).toLocal8Bit().constData()) << size;
    }
}

void BenchmarkTests::moveOne()
{
    QFETCH(int, size);

    QVariantList from = create(size);
    QVERIFY(from.size() == size);

    QVariantList to = from;
    to.move(0, to.size() - 1);

    QVERIFY(from != to);

    QBENCHMARK {
        QSDiffRunner runner;
        runner.setKeyField("id");
        runner.compare(from, to);
    }
}

void BenchmarkTests::moveOne_data()
{
    QTest::addColumn<int>("size");

    foreach(int size, sizes) {
        QTest::newRow(QString("%1").arg(size).toLocal8Bit().constData()) << size;
    }

}

void BenchmarkTests::insertAll()
{
    QFETCH(int, size);

    QVariantList from;
    QVariantList to = create(size);

    QVERIFY(from != to);

    QBENCHMARK {
        QSDiffRunner runner;
        runner.setKeyField("id");
        runner.compare(from, to);
    }

}

void BenchmarkTests::insertAll_data()
{
    setup();
}

