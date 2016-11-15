#ifndef QSYNCABLEQMLWRAPPER_H
#define QSYNCABLEQMLWRAPPER_H

#include <QObject>
#include <QJSValue>

class QSyncableQmlWrapper : public QObject
{
    Q_OBJECT
public:
    explicit QSyncableQmlWrapper(QObject *parent = 0);

signals:

public slots:

    void assign(QObject* dest, QJSValue source);

};

#endif // QSYNCABLEQMLWRAPPER_H
