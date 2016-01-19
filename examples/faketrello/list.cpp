#include <QtCore>
#include <QUuid>
#include "list.h"

class ListPriv : public QSharedData
{
public:
    ListPriv() {
        uuid = QUuid::createUuid().toString().remove("{").remove("}");
    }

    QString uuid;
    QList<Card> cards;
    QString title;
};

List::List() : d(new ListPriv)
{

}

List::List(const QString &title) :d(new ListPriv)
{
    d->title = title;
}

List::List(const List &rhs) : d(rhs.d)
{

}

List &List::operator=(const List &rhs)
{
    if (this != &rhs)
        d.operator=(rhs.d);
    return *this;
}

List::~List()
{

}

QString List::uuid() const
{
    return d->uuid;
}

void List::setUuid(const QString &uuid)
{
    d->uuid = uuid;
}

QList<Card> List::cards() const
{
    return d->cards;
}

void List::setCards(const QList<Card> &cards)
{
    d->cards = cards;
}

QVariantMap List::toMap() const
{
    QVariantMap res;

    res["uuid"] = d->uuid;
    res["title"] = d->title;

    QVariantList cards;

    for (int i = 0 ; i < d->cards.size() ; i++) {
        cards << d->cards[i].toMap();
    }

    res["cards"] = cards;


    return res;
}

QString List::title() const
{
    return d->title;
}

void List::setTitle(const QString &title)
{
    d->title = title;
}

void List::insertCard(int index, const Card &card)
{
    d->cards.insert(index, card);
}

void List::appendCard(const Card &card)
{
    d->cards.append(card);
}

void List::removeCard(const QString &uuid)
{
    for (int i = 0 ; i < d->cards.size() ; i++) {
        Card card = d->cards.at(i);
        if (card.uuid() == uuid) {
            QList<Card> cards = d->cards;
            cards.removeAt(i);
            d->cards = cards;
            break;
        }
    }
}

List& operator<<(List& list,const Card& card) {
    list.appendCard(card);
    return list;
}
