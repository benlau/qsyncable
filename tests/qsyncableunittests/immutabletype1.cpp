#include "immutabletype1.h"

class ImmutableType1Data : public QSharedData
{
public:
    ImmutableType1Data() {
    }

    QString value;

};

ImmutableType1::ImmutableType1() : data(new ImmutableType1Data)
{

}

ImmutableType1::ImmutableType1(const ImmutableType1 &rhs) : data(rhs.data)
{

}

ImmutableType1 &ImmutableType1::operator=(const ImmutableType1 &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

ImmutableType1::~ImmutableType1()
{

}

QString ImmutableType1::value() const
{
    return data->value;
}

void ImmutableType1::setValue(QString value1)
{
    data->value = value1;
}

QString ImmutableType1::key() const
{
    return data->value;
}

bool ImmutableType1::isSharedWith(const ImmutableType1 &other) const
{
    return data == other.data;
}
