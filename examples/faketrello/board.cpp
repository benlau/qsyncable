#include "board.h"

class BoardPriv : public QSharedData
{
public:
    QList<List> lists;
    int nextId;
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

int Board::nextId() const
{
    return d->nextId;
}

void Board::setNextId(int nextId)
{
    d->nextId = nextId;
}

void Board::load()
{
    List list1("List 1");
    list1 << Card("Task 1")
          << Card("Task 2");
    d->lists << list1;
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

