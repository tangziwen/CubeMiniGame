#ifndef TZWS_SETTLEMENT_H
#define TZWS_SETTLEMENT_H

#include <string>
#include "AttributeList.h"
#include "LogicEntity.h"
namespace tzwS {
class Hero;
class Settlement : public LogicEntity
{
public:
    Settlement();
    Settlement(std::string theName);
    Settlement(std::string theName, Hero * ownerHero);
    Hero *owner() const;
    void setOwner(Hero *owner);

    virtual void update();
private:
    void init();
    Hero * m_owner;
    int m_ticks;
};

} // namespace tzwS

#endif // TZWS_SETTLEMENT_H
