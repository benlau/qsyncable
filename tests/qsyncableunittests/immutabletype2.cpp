#include "immutabletype2.h"

class ImmutableType2Data : public QSharedData
{
public:
    QString value;
};

ImmutableType2::ImmutableType2() : data(new ImmutableType2Data)
{

}

ImmutableType2::ImmutableType2(const ImmutableType2 &rhs) : data(rhs.data)
{

}

ImmutableType2 &ImmutableType2::operator=(const ImmutableType2 &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

ImmutableType2::~ImmutableType2()
{

}

QString ImmutableType2::value() const
{
    return data->value;
}

void ImmutableType2::setValue(const QString &value)
{
    data->value = value;
}

size_t ImmutableType2::immutableKey() const
{
    return (size_t) data.data();
}
