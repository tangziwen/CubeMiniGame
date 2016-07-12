#ifndef TZWS_HERO_H
#define TZWS_HERO_H

#include <string>
namespace tzwS {

class Hero
{
public:
    Hero();
    std::string name() const;
    void setName(const std::string &name);
private:
    std::string m_name;
};

} // namespace tzwS

#endif // TZWS_HERO_H
