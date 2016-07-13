#include "Settlement.h"
#include "Hero.h"
namespace tzwS {

Settlement::Settlement()
{
    init();
}

Settlement::Settlement(std::string theName)
{
    init();
    setName(theName);
}

Settlement::Settlement(std::string theName, Hero *ownerHero)
{
    init();
    setName(theName);
    m_owner = ownerHero;
}

Hero *Settlement::owner() const
{
    return m_owner;
}

void Settlement::setOwner(Hero *owner)
{
    m_owner = owner;
}

void Settlement::update()
{
    m_ticks +=1;
    if(m_ticks > 5)
    {
        printf("settlement %s is updating\n",m_name.c_str());
        float totalOutputMoney = T_GETT("Population") * 0.7 + T_GETT("PublicOrder") * 0.3 + T_GETT("Development") * 0.4;
        if(m_owner)
        {
            m_owner->setMoney(m_owner->money() + totalOutputMoney * 0.33);
        }
        m_ticks = 0;
    }
}

void Settlement::init()
{
    m_owner = nullptr;
    m_attr->set("PublicOrder",0.0f);
    m_attr->set("Population",100.0f);
    m_attr->set("Development",2.0f);
    m_attr->set("MillitaryPerformance",100.0f);
    m_attr->set("Defence",100.0f);

    m_ticks = 0;
}

} // namespace tzwS
