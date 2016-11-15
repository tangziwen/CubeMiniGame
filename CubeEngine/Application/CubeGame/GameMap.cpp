#include "GameMap.h"
#include "Chunk.h"
namespace tzw {
GameMap * GameMap::m_instance = nullptr;
module::RidgedMulti mountainTerrain;
module::ScaleBias mountainTerrainSelect;
module::Billow baseFlatTerrain;
module::ScaleBias flatTerrain;
module::Perlin terrainType;
module::Blend finalTerrain;

GameMap::GameMap()
    :m_maxHeight(1),m_mapType(MapType::Noise)
{
    m_plane = new noise::model::Plane(myModule);
	myModule.SetPersistence(0.25);
	
	baseFlatTerrain.SetFrequency(0.5);
	baseFlatTerrain.SetOctaveCount(2.0);
	flatTerrain.SetSourceModule(0, baseFlatTerrain);
	flatTerrain.SetScale(1.5);
	flatTerrain.SetBias(10.0);

	terrainType.SetFrequency(0.20);
	terrainType.SetPersistence(0.25);

	//mountainTerrain.SetFrequency(0.5);
	mountainTerrain.SetOctaveCount(4);
	mountainTerrainSelect.SetSourceModule(0, mountainTerrain);
	mountainTerrainSelect.SetScale(6.0);
	mountainTerrainSelect.SetBias(20.0);

	finalTerrain.SetSourceModule(0, flatTerrain);
	finalTerrain.SetSourceModule(1, mountainTerrainSelect);
	finalTerrain.SetSourceModule(2, terrainType);
}

void GameMap::init(float ratio)
{
    m_ratio = ratio;
    x_offset = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    y_offset = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	z_offset = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
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

double GameMap::getValue(float x, float y, float z)
{
	double value = finalTerrain.GetValue(x_offset + x*m_ratio, y_offset + y * m_ratio, z_offset + z * m_ratio); //m_plane->GetModule().GetValue(x_offset + x*m_ratio, y_offset + y * m_ratio, z_offset + z * m_ratio);
    value = value;
	return m_minHeight + value;
}

bool GameMap::isBlock(Chunk * chunk,int x, int y, int z)
{
    switch(m_mapType)
    {
    case MapType::Noise:
    {
        vec3 worldPos = chunk->getGridPos(x,y,z);
        float height = getValue(worldPos.x, worldPos.y, worldPos.z);
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
        float height = getValue(pos.x,pos.y, pos.z);
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
        float height = getValue(pos.x,0.0, pos.z);
		float delta = fabs(pos.y - height);
        if(pos.y <= height)
        {
            return delta;
        }else
        {
            return -delta;
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

void GameMap::setMinHeight(float minHeight)
{
	this->m_minHeight = minHeight;
}

float GameMap::minHeight()
{
	return m_minHeight;
}

} // namespace tzw

