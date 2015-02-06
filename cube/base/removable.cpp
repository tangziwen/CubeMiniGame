#include "removable.h"

Removable::Removable()
{
    m_isRemoved = false;
}
bool Removable::isRemoved() const
{
    return m_isRemoved;
}

void Removable::setIsRemoved(bool isRemoved)
{
    m_isRemoved = isRemoved;
}


