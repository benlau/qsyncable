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
