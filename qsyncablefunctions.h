#ifndef QSYNCABLEFUNCTIONS_H
#define QSYNCABLEFUNCTIONS_H

#include <QObject>
#include <QJSValue>
#include <QVariant>

namespace QSyncable {

    /// Assign properties from source object to the destination object.
    void assign(QVariantMap& dest, const QObject*source);

    void assign(QObject* dest, const QVariantMap& source);

    void assign(QObject* dest, const QJSValue& source);

    /// Gets the value at path of object. If the path is not found, the defaultValue is returned.
    /*
     Example:

     get(object, "a.b.c");

    */
    QVariant get(const QObject* source, const QString& path, const QVariant& defaultValue = QVariant());

    QVariant get(const QObject* source, const QStringList& path, const QVariant& defaultValue = QVariant());

    QVariant get(const QVariantMap& source, const QString& path, const QVariant& defaultValue = QVariant());

    QVariant get(const QVariantMap& source, const QStringList& path, const QVariant& defaultValue = QVariant());

    /// Sets the value at path of object. If a portion of path doesn't exist, it's created.
    /*
     Example:

     set(data, "a.b", 3); // data["a"] will be a QVariantMap that contains a key of "b".

     */
    void set(QVariantMap& dest, const QString& path, const QVariant& value);

    void set(QVariantMap& dest, const QStringList& path, const QVariant& value);

    /// Creates an QVariantMap composed of the picked object properties at paths.
    /*
     Example:

         pick(object, QStringList() << "a" << "b.c");

     If a property contains QObject pointer, it will be converted to QVariantMap.

     In case you need to obtain a QObject pointer, please use get().

     */

    QVariantMap pick(QObject* source, const QStringList& paths);

    QVariantMap pick(QVariantMap source, const QStringList& paths);

    QVariantMap pick(QVariantMap source, const QVariantMap& paths);


    /// The opposite of pick(), this method creates an QVariantMap composed of the own properties that are not omitted.
    /*
     If a property contains QObject pointer, it will be converted to QVariantMap.

     In case you need to obtain a QObject pointer, please use get().
     */
    QVariantMap omit(const QVariantMap& source, const QVariantMap& properties);

}

#endif // QSYNCABLEFUNCTIONS_H
