#include "qschange.h"

class QSChangeData : public QSharedData
{
public:
    QSChange::Type type;
    QVariantMap data;

};

QSChange::QSChange() : d(new QSChangeData)
{

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
    if (d->type != rhs.d->type) {
        return false;
    }

    if (d->data != rhs.data()) {
        return false;
    }

    return true;
}

