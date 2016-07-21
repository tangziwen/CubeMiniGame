#include "BuildingPool.h"

namespace tzwS {
TZW_SINGLETON_IMPL(BuildingPool);
BuildingPool::BuildingPool()
{
    init();
}

unsigned int BuildingPool::insert(Building building)
{
    building.setType(m_buildingList.size());
    m_buildingList.push_back(building);
    return building.type();
}

Building* BuildingPool::getBuildingByIndex(unsigned int index)
{
    return &m_buildingList[index];
}

Building* BuildingPool::getBuildingByName(std::string buildingName)
{
    for( auto & building : m_buildingList)
    {
        if(building.name() == buildingName)
        {
            return &building;
        }
    }
    return &m_buildingList[0];
}

void BuildingPool::init()
{
    insert(Building("empty",0,0,0,0,0));

    insert(Building("market",3,0,0,0,0));

    insert(Building("farm",1,2,0,0,0));

    insert(Building("office",-1,0,3,0,1));

    insert(Building("barracks",-1,-1,1,3,1));
}

unsigned int BuildingPool::getBuildingAmount()
{
    return m_buildingList.size();
}

} // namespace tzwS
