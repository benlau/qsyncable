#ifndef QSPATCHABLE
#define QSPATCHABLE

#include <QVariantMap>

class QSPatchable {

    // No. of item in row
    virtual int count() const = 0;

    virtual void insert(int index, const QVariantMap& value) = 0;

    virtual void move(int from, int to) = 0;

    virtual void remove(int i , int count  = 1) = 0;

    virtual QVariantMap get(int i) const = 0;

    virtual void setProperty(int index, QString property ,QVariant value) = 0;

    virtual void set(int index,QVariantMap data) = 0;

    /// Apply the changes to a record at index. Only modified value will be set.
    virtual void setProperties(int index, QVariantMap diff) = 0;
};

#endif // QSPATCHABLE

