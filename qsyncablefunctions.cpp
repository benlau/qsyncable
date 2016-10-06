#include <QMetaProperty>
#include <QtQml>
#include "qsyncablefunctions.h"

void QSyncable::assign(QVariantMap &dest, const QObject *source)
{
    const QMetaObject* meta = source->metaObject();

    for (int i = 0 ; i < meta->propertyCount(); i++) {
        const QMetaProperty property = meta->property(i);
        QString p = property.name();

        QVariant value = source->property(property.name());

        if (value.canConvert<QObject*>()) {
            QVariantMap map;
            assign(map, value.value<QObject*>()); // nested properties is not supported yet
            value = map;
        }

        dest[p] = value;
    }

}

void QSyncable::assign(QObject *dest, const QVariantMap & source)
{
    const QMetaObject* meta = dest->metaObject();

    QMap<QString,QVariant>::const_iterator iter = source.begin();
    while (iter != source.end()) {
        QByteArray key = iter.key().toLocal8Bit();

        int index = meta->indexOfProperty(key.constData());
        if (index < 0) {
            qWarning() << QString("QSyncable::assign:assign a non-existed property: %1").arg(iter.key());
            iter++;
            continue;
        }

        QVariant orig = dest->property(key.constData());
        QVariant value = source[iter.key()];

        if (orig.canConvert<QObject*>()) {
            if (value.type() != QVariant::Map) {
                qWarning() << QString("QSyncable::assign:expect a QVariantMap property but it is not: %1");
            } else {
                assign(orig.value<QObject*>(), value.toMap());
            }

        } else if (orig != value) {
            dest->setProperty(key.constData(), value);
        }

        iter++;
    }
}

static QVariant _get(const QVariantMap& object, const QStringList &path, const QVariant& defaultValue) ;

static QVariant _get(const QObject* object, const QStringList &path, const QVariant& defaultValue) {

    QString key = path[0];

    const QMetaObject* meta = object->metaObject();

    if (meta->indexOfProperty(key.toUtf8().constData()) < 0) {
        return defaultValue;
    }

    QVariant value = object->property(key.toUtf8().constData());

    if (path.size() == 1) {
        return value;
    }

    QStringList nextPath = path;
    nextPath.removeFirst();

    if (value.canConvert<QObject*>()) {
        return _get(qvariant_cast<QObject*>(value), nextPath, defaultValue);
    } else if (value.type() == QVariant::Map) {
        return _get(value.toMap(), nextPath, defaultValue);
    } else {
        return defaultValue;
    }
}

static QVariant _get(const QVariantMap& object, const QStringList &path, const QVariant& defaultValue) {

    QString key = path[0];

    if (!object.contains(key)) {
        return defaultValue;
    }

    QVariant value = object[key];

    if (path.size() == 1) {
        return value;
    }

    QStringList nextPath = path;
    nextPath.removeFirst();

    if (value.canConvert<QObject*>()) {
        return _get(qvariant_cast<QObject*>(value), nextPath, defaultValue);
    } else if (value.type() == QVariant::Map) {
        return _get(value.toMap(), nextPath, defaultValue);
    } else {
        return defaultValue;
    }
}

QVariant QSyncable::get(const QObject *object, const QString &path, const QVariant& defaultValue)
{
    return get(object, path.split("."), defaultValue);
}

QVariant QSyncable::get(const QObject *object, const QStringList &path, const QVariant& defaultValue)
{
    return _get(object, path, defaultValue);
}

QVariant QSyncable::get(const QVariantMap &source, const QString &path, const QVariant &defaultValue)
{
    return get(source, path.split("."), defaultValue);
}

QVariant QSyncable::get(const QVariantMap &source, const QStringList &path, const QVariant &defaultValue)
{
    return _get(source, path, defaultValue);
}

void QSyncable::set(QVariantMap &data, const QString &path, const QVariant &value)
{
    return set(data, path.split("."), value);
}

void QSyncable::set(QVariantMap &data, const QStringList &path, const QVariant &value)
{
    QString key = path[0];

    if (path.size() == 1) {
        data[key] = value;
    } else {
        if (!data.contains(key) || !data[key].canConvert<QVariantMap>()) {
            data[key] = QVariantMap();
        }
        QStringList nextPath = path;
        nextPath.removeFirst();
        QVariantMap map = data[key].toMap();
        set(map, nextPath, value);
        data[key] = map;
    }
}

QVariantMap QSyncable::pick(QObject *object, const QStringList &paths)
{
    QVariantMap data;
    foreach (QString path, paths) {
        QVariant value = get(object, path);
        if (value.isNull()) {
            continue;
        }

        if (value.canConvert<QObject*>()) {
            QVariantMap map;
            assign(map, value.value<QObject*>());
            value = map;
        }

        set(data, path, value);
    }
    return data;
}


QVariantMap QSyncable::pick(QVariantMap source, const QStringList &paths)
{
    QVariantMap data;
    foreach (QString path, paths) {
        QVariant value = get(source, path);
        if (value.isNull()) {
            continue;
        }

        if (value.canConvert<QObject*>()) {
            QVariantMap map;
            assign(map, value.value<QObject*>());
            value = map;
        }

        set(data, path, value);
    }
    return data;
}

QVariantMap QSyncable::pick(QVariantMap source, const QVariantMap &paths)
{
    return pick(source, paths.keys());
}

QVariantMap QSyncable::omit(const QVariantMap &source, const QVariantMap &properties)
{

    QMap<QString,QVariant>::const_iterator iter = source.begin();
    QVariantMap result;

    while (iter != source.end()) {

        if (properties.contains(iter.key())) {
            iter++;
            continue;
        }

        QVariant value = source[iter.key()];

        if (value.canConvert<QObject*>()) {
            QVariantMap map;
            assign(map, value.value<QObject*>());
            value = map;
        }

        result[iter.key()] = value;
        iter++;
    }

    return result;
}
