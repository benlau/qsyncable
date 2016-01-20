/* QSyncable Project
   Author: Ben Lau
   License: Apache-2.0
   Web: https://github.com/benlau/qsyncable
*/
#include "qspatch.h"

class QSPatchPriv : public QSharedData
{
public:
    QSPatchPriv() {
        from = 0;
        to = 0;
        type = QSPatch::Null;
        count = 0;
    }

    QSPatch::Type type;
    QVariantList data;
    int from;
    int to;
    int count;
};

QSPatch::QSPatch() : d(new QSPatchPriv)
{

}

QSPatch::QSPatch(QSPatch::Type type, int from, int to, int count, const QVariantMap& data) : d(new QSPatchPriv)
{
    d->type = type;
    d->from = from;
    d->to = to;
    d->count = count;
    d->data.append(data);
}

QSPatch::QSPatch(Type type,int from, int to, int count, const QVariantList& data) : d(new QSPatchPriv) {
    d->type = type;
    d->from = from;
    d->to = to;
    d->count = count;
    d->data = data;
}


QSPatch::QSPatch(const QSPatch &rhs) : d(rhs.d)
{

}

QSPatch &QSPatch::operator=(const QSPatch &rhs)
{
    if (this != &rhs)
        d.operator=(rhs.d);
    return *this;
}

QSPatch::~QSPatch()
{

}

QSPatch::Type QSPatch::type() const
{
    return d->type;
}

void QSPatch::setType(const QSPatch::Type &type)
{
    d->type = type;
}

QVariantList QSPatch::data() const
{
    return d->data;
}

void QSPatch::setData(const QVariantList &data)
{
    d->data = data;
}

void QSPatch::setData(const QVariantMap &data)
{
    d->data.clear();
    d->data.append(data);
}

bool QSPatch::operator==(const QSPatch &rhs) const
{
    if (d->type != rhs.d->type ||
        d->data != rhs.data() ||
        d->from != rhs.from() ||
        d->to != rhs.to() ||
        d->count != rhs.count()) {
        return false;
    }

    return true;
}

int QSPatch::from() const
{
    return d->from;
}

void QSPatch::setFrom(int from)
{
    d->from = from;
}

int QSPatch::to() const
{
    return d->to;
}

void QSPatch::setTo(int to)
{
    d->to = to;
}

bool QSPatch::isNull() const
{
    return d->type == QSPatch::Null;
}

int QSPatch::count() const
{
    return d->count;
}

void QSPatch::setCount(int count)
{
    d->count = count;
}

bool QSPatch::canMerge(const QSPatch &other) const
{
    bool res = false;

    if (d->type != other.type()) {
        return false;
    }

    if (d->type == QSPatch::Remove) {
        if (d->from == other.to() + 1 ||
            d->to == other.from() - 1) {
            res = true;
        }
    } else if (d->type == QSPatch::Move) {
        if (d->from + d->count == other.from()  &&
            d->to + d->count == other.to() ) {
            res = true;
        }
    } else if (d->type == QSPatch::Insert) {
        if (d->from == other.from() ||
            d->to == other.from() - 1) {
            res = true;
        }
    }

    return res;
}

QSPatch QSPatch::merge(const QSPatch &other) const
{
    if (!canMerge(other))
        return QSPatch();

    QSPatch res;

    if (d->type == QSPatch::Remove) {
        int from = qMin(d->from, other.from());
        int to = qMax(d->to, other.to());
        res = QSPatch(QSPatch::Remove, from, to, to - from + 1);
    } else if (d->type == QSPatch::Move) {
        res = *this;
        res.setCount(res.count() + other.count());
    } else if (d->type == QSPatch::Insert) {
        int from = d->from;
        QVariantList data;
        data = d->data;
        data.append(other.data());
        res = QSPatch(QSPatch::Insert, from,
                      from + data.count() - 1, data.count(), data);
    }

    return res;
}

QSPatch QSPatch::createUpdate(int index, const QVariantMap &diff)
{
    return QSPatch(QSPatch::Update, index, index, 1, diff);
}

QDebug operator<<(QDebug dbg, const QSPatch& change){
    switch (change.type()) {

    case QSPatch::Remove:
        dbg << QString("Remove from %1 to %2").arg(change.from()).arg(change.to());
        break;

    case QSPatch::Move:
        dbg << QString("Move from %1 to %2 with %3").arg(change.from()).arg(change.to()).arg(change.count());
        break;

    case QSPatch::Insert:
        dbg << QString("Insert from %1 to %2 with %3").arg(change.from()).arg(change.to()).arg(change.count());
        dbg << change.data();
        break;

    case QSPatch::Update:
        dbg << QString("Update %12").arg(change.from());
        dbg << change.data();
        break;

    default:
        dbg << "Null";
        break;
    }

    return dbg;
}


