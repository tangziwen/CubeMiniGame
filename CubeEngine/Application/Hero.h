#ifndef TZWS_HERO_H
#define TZWS_HERO_H

#include <string>
#include "Settlement.h"
#include "LogicEntity.h"
#include "AttributeList.h"
#include "Currency.h"
namespace tzwS {

class Hero : public LogicEntity
{
public:
    Hero();
    Hero(std::string theName);
    Settlement *location() const;
    void setLocation(Settlement *location);
    int intelligence() const;
    void setIntelligence(int intelligence);

    int strength() const;
    void setStrength(int strength);

    int admin() const;
    void setAdmin(int admin);

    int millitary() const;
    void setMillitary(int millitary);

    int willing() const;
    void setWilling(int willing);

    int charming() const;
    void setCharming(int charming);

    virtual void update();

    void own(std::string settlementName);
    Currency currency;
private:
    void init();
    Settlement * m_location;
    int m_intelligence;
    int m_strength;
    int m_admin;
    int m_millitary;
    int m_willing;
    int m_charming;
};

} // namespace tzwS

#endif // TZWS_HERO_H
