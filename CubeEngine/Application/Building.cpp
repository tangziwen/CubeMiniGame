#include "Building.h"

namespace tzwS {

Building::Building()
{

}

Building::Building(int money, int food, int admin, int mil, int tech)
{
    m_boost = Currency(money,food,admin,mil,tech);
}

Building::Building(std::string name, int money, int food, int admin, int mil, int tech)
{
    m_name = name;
    m_boost = Currency(money,food,admin,mil,tech);
}

Currency Building::boost() const
{
    return m_boost;
}

unsigned int Building::type() const
{
    return m_type;
}

void Building::setType(unsigned int type)
{
    m_type = type;
}

} // namespace tzwS
