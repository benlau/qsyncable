#ifndef IMMUTABLETYPE1_H
#define IMMUTABLETYPE1_H

#include <QObject>
#include <QSharedDataPointer>
#include <QVariant>

class ImmutableType1Data;

class ImmutableType1
{
    Q_GADGET
    Q_PROPERTY(QString value1 READ value1 WRITE setValue1)

public:
    ImmutableType1();
    ImmutableType1(const ImmutableType1 &);
    ImmutableType1 &operator=(const ImmutableType1 &);
    ~ImmutableType1();

    bool isSharedWith(const ImmutableType1& other) const;

    QString value1() const;

    void setValue1(QString value1);

    Q_INVOKABLE QVariant key() const;

private:
    QSharedDataPointer<ImmutableType1Data> data;
};

#endif // IMMUTABLETYPE1_H
