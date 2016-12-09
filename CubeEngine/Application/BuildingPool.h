#ifndef TZWS_SETTLEMENTCELLPOOL_H
#define TZWS_SETTLEMENTCELLPOOL_H

#include "../EngineSrc/Engine/EngineDef.h"
#include "Building.h"
#include <vector>
#include <string>
namespace tzwS {

class BuildingPool
{
public:
    BuildingPool();
    TZW_SINGLETON_DECL(BuildingPool);
    unsigned int insert(Building building);
    Building *getBuildingByIndex(unsigned int index);
    Building *getBuildingByName(std::string buildingName);
    void init();
    unsigned int getBuildingAmount();
protected:
    std::vector<Building> m_buildingList;
};

} // namespace tzwS

#endif // TZWS_SETTLEMENTCELLPOOL_H
