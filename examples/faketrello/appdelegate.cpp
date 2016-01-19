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

    m_engine.rootContext()->setContextProperty("App", this);

    m_engine.rootContext()->setContextProperty("CardListStore", cardListStore);

    m_engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    sync();

    return app->exec();
}

void AppDelegate::addList()
{
    m_board.addList();
    sync();
}

void AppDelegate::addCard(const QString &listUuid)
{
    m_board.addCard(listUuid);
    sync();
}

void AppDelegate::removeCard(const QString& listUuid, const QString& cardUuid)
{
    m_board.removeCard(listUuid, cardUuid);
    sync();
}

void AppDelegate::moveCard(const QString &listUuid, const QString &fromCardUUid, const QString &toCardUuid)
{
    m_board.moveCard(listUuid, fromCardUUid, toCardUuid);
    sync();
}

void AppDelegate::sync()
{
    // It is the only way to update QML model, you don't need
    // to handle move, insert, remove operations explicitly.
    QVariantMap map = m_board.toMap();

    QVariantList lists = map["lists"].toList();

    QSDiffRunner runner;

    // It is important to set an unique key field on input data.
    // Otherwise, it won't be able to generate insertion, removal and move patch.
    runner.setKeyField("uuid");

    QList<QSPatch> patches = runner.compare(cardListStore->storage(),
                                            lists);

    runner.patch(cardListStore, patches);
}

