#include "Currency.h"

namespace tzwS {

Currency::Currency()
{
    money = 0;
    food = 0;
    admin = 0;
    mil = 0;
    tech = 0;
}

Currency::Currency(int money, int food, int admin, int mil, int tech)
{
    this->money = money;
    this->food = food;
    this->admin = admin;
    this->mil = mil;
    this->tech = tech;
}

Currency Currency::operator +(const Currency &other) const
{
    return Currency(money + other.money,food + other.food,
                    admin + other.admin,mil + other.mil,tech + other.tech);
}

unsigned int &Currency::operator [](int index)
{
    switch(index)
    {
    case 0:
        return money;
        break;
    case 1:
        return food;
        break;
    case 2:
        return admin;
        break;
    case 3:
        return mil;
        break;
    case 4:
        return tech;
        break;
    }
    return money;
}

std::string Currency::fieldToStr(int index)
{
    switch(index)
    {
    case 0:
        return "money";
        break;
    case 1:
        return "food";
        break;
    case 2:
        return "admin";
        break;
    case 3:
        return "mil";
        break;
    case 4:
        return "tech";
        break;
    }
    return "money";
}

std::string Currency::toCompactStr()
{
    std::string result;
    char tmp[128];
    if(this->money != 0)
    {
        sprintf(tmp,"M:%d",money);
        result.append(tmp);
    }
    if(this->food != 0)
    {
        sprintf(tmp,"F:%d",food);
        result.append(tmp);
    }
    if(this->admin != 0)
    {
        sprintf(tmp,"A:%d",admin);
        result.append(tmp);
    }
    if(this->mil != 0)
    {
        sprintf(tmp,"MI:%d",mil);
        result.append(tmp);
    }
    if(this->tech != 0)
    {
        sprintf(tmp,"T:%d",tech);
        result.append(tmp);
    }
    return result;
}

} // namespace tzwS
