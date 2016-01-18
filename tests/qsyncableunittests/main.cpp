#include <QString>
#include <QtTest>
#include "testrunner.h"
#include "qsyncabletests.h"

int main(int argc, char* argv[]) {

    QGuiApplication app(argc, argv);
    TestRunner runner;

    runner.add<QSyncableTests>();
    runner.add(QString(SRCDIR));

    bool error = runner.exec(app.arguments());

    if (!error) {
        qDebug() << "All test cases passed!";
    }

    return error;
}
