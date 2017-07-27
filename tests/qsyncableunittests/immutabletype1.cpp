#include "immutabletype1.h"

class ImmutableType1Data : public QSharedData
{
public:
    ImmutableType1Data() {
        value1 = 0;
    }

    int value1;

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

int ImmutableType1::value1() const
{
    return data->value1;
}

void ImmutableType1::setValue1(int value1)
{
    data->value1 = value1;
}

size_t ImmutableType1::immutableKey() const
{
    return (size_t) data.data();
}
