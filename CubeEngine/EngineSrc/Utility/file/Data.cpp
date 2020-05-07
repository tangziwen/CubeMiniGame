#include "Data.h"
#include <assert.h>
#include "Utility/log/Log.h"

namespace tzw
{
const Data Data::Null;

Data::Data() :
_bytes(nullptr),
_size(0),mangledForDDS(false)
{
}

Data::Data(Data&& other) :
_bytes(nullptr),
_size(0),
	mangledForDDS(false)
{
    move(other);
}

Data::Data(const Data& other) :
_bytes(nullptr),
_size(0)
{
    tlog("In the copy constructor of Data.");
    copy(other._bytes, other._size);
	mangledForDDS = other.mangledForDDS;
}

Data::~Data()
{
    clear();
}

Data& Data::operator= (const Data& other)
{
    tlog("In the copy assignment of Data.");
    copy(other._bytes, other._size);
    return *this;
}

Data& Data::operator= (Data&& other)
{
    tlog("In the move assignment of Data.");
    move(other);
    return *this;
}

void Data::move(Data& other)
{
    _bytes = other._bytes;
    _size = other._size;
	mangledForDDS = other.mangledForDDS;
    other._bytes = nullptr;
    other._size = 0;
}

bool Data::isNull() const
{
    return (_bytes == nullptr || _size == 0);
}

unsigned char* Data::getBytes() const
{
    return _bytes;
}

std::string Data::getString()
{
    std::string a((char *)_bytes);
    return a;
}

size_t Data::getSize() const
{
    return _size;
}

void Data::copy(const unsigned char* bytes, const size_t size)
{
    clear();

    if (size > 0)
    {
        _size = size;
        _bytes = (unsigned char*)malloc(sizeof(unsigned char) * _size);
		assert(_bytes);
        memcpy(_bytes, bytes, _size);
    }
}

void Data::fastSet(unsigned char* bytes, const size_t size)
{
    _bytes = bytes;
    _size = size;
}

void Data::clear()
{
    free(_bytes);
    _bytes = nullptr;
    _size = 0;
}
}

