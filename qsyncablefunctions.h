#ifndef QSYNCABLEFUNCTIONS_H
#define QSYNCABLEFUNCTIONS_H

#include <QObject>

namespace QSyncable {

    /// Assign properties from source object to the destination objec
    void assign(QVariantMap& dest,
                const QObject* source);

}

#endif // QSYNCABLEFUNCTIONS_H
