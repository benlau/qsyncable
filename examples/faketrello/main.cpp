#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "appdelegate.h"
#include "QSListModel"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    Q_UNUSED(app);
    registerQSyncableTypes();
    AppDelegate delegate;

    return delegate.run();
}

