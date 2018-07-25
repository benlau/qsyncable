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
    // NPM: Find filesystem location for persistent storage
    QString persistDir =
#ifdef Q_OS_ANDROID /* for android, store data in a location that doesn't get erased each app update */
            QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/" //"/storage/emulated/0/"
             + "faketrello.example.qtproject.org";
#else
            QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#endif /* Q_OS_ANDROID */

    // NPM: Create application-specific directory for persistent storage if it doesn't exist
    QDir path;
    path.setPath(persistDir);
    if (!path.exists()) {
      path.mkpath(persistDir);
    }

    QCoreApplication* app = QCoreApplication::instance();

    // NPM: load board layout from persistence file,
    // or load dummy data if no persistence file, or parse error.
    QString persistFilePath = persistDir + "/persist.json";
    m_board.load(persistFilePath);

    m_engine.rootContext()->setContextProperty("PersistFilePath", persistFilePath);

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

// NPM -- see https://github.com/benlau/qsyncable/issues/2
QByteArray AppDelegate::stringifyModel()
{
    QVariantList lists;
    int count = cardListStore->storage().size();
    for (int i = 0 ; i < count ; i++) {
        lists << cardListStore->get(i);
    }

    QVariantMap map;
    map["lists"] = lists;
    QJsonObject object = QJsonObject::fromVariantMap(map);

    QJsonDocument doc;
    doc.setObject(object);
    QByteArray bytes = doc.toJson(QJsonDocument::Indented);

    return bytes;
}

// NPM -- see https://github.com/benlau/qsyncable/issues/2
void AppDelegate::persistModel(const QString& filePath) {
    QFile file(filePath);
    file.open(QIODevice::WriteOnly);
    file.write(AppDelegate::stringifyModel());
    file.close();
}

// NPM -- see https://github.com/benlau/qsyncable/issues/2
QByteArray AppDelegate::stringifyBoard()
{
    QVariantMap map = m_board.toMap();
    QJsonObject object = QJsonObject::fromVariantMap(map);

    QJsonDocument doc;
    doc.setObject(object);
    QByteArray bytes = doc.toJson(QJsonDocument::Indented);

    return bytes;
}

// NPM -- see https://github.com/benlau/qsyncable/issues/2
void AppDelegate::persistBoard(const QString& filePath) {
    QFile file(filePath);
    file.open(QIODevice::WriteOnly);
    file.write(AppDelegate::stringifyBoard());
    file.close();
}
