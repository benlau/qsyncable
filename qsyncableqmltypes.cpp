#include <QtQml>
#include "qsjsonlistmodel.h"
#include "qsuuid.h"

static QObject *provider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    QSUuid* object = new QSUuid();

    return object;
}

static void registerTypes() {
    qmlRegisterType<QSJsonListModel>("QSyncable", 1, 0, "JsonListModel");
    qmlRegisterSingletonType<QSUuid>("QSyncable", 1, 0, "Uuid", provider);
}

Q_COREAPP_STARTUP_FUNCTION(registerTypes)
