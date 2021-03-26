#ifndef QSPATCHABLE
#define QSPATCHABLE

#include <QVariantMap>

class QSPatchable {
public:
    virtual ~QSPatchable() {}

    virtual void insert(int index, const QVariantList &value) = 0;

    virtual void move(int from, int to, int count) = 0;

    virtual void remove(int i , int count  = 1) = 0;

    virtual void set(int index, QVariantMap dict) = 0;
};

#endif // QSPATCHABLE

