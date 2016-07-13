#include "MapSystem.h"
#include <stdlib.h>
#include <string.h>
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Engine/Engine.h"
namespace tzwS {

TZW_SINGLETON_IMPL(MapSystem);
MapSystem::MapSystem()
{
    m_width = 25;
    m_height = 25;
    m_cellGraphicsSize = 32;
}

void MapSystem::init()
{
    m_map = (MapCell *)malloc(m_width * m_height * sizeof(MapCell));
    for(size_t i = 0; i< m_height;i++)
    {
        for(size_t j = 0; j < m_width; j++)
        {
            m_map[i*m_width + j] = MapCell();
        }
    }
}

void MapSystem::createMapFromData()
{

}

unsigned int MapSystem::width() const
{
    return m_width;
}

void MapSystem::setWidth(unsigned int width)
{
    m_width = width;
}

unsigned int MapSystem::height() const
{
    return m_height;
}

void MapSystem::setHeight(unsigned int height)
{
    m_height = height;
}

void MapSystem::addCell(int x, int y, MapCell cell)
{
    m_map[m_width * y + x] = cell;
}

void MapSystem::addSettlement(int x, int y, Settlement *settlement)
{
    m_map[m_width * y + x] = MapCell(settlement);
    m_settlementList.push_back(settlement);
}

void MapSystem::initGraphics()
{
    for(size_t i = 0; i< m_height;i++)
    {
        for(size_t j = 0; j < m_width; j++)
        {
            auto cell = m_map[i*m_width + j];
            switch (cell.landType())
            {

            case LAND_TYPE_PLAIN:
            {
                auto sprite = tzw::Sprite::create(tzw::Engine::shared()->getUserPath("Strategy/emptyLand.png"));
                tzw::SceneMgr::shared()->currentScene()->addNode(sprite);
                sprite->setPos2D(j * m_cellGraphicsSize,i * m_cellGraphicsSize);
            }
                break;
            case LAND_TYPE_SETTLEMENT:
            {
                auto sprite = tzw::Sprite::create(tzw::Engine::shared()->getUserPath("Strategy/city.png"));
                tzw::SceneMgr::shared()->currentScene()->addNode(sprite);
                sprite->setPos2D(j * m_cellGraphicsSize,i * m_cellGraphicsSize);
            }
                break;
            case LAND_TYPE_WASTELAND:
            {
                auto sprite = tzw::Sprite::create(tzw::Engine::shared()->getUserPath("Strategy/emptyLand.png"));
                tzw::SceneMgr::shared()->currentScene()->addNode(sprite);
                sprite->setPos2D(j * m_cellGraphicsSize,i * m_cellGraphicsSize);
            }
                break;
            }
        }
    }
}

unsigned int MapSystem::cellGraphicsSize() const
{
    return m_cellGraphicsSize;
}

void MapSystem::setCellGraphicsSize(unsigned int cellGraphicsSize)
{
    m_cellGraphicsSize = cellGraphicsSize;
}

void MapSystem::update()
{
    for(auto settlement : m_settlementList)
    {
        settlement->update();
    }
}

Settlement *MapSystem::getSettlementByName(std::string theName)
{
    for(auto settlement :m_settlementList)
    {
        if(settlement->name() == theName)
        {
            return settlement;
        }
    }
    return nullptr;
}

} // namespace tzwS
