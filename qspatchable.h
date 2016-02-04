#ifndef QSPATCHABLE
#define QSPATCHABLE

#include <QVariantMap>

class QSPatchable {
public:

    virtual void insert(int index, const QVariantList &value) = 0;

    virtual void move(int from, int to, int count) = 0;

    virtual void remove(int i , int count  = 1) = 0;

    /// Apply the changes to a record at index. Only modified value will be set.
    virtual void setProperties(int index, QVariantMap diff) = 0;
};

#endif // QSPATCHABLE

