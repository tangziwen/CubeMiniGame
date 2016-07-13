#include "Hero.h"

#include "MapSystem.h"
namespace tzwS {

Hero::Hero()
{
    init();
}

Hero::Hero(std::string theName)
{
    init();
    m_name = theName;
}

Settlement *Hero::location() const
{
    return m_location;
}

void Hero::setLocation(Settlement *location)
{
    m_location = location;
}

int Hero::intelligence() const
{
    return m_intelligence;
}

void Hero::setIntelligence(int intelligence)
{
    m_intelligence = intelligence;
}

int Hero::strength() const
{
    return m_strength;
}

void Hero::setStrength(int strength)
{
    m_strength = strength;
}

int Hero::admin() const
{
    return m_admin;
}

void Hero::setAdmin(int admin)
{
    m_admin = admin;
}

int Hero::millitary() const
{
    return m_millitary;
}

void Hero::setMillitary(int millitary)
{
    m_millitary = millitary;
}

int Hero::willing() const
{
    return m_willing;
}

void Hero::setWilling(int willing)
{
    m_willing = willing;
}

int Hero::charming() const
{
    return m_charming;
}

void Hero::setCharming(int charming)
{
    m_charming = charming;
}

int Hero::money() const
{
    return m_money;
}

void Hero::setMoney(int money)
{
    m_money = money;
}

void Hero::update()
{
    printf("the hero %s, his money is %d\n",m_name.c_str(),m_money);
}

void Hero::own(std::string settlementName)
{
    auto settle = MapSystem::shared()->getSettlementByName(settlementName);
    if(settle != nullptr)
    {
        settle->setOwner(this);
    }
}

void Hero::init()
{
    m_location = nullptr;
    m_money = 0;
}

} // namespace tzwS
