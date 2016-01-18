#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "appdelegate.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    Q_UNUSED(app);

    AppDelegate delegate;

    return delegate.run();
}

