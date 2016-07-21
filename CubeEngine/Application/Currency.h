#ifndef TZWS_CURRENCY_H
#define TZWS_CURRENCY_H

#include <string>
namespace tzwS {

class Currency
{
public:
    Currency();
    Currency(int money, int food, int admin, int mil, int tech);
    Currency operator + (const Currency & other) const;
    unsigned int &operator [](int index);
    std::string fieldToStr(int index);
    std::string toCompactStr();
    unsigned int money;
    unsigned int food;
    unsigned int admin;
    unsigned int mil;
    unsigned int tech;
};

} // namespace tzwS

#endif // TZWS_CURRENCY_H
