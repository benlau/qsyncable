#ifndef QSYNCABLEFUNCTIONS_H
#define QSYNCABLEFUNCTIONS_H

#include <QObject>

namespace QSyncable {

    /// Assign properties from source object to the destination object.
    /// If proeprties is not empty, it will only assign the specificed properties. Otherwise, it will assign all the properties.
    void assign(QVariantMap& dest,
                const QObject*source, const QStringList properties = QStringList());

    void assign(QObject* dest, const QVariantMap& source);


    /// Gets the value at path of object. If the path is not found, the defaultValue is returned.
    /*
     Example:

     get(object, "a.b.c");

    */
    QVariant get(QObject* object, const QString& path, const QVariant& defaultValue = QVariant());

    QVariant get(QObject* object, const QStringList& path, const QVariant& defaultValue = QVariant());

}

#endif // QSYNCABLEFUNCTIONS_H
