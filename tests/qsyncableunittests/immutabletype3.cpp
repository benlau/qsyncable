#include "immutabletype3.h"

class ImmutableType3Data : public QSharedData
{
public:
    ImmutableType3Data() {
        value = -1;
    }

    int value;
};

ImmutableType3::ImmutableType3() : data(new ImmutableType3Data)
{

}

ImmutableType3::ImmutableType3(const ImmutableType3 &rhs) : data(rhs.data)
{

}

ImmutableType3 &ImmutableType3::operator=(const ImmutableType3 &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

ImmutableType3::~ImmutableType3()
{

}

int ImmutableType3::value() const
{
    return data->value;
}

void ImmutableType3::setValue(int value)
{
    data->value = value;
}

bool ImmutableType3::isSharedWith(const ImmutableType3 &other) const
{
    return data == other.data;
}

int ImmutableType3::key() const
{
    return data->value;
}
