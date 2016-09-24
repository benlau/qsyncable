#include <QMetaProperty>
#include <QtQml>
#include "qsyncablefunctions.h"

void QSyncable::assign(QVariantMap &dest, const QObject *source)
{

    const QMetaObject* meta = source->metaObject();

    for (int i = 0 ; i < meta->propertyCount(); i++) {
        const QMetaProperty property = meta->property(i);
        QVariant value = source->property(property.name());

        if (value.canConvert<QObject*>()) {
            QVariantMap map;
            assign(map, value.value<QObject*>());
            value = map;
        }

        dest[property.name()] = value;
    }

}
