#include "GameMap.h"
#include "Chunk.h"
namespace tzw {
GameMap * GameMap::m_instance = nullptr;

module::Perlin baseMountainTerrain;

module::Billow baseFlatTerrain;
module::Perlin baseBumpyFlatTerrain;


module::ScaleBias flatTerrain;
module::ScaleBias BumpyFlatTerrain;
module::ScaleBias mountainTerrain;

module::Perlin terrainType;
module::Select finalTerrain;

module::Add finalFlatTerrain;

/// <summary>
/// Initializes a new instance of the <see cref="GameMap"/> class.
/// </summary>
GameMap::GameMap()
    :m_maxHeight(1),m_mapType(MapType::Noise)
{
    m_plane = new noise::model::Plane(myModule);
	myModule.SetPersistence(0.001);

	baseFlatTerrain.SetFrequency(0.001);
	baseFlatTerrain.SetPersistence(0.1);
	flatTerrain.SetSourceModule(0, baseFlatTerrain);
	flatTerrain.SetScale(8.0);
	flatTerrain.SetBias(15.0);

	baseBumpyFlatTerrain.SetFrequency(0.1);
	baseBumpyFlatTerrain.SetPersistence(0.4);

	BumpyFlatTerrain.SetSourceModule(0, baseBumpyFlatTerrain);
	BumpyFlatTerrain.SetScale(0.8);

	finalFlatTerrain.SetSourceModule(0, flatTerrain);
	finalFlatTerrain.SetSourceModule(1, BumpyFlatTerrain);

	baseMountainTerrain.SetSeed(233);
	baseMountainTerrain.SetFrequency(0.015);
	baseMountainTerrain.SetPersistence(0.3);

	mountainTerrain.SetSourceModule(0, baseMountainTerrain);
	mountainTerrain.SetScale(30.0);
	mountainTerrain.SetBias(30.0);

	printf("hello , test");

	finalTerrain.SetSourceModule(0, finalFlatTerrain);
	finalTerrain.SetSourceModule(1, mountainTerrain);

	terrainType.SetFrequency(0.005);

	finalTerrain.SetControlModule(terrainType);
	finalTerrain.SetBounds(0.2, 100.0);
	finalTerrain.SetEdgeFalloff(0.4);
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
	double value = finalTerrain.GetValue(x_offset + x, y_offset + y, z_offset + z);
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
		return height - pos.y;
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

