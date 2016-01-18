#include <QCoreApplication>
#include <QQmlContext>
#include "appdelegate.h"
#include "qsdiffrunner.h"

AppDelegate::AppDelegate(QObject *parent) : QObject(parent)
{
    cardListStore = new QSListModel(this);

}

int AppDelegate::run()
{
    // Pretend to load from file, but now it just load dummy daa.
    m_board.load();

    QCoreApplication* app = QCoreApplication::instance();

    m_engine.rootContext()->setContextProperty("CardListStore", cardListStore);

    m_engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    sync();

    return app->exec();
}

void AppDelegate::sync()
{
    QVariantMap map = m_board.toMap();

    QVariantList lists = map["lists"].toList();

    QSDiffRunner runner;
    runner.setKeyField("uuid");

    QList<QSPatch> patches = runner.compare(cardListStore->storage(),
                                            lists);

    runner.patch(cardListStore, patches);
}

