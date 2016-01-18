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

    int nextId() const;
    void setNextId(int nextId);

    // Load from file, but now it just load dummy data for demo
    void load();

    QVariantMap toMap() const;

private:
    QSharedDataPointer<BoardPriv> d;
};

#endif // BOARD_H
