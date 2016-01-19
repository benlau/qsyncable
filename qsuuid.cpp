#include <QUuid>
#include <QtQml>
#include "qsuuid.h"

QSUuid::QSUuid(QObject *parent) : QObject(parent)
{

}

QString QSUuid::create() const
{
    return QUuid::createUuid().toString().replace(QRegExp("[{}]"), "");
}


static QObject *provider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    QSUuid* object = new QSUuid();

    return object;
}


class QSUuidRegistionHelper {

public:
    QSUuidRegistionHelper() {
        qmlRegisterSingletonType<QSUuid>("QSyncable", 1, 0, "Uuid", provider);
    }
};

static QSUuidRegistionHelper registerHelper;

