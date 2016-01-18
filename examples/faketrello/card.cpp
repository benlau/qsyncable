#include <QUuid>
#include "card.h"

class CardPriv : public QSharedData
{
public:

    CardPriv() {
        uuid = QUuid::createUuid().toString().remove("{").remove("}");
    }

    QString uuid;
    QString text;
};

Card::Card() : d(new CardPriv)
{

}

Card::Card(QString text) : d(new CardPriv)
{
    d->text = text;

}

Card::Card(const Card &rhs) : d(rhs.d)
{

}

Card &Card::operator=(const Card &rhs)
{
    if (this != &rhs)
        d.operator=(rhs.d);
    return *this;
}

Card::~Card()
{

}

QString Card::uuid() const
{
    return d->uuid;
}

void Card::setUuid(const QString &uuid)
{
    d->uuid = uuid;
}

QString Card::text() const
{
    return d->text;
}

void Card::setText(const QString &text)
{
    d->text = text;
}

QVariantMap Card::toMap() const
{
    QVariantMap res;

    res["uuid"] = d->uuid;
    res["text"] = d->text;

    return res;
}

