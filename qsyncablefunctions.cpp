#include <QMetaProperty>
#include <QtQml>
#include "qsyncablefunctions.h"

void QSyncable::assign(QVariantMap &dest, const QObject *source, const QStringList properties)
{
    const QMetaObject* meta = source->metaObject();

    QMap<QString,bool> index;

    foreach (QString property, properties) {
        index[property] = true;
    }

    for (int i = 0 ; i < meta->propertyCount(); i++) {
        const QMetaProperty property = meta->property(i);
        QString p = property.name();

        if (properties.size() > 0 && !index.contains(p)) {
            continue;
        }

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
