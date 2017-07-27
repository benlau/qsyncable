#ifndef IMMUTABLETYPE1_H
#define IMMUTABLETYPE1_H

#include <QObject>
#include <QSharedDataPointer>

class ImmutableType1Data;

class ImmutableType1
{
    Q_GADGET
    Q_PROPERTY(int value1 READ value1 WRITE setValue1)

public:
    ImmutableType1();
    ImmutableType1(const ImmutableType1 &);
    ImmutableType1 &operator=(const ImmutableType1 &);
    ~ImmutableType1();


    size_t immutableKey() const;

    int value1() const;

    void setValue1(int value1);

private:
    QSharedDataPointer<ImmutableType1Data> data;
};

#endif // IMMUTABLETYPE1_H
