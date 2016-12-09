#include "SettlementCell.h"
#include "External/TUtility/TUtility.h"
#include <algorithm>
#include "Settlement.h"
namespace tzwS {

SettlementCell::SettlementCell()
{
    m_buildingType = 0;
    m_building = nullptr;
    randomGenerate();
    tick = 0;
	m_name = "emptyLand";
}

Settlement *SettlementCell::parent() const
{
    return m_parent;
}

void SettlementCell::setParent(Settlement *parent)
{
    m_parent = parent;
}

unsigned int SettlementCell::buildingType() const
{
    return m_buildingType;
}

void SettlementCell::setBuildingType(unsigned int buildingType)
{
    m_buildingType = buildingType;
}

bool SettlementCell::isEmpty()
{
    return m_buildingType == 0;
}

void SettlementCell::remove()
{
    m_buildingType = 0;
}

void SettlementCell::build(unsigned int buildingID)
{
    m_buildingType = buildingID;
    m_building = BuildingPool::shared()->getBuildingByIndex(buildingID);
}

void SettlementCell::update()
{

    if (m_parent)
    {
        m_parent->setCurrency(m_parent->getCurrency() + m_boost);
    }
}

void SettlementCell::randomGenerate()
{
    std::vector<int> array;
	array.push_back(0);
	array.push_back(1);
	array.push_back(2);
	array.push_back(3);
	array.push_back(4);
    std::random_shuffle(array.begin(),array.end());
    int amount = tzw::TbaseMath::randRangeInt(0,3);
    for(int i = 0;i<amount;i++)
    {
        m_boost[array[i]] += tzw::TbaseMath::randRangeInt(0,2);
    }
}

Currency SettlementCell::boost() const
{
    return m_boost;
}

void SettlementCell::setBoost(const Currency &boost)
{
    m_boost = boost;
}

Building *SettlementCell::building() const
{
    return m_building;
}

void SettlementCell::setBuilding(Building *building)
{
    m_building = building;
}

} // namespace tzwS
