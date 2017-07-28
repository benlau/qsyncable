#ifndef IMMUTABLETYPE3_H
#define IMMUTABLETYPE3_H

#include <QSharedDataPointer>
#include <QVariant>
#include <QObject>

class ImmutableType3Data;

/// Immutable type with int key

class ImmutableType3
{
    Q_GADGET
    Q_PROPERTY(int value READ value WRITE setValue)

public:

    ImmutableType3();
    ImmutableType3(const ImmutableType3 &);
    ImmutableType3 &operator=(const ImmutableType3 &);
    ~ImmutableType3();

    int value() const;

    void setValue(int value);

    bool isSharedWith(const ImmutableType3& other) const;

    Q_INVOKABLE int key() const;

private:
    QSharedDataPointer<ImmutableType3Data> data;
};

#endif // IMMUTABLETYPE3_H
