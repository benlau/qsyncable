#ifndef QSYNCABLEFUNCTIONS_H
#define QSYNCABLEFUNCTIONS_H

#include <QObject>

namespace QSyncable {

    /// Assign properties from source object to the destination object.
    /// If proeprties is not empty, it will only assign the specificed properties. Otherwise, it will assign all the properties.
    void assign(QVariantMap& dest,
                const QObject*source, const QStringList properties = QStringList());

}

#endif // QSYNCABLEFUNCTIONS_H
