#include "qsyncablefunctions.h"
#include "qsyncableqmlwrapper.h"

QSyncableQmlWrapper::QSyncableQmlWrapper(QObject *parent) : QObject(parent)
{

}

void QSyncableQmlWrapper::assign(QObject *dest, QJSValue source)
{
    return QSyncable::assign(dest, source);
}
