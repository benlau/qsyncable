#ifndef CARD_H
#define CARD_H

#include <QSharedDataPointer>
#include <QVariantMap>

class CardPriv;

class Card
{
public:
    Card();
    Card(QString text);
    Card(const Card &);
    Card &operator=(const Card &);
    ~Card();

    QString uuid() const;
    void setUuid(const QString &uuid);

    QString text() const;
    void setText(const QString &text);

    QVariantMap toMap() const;

private:
    QSharedDataPointer<CardPriv> d;

};

#endif // CARD_H
