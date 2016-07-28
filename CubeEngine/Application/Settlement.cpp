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
    if(m_ticks >= 30)
    {
        for(auto cell : m_cellList)
        {
            cell->update();
        }
        float totalOutputMoney = T_GETT("Population") * 0.1 + T_GETT("PublicOrder") * 0.1 + T_GETT("Development") * 0.1;
        m_currency.money += int(totalOutputMoney);
//        if(m_owner)
//        {
//            m_owner->currency.money += totalOutputMoney * 0.33;
//        }
        m_ticks = 0;
    }
}

SettlementCell *Settlement::getCellIndex(int index)
{
    return m_cellList[index];
}

unsigned int Settlement::getCellSize()
{
    return SETTLEMENT_CELL_SIZE;
}

Currency Settlement::getCurrency() const
{
    return m_currency;
}

void Settlement::setCurrency(const Currency &currency)
{
    m_currency = currency;
}

void Settlement::init()
{
    m_owner = nullptr;
    m_attr->set("PublicOrder",0.0f);
    m_attr->set("Population",100.0f);
    m_attr->set("Development",2.0f);
    m_attr->set("MillitaryPerformance",100.0f);
    m_attr->set("Defence",100.0f);
    for(int i =0;i <SETTLEMENT_CELL_SIZE;i++)
    {
        auto cell = new SettlementCell();
        cell->setParent(this);
        m_cellList.push_back(cell);
    }
    m_ticks = 0;
}

} // namespace tzwS
