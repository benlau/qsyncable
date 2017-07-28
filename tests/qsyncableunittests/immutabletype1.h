#ifndef IMMUTABLETYPE1_H
#define IMMUTABLETYPE1_H

#include <QObject>
#include <QSharedDataPointer>
#include <QVariant>

class ImmutableType1Data;

class ImmutableType1
{
    Q_GADGET
    Q_PROPERTY(QString id READ id WRITE setId)
    Q_PROPERTY(QString value READ value WRITE setValue)

public:
    ImmutableType1();
    ImmutableType1(const ImmutableType1 &);
    ImmutableType1 &operator=(const ImmutableType1 &);
    ~ImmutableType1();

    bool isSharedWith(const ImmutableType1& other) const;

    QString id() const;

    void setId(QString id);

    Q_INVOKABLE QString key() const;

    QString value() const;
    void setValue(const QString &value);

private:
    QSharedDataPointer<ImmutableType1Data> data;
};

#endif // IMMUTABLETYPE1_H
