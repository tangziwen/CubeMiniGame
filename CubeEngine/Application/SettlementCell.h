#ifndef TZWS_SETTLEMENTCELL_H
#define TZWS_SETTLEMENTCELL_H

#include "LogicEntity.h"
#include "BuildingPool.h"
#include "Currency.h"
namespace tzwS {
class Settlement;
class SettlementCell: public LogicEntity
{
public:
    SettlementCell();

    Settlement *parent() const;
    void setParent(Settlement *parent);

    unsigned int buildingType() const;
    void setBuildingType(unsigned int buildingType);
    bool isEmpty();
    void remove();
    void build(unsigned int buildingID);

    virtual void update();
    void randomGenerate();
    Currency boost() const;
    void setBoost(const Currency &boost);

    Building *building() const;

    void setBuilding(Building *building);

protected:
    int tick;
    Currency m_boost;
    unsigned int m_buildingType;
    Building * m_building;
    Settlement * m_parent;
};

} // namespace tzwS

#endif // TZWS_SETTLEMENTCELL_H
