#ifndef IMMUTABLETYPE2_H
#define IMMUTABLETYPE2_H

#include <QSharedDataPointer>
#include <QObject>

class ImmutableType2Data;

class ImmutableType2
{
    Q_GADGET
    Q_PROPERTY(QString value READ value WRITE setValue)
public:
    ImmutableType2();
    ImmutableType2(const ImmutableType2 &);
    ImmutableType2 &operator=(const ImmutableType2 &);
    ~ImmutableType2();

    QString value() const;
    void setValue(const QString &value);

    size_t immutableKey() const;

private:
    QSharedDataPointer<ImmutableType2Data> data;
};

#endif // IMMUTABLETYPE2_H
