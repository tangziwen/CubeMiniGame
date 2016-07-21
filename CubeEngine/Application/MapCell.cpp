#include "MapCell.h"

namespace tzwS {

MapCell::MapCell()
{
    m_landType = LAND_TYPE_PLAIN;
    m_settlement = nullptr;
}

MapCell::MapCell(Settlement *settlement)
{
    m_landType = LAND_TYPE_SETTLEMENT;
    m_settlement = settlement;
}

int MapCell::landType() const
{
    return m_landType;
}

void MapCell::setLandType(int landType)
{
    m_landType = landType;
}

Settlement *MapCell::settlement() const
{
    return m_settlement;
}

void MapCell::setSettlement(Settlement *settlement)
{
    m_settlement = settlement;
}
} // namespace tzwS
