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

    for (int i = 100 ; i <= 100000 ; i*=10) {
        QTest::newRow(QString("%1").arg(i).toLocal8Bit().constData()) << i;
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

    for (int i = 100 ; i <= 100000 ; i*=10) {
        QTest::newRow(QString("%1").arg(i).toLocal8Bit().constData()) << i;
    }
}

