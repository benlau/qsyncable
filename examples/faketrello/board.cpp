#include <QtCore>
#include "board.h"

class BoardPriv : public QSharedData
{
public:
    BoardPriv() {
        nextCardId = 1;
        nextListId = 1;
    }

    QList<List> lists;
    int nextCardId;
    int nextListId;
};

Board::Board() : d(new BoardPriv)
{

}

Board::Board(const Board &rhs) : d(rhs.d)
{

}

Board &Board::operator=(const Board &rhs)
{
    if (this != &rhs)
        d.operator=(rhs.d);
    return *this;
}

Board::~Board()
{

}

QList<List> Board::lists() const
{
    return d->lists;
}

void Board::setLists(const QList<List> &list)
{
    d->lists = list;
}

int Board::nextCardId() const
{
    return d->nextCardId;
}

void Board::setNextCardId(int nextId)
{
    d->nextCardId = nextId;
}

void Board::addList()
{
    d->lists.append(list());
}

void Board::removeCard(QString listUuid, QString cardUuid)
{
    for (int i = 0 ; i < d->lists.size() ; i++) {
        List list = d->lists.at(i);
        if (list.uuid() == listUuid) {
            list.removeCard(cardUuid);
            d->lists[i] = list;
            break;
        }
    }
}

void Board::load()
{
    List list1 = list();
    list1 << card()
          << card()
          << card();

    List list2 = list();
    list2 << card() << card() << card() << card() << card();

    d->lists << list1 << list2;
}

QVariantMap Board::toMap() const
{
    QVariantMap res;

    QVariantList lists;
    for (int i = 0 ; i < d->lists.size() ; i++) {
        lists << d->lists[i].toMap();
    }

    res["lists"] = lists;

    return res;
}

Card Board::card()
{
    return Card(QString("Card %1").arg(d->nextCardId++));
}

List Board::list()
{
    return List(QString("List %1").arg(d->nextListId++));
}

