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

    void moveCard(const QString& listUuid,const QString& fromCardUUid, const QString& toCardUuid);

    int indexOfList(const QString& listUuid);

    // NPM: Load persisted board from file.
    void load(const QString &persistFilePath);

    // NPM: Load initial board if persistence file missing or malformed.
    void failsafeLoad();

    QVariantMap toMap() const;

private:
    QSharedDataPointer<BoardPriv> d;
    Card card();
    List list();
};

#endif // BOARD_H
