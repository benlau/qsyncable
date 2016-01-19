#ifndef BOARD_H
#define BOARD_H

#include <QSharedDataPointer>
#include "list.h"

class BoardPriv;

class Board
{
public:
    Board();
    Board(const Board &);
    Board &operator=(const Board &);
    ~Board();

    QList<List> lists() const;
    void setLists(const QList<List> &lists);

    int nextCardId() const;
    void setNextCardId(int nextCardId);

    void addList();

    void removeCard(const QString& listUuid, const QString& cardUUid);

    void addCard(const QString& listUuid);

    // Load from file, but now it just load dummy data for demo
    void load();

    QVariantMap toMap() const;

private:
    QSharedDataPointer<BoardPriv> d;
    Card card();
    List list();
};

#endif // BOARD_H
