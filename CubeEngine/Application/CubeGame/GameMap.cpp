#include "GameMap.h"
#include "Chunk.h"
namespace tzw {
GameMap * GameMap::m_instance = nullptr;
module::Perlin baseMountainTerrain;
module::Perlin baseFlatTerrain;
module::ScaleBias flatTerrain;
module::ScaleBias mountainTerrain;
module::Perlin terrainType;
module::Select finalTerrain;
//module::Blend finalTerrain;

GameMap::GameMap()
    :m_maxHeight(1),m_mapType(MapType::Noise)
{
    m_plane = new noise::model::Plane(myModule);
	myModule.SetPersistence(0.25);

	baseFlatTerrain.SetPersistence(0.4);
	baseFlatTerrain.SetOctaveCount(3);
	baseFlatTerrain.SetFrequency(0.06);

	flatTerrain.SetSourceModule(0, baseFlatTerrain);
	flatTerrain.SetScale(3.0);
	flatTerrain.SetBias(1.5 + 4);

	terrainType.SetFrequency (0.5);
	terrainType.SetPersistence (0.25);

	baseMountainTerrain.SetFrequency(0.0125);
	baseMountainTerrain.SetOctaveCount(8);
	baseMountainTerrain.SetPersistence(0.25);
	baseMountainTerrain.SetLacunarity(2.0);
	mountainTerrain.SetSourceModule(0, baseMountainTerrain);
	mountainTerrain.SetScale(10);
	mountainTerrain.SetBias(5);

	finalTerrain.SetSourceModule(1, flatTerrain);
	finalTerrain.SetSourceModule(0, mountainTerrain);
	finalTerrain.SetControlModule(terrainType);
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

double GameMap::getNoiseValue(float x, float y, float z)
{
	double value = mountainTerrain.GetValue(x_offset + x, y_offset + y, z_offset + z);
	return m_minHeight + value;
}

bool GameMap::isBlock(Chunk * chunk,int x, int y, int z)
{
    switch(m_mapType)
    {
    case MapType::Noise:
    {
        vec3 worldPos = chunk->getGridPos(x,y,z);
        float height = getNoiseValue(worldPos.x, worldPos.y, worldPos.z);
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
        float height = getNoiseValue(pos.x,pos.y, pos.z);
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
		static double oldX = -99999999.0;
		static double oldZ = -99999999.0;
		static float oldHeight = 0.0;
		if (fabs(pos.x - oldX) < 0.00001 && fabs(pos.z - oldZ) < 0.00001)
		{
			return oldHeight - pos.y;
		}
        float height = getNoiseValue(pos.x,0.0, pos.z);
		float delta = height - pos.y;
		oldX = pos.x;
		oldZ = pos.z;
		oldHeight = height;
		return delta;
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
            return -1.0;
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

