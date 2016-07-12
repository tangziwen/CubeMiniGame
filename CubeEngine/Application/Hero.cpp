#include "Hero.h"

namespace tzwS {

Hero::Hero()
{

}

std::string Hero::name() const
{
    return m_name;
}

void Hero::setName(const std::string &name)
{
    m_name = name;
}

} // namespace tzwS
