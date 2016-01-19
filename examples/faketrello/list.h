#ifndef LIST_H
#define LIST_H

#include <QSharedDataPointer>
#include "card.h"

class ListPriv;

class List
{
public:
    List();
    List(const QString& title);
    List(const List &);
    List &operator=(const List &);
    ~List();

    QString uuid() const;
    void setUuid(const QString &uuid);

    QList<Card> cards() const;
    void setCards(const QList<Card> &cards);

    QVariantMap toMap() const;

    QString title() const;

    void setTitle(const QString &title);

    void insertCard(int index, const Card& card);

    void appendCard(const Card& card);

    void removeCard(const QString &uuid);

private:
    QSharedDataPointer<ListPriv> d;

};

List& operator<<(List& list,const Card& card);

#endif // LIST_H
