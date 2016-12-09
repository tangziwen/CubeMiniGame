#ifndef TZWS_MAPSYSTEM_H
#define TZWS_MAPSYSTEM_H
#include "../EngineSrc/Engine/EngineDef.h"

#include "MapCell.h"
#include <vector>
#include "../EngineSrc/Event/Event.h"
#include "../EngineSrc/Event/EventMgr.h"
#include "../EngineSrc/2D/Sprite.h"
namespace tzwS {

class MapSystem : public tzw::EventListener
{
public:
    MapSystem();
    void init();
    void createMapFromData();

    unsigned int width() const;
    void setWidth(unsigned int width);

    unsigned int height() const;
    void setHeight(unsigned int height);
    void addCell(int x,int y,MapCell cell);
    void addSettlement(int x,int y,Settlement * settlement);
    void initGraphics();
    unsigned int cellGraphicsSize() const;
    void setCellGraphicsSize(unsigned int cellGraphicsSize);
    void update();
    virtual bool onMouseRelease(int button,tzw::vec2 pos);
    Settlement * getSettlementByName(std::string theName);
    MapCell *getCurrentSelectedCell() const;

private:
    MapCell * m_map;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_cellGraphicsSize;
    MapCell * m_currentSelectedCell;
    std::vector<Settlement *> m_settlementList;
    std::vector<tzw::Sprite *> m_spriteList;
    TZW_SINGLETON_DECL(MapSystem);
};

} // namespace tzwS

#endif // TZWS_MAPSYSTEM_H
