#include "Settlement.h"

namespace tzwS {

Settlement::Settlement()
{

}

Settlement::Settlement(std::string theName)
{
    setName(theName);
}

std::string Settlement::name() const
{
    return m_name;
}

void Settlement::setName(const std::string &name)
{
    m_name = name;
}

} // namespace tzwS
