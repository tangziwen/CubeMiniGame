#ifndef TZWS_MAPCELL_H
#define TZWS_MAPCELL_H

#define LAND_TYPE_PLAIN 0
#define LAND_TYPE_SETTLEMENT 1
#define LAND_TYPE_WASTELAND 2

#include "Settlement.h"
namespace tzwS {

class MapCell
{
public:
    MapCell();
    MapCell(Settlement * settlement);
    int landType() const;
    void setLandType(int landType);
    Settlement *settlement() const;
    void setSettlement(Settlement *settlement);
private:
    int m_landType;
    Settlement * m_settlement;
};

} // namespace tzwS

#endif // TZWS_MAPCELL_H
