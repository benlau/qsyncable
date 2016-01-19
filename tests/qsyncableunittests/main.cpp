#include <QString>
#include <QtTest>
#include "testrunner.h"
#include "qsyncabletests.h"
#include "benchmarktests.h"

int main(int argc, char* argv[]) {

    QGuiApplication app(argc, argv);
    TestRunner runner;

    runner.add<QSyncableTests>();
    runner.add(QString(SRCDIR));
    runner.add<BenchmarkTests>();

    bool error = runner.exec(app.arguments());

    if (!error) {
        qDebug() << "All test cases passed!";
    }

    return error;
}
