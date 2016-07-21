#ifndef TZWS_BUILDING_H
#define TZWS_BUILDING_H
#include "LogicEntity.h"
#include "Currency.h"
namespace tzwS {

class Building : public LogicEntity
{
public:
    Building();
    Building(int money,int food,int admin,int mil,int tech);
    Building(std::string name,int money,int food,int admin,int mil,int tech);
    Currency boost() const;
    unsigned int type() const;
    void setType(unsigned int type);

protected:
    Currency m_boost;
    unsigned int m_type;
};


} // namespace tzwS

#endif // TZWS_BUILDING_H
