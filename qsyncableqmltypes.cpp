#include <QtQml>
#include "qsjsonlistmodel.h"
#include "qsuuid.h"
#include "qsyncableqmlwrapper.h"

template <typename T>
static QObject *provider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    T* object = new T();

    return object;
}

static void registerTypes() {
    qmlRegisterType<QSJsonListModel>("QSyncable", 1, 0, "JsonListModel");
    qmlRegisterSingletonType<QSUuid>("QSyncable", 1, 0, "Uuid", provider<QSUuid>);
    qmlRegisterSingletonType<QSyncableQmlWrapper>("QSyncable", 1, 0, "QSyncable", provider<QSyncableQmlWrapper>);
}

Q_COREAPP_STARTUP_FUNCTION(registerTypes)
