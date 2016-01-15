#include "qschange.h"

class QSChangeData : public QSharedData
{
public:
    QSChangeData() {
        from = 0;
        to = 0;
        type = QSChange::Null;
        count = 0;
    }

    QSChange::Type type;
    QVariantMap data;
    int from;
    int to;
    int count;
};

QSChange::QSChange() : d(new QSChangeData)
{

}

QSChange::QSChange(QSChange::Type type, int from, int to, int count) : d(new QSChangeData)
{
    d->type = type;
    d->from = from;
    d->to = to;
    d->count = count;
}

QSChange::QSChange(const QSChange &rhs) : d(rhs.d)
{

}

QSChange &QSChange::operator=(const QSChange &rhs)
{
    if (this != &rhs)
        d.operator=(rhs.d);
    return *this;
}

QSChange::~QSChange()
{

}

QSChange::Type QSChange::type() const
{
    return d->type;
}

void QSChange::setType(const QSChange::Type &type)
{
    d->type = type;
}

QVariantMap QSChange::data() const
{
    return d->data;
}

void QSChange::setData(const QVariantMap &data)
{
    d->data = data;
}

bool QSChange::operator==(const QSChange &rhs) const
{
    if (d->type != rhs.d->type ||
        d->data != rhs.data() ||
        d->from != rhs.from() ||
        d->to != rhs.to()) {
        return false;
    }

    return true;
}

int QSChange::from() const
{
    return d->from;
}

void QSChange::setFrom(int from)
{
    d->from = from;
}

int QSChange::to() const
{
    return d->to;
}

void QSChange::setTo(int to)
{
    d->to = to;
}

bool QSChange::isNull() const
{
    return d->type == QSChange::Null;
}

int QSChange::count() const
{
    return d->count;
}

void QSChange::setCount(int count)
{
    d->count = count;
}

bool QSChange::canMerge(const QSChange &other) const
{
    bool res = false;

    if (d->type != other.type()) {
        return false;
    }

    if (d->type == QSChange::Remove) {
        if (d->from == other.to() + 1 ||
            d->to == other.from() - 1) {
            res = true;
        }
    }

    return res;
}

QSChange QSChange::merge(const QSChange &other) const
{
    if (!canMerge(other))
        return QSChange();

    QSChange res;

    if (d->type == QSChange::Remove) {
        int from = qMin(d->from, other.from());
        int to = qMax(d->to, other.to());
        res = QSChange(QSChange::Remove, from, to);
    }

    return res;
}

QDebug operator<<(QDebug dbg, const QSChange& change){
    switch (change.type()) {

    case QSChange::Remove:
        dbg.noquote() << QString("Remove from %1 to %2").arg(change.from()).arg(change.to());
        break;

    case QSChange::Move:
        dbg.noquote() << QString("Move from %1 to %2").arg(change.from()).arg(change.to());
        break;

    case QSChange::Insert:
        dbg << "Insert";
        break;

    case QSChange::Update:
        dbg << "Update";
        break;

    default:
        dbg << "Null";
        break;
    }

    return dbg;
}


