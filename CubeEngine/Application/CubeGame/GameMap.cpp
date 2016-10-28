#include "GameMap.h"
#include "Chunk.h"
namespace tzw {
GameMap * GameMap::m_instance = nullptr;
GameMap::GameMap()
    :m_maxHeight(1),m_mapType(MapType::Noise)
{
    m_plane = new noise::model::Plane(myModule);
}

void GameMap::init(float ratio)
{
    m_ratio = ratio;
    x_offset = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    y_offset = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

GameMap *GameMap::shared()
{
    if(!m_instance)
    {
        m_instance = new GameMap();
    }
    return m_instance;
}

float GameMap::maxHeight() const
{
    return m_maxHeight;
}

void GameMap::setMaxHeight(float maxHeight)
{
    m_maxHeight = maxHeight;
}

double GameMap::getValue(int x, int z)
{

    double value = m_plane->GetValue(x_offset + x*m_ratio,y_offset + z * m_ratio);
    value = value * 0.5 + 0.5;
    if(value*m_maxHeight<= 3)
    {
        return 3;
    }else
    {
        return value*m_maxHeight;
    }

}

bool GameMap::isBlock(Chunk * chunk,int x, int y, int z)
{
    switch(m_mapType)
    {
    case MapType::Noise:
    {
        vec3 worldPos = chunk->getGridPos(x,y,z);
        float height = getValue(worldPos.x,worldPos.z);
        if(worldPos.y<= height)
        {
            return true;
        }else
        {

            return false;
        }
    }
        break;
    case MapType::Plain:
    {
        vec3 worldPos = chunk->getGridPos(x,y,z);
        float height = maxHeight();
        if(worldPos.y<= height)
        {
            return true;
        }else
        {
            return false;
        }
    }
        break;
    default:
        return false;
    }
    return false;
}

bool GameMap::isSurface( vec3 pos)
{
    switch(m_mapType)
    {
    case MapType::Noise:
    {
        float height = getValue(pos.x,pos.z);
        if(pos.y <= int(height))
        {
            return true;
        }else
        {
            return false;
        }
    }
        break;
    case MapType::Plain:
    {
        float height = maxHeight();
        if(pos.y <= int(height))
        {
            return true;
        }else
        {
            return false;
        }
    }
        break;
    default:
        return false;
    }
    return false;
}

float GameMap::getDensity(vec3 pos)
{
    switch(m_mapType)
    {
    case MapType::Noise:
    {
        float height = getValue(pos.x,pos.z);
        if(pos.y <= int(height))
        {
            return fabs(pos.y - height);
        }else
        {
            return -fabs(pos.y - height);
        }
    }
        break;
    case MapType::Plain:
    {
        float height = maxHeight();
        if(pos.y <= int(height))
        {
            return 1;
        }else
        {
            return -1;
        }
    }
        break;
    default:
        return 1;
    }
    return 1;
}

GameMap::MapType GameMap::getMapType() const
{
    return m_mapType;
}

void GameMap::setMapType(const MapType &mapType)
{
    m_mapType = mapType;
}

} // namespace tzw

