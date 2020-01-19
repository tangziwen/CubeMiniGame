#include "GameMap.h"
#include "Chunk.h"
#include "GameConfig.h"

namespace tzw {
GameMap* GameMap::m_instance = nullptr;

module::Perlin baseMountainTerrain;

module::Billow baseFlatTerrain;
module::Perlin baseBumpyFlatTerrain;

module::ScaleBias flatTerrain;
module::ScaleBias BumpyFlatTerrain;
module::ScaleBias mountainTerrain;

module::Perlin terrainType;
module::Select finalTerrain;

module::Add finalFlatTerrain;

ChunkInfo::ChunkInfo(int theX, int theY, int theZ):isLoaded(false),
	mcPoints(nullptr),x(theX),y(theY),z(theZ)
{
	mcPoints = new vec4[(MAX_BLOCK + 1) * (MAX_BLOCK + 1) * (MAX_BLOCK + 1)];
}

void ChunkInfo::loadChunk(FILE* f)
{
	fread(mcPoints,sizeof(vec4)* (MAX_BLOCK + 1) * (MAX_BLOCK + 1) * (MAX_BLOCK + 1),1,f);
	isLoaded = true;
}

void ChunkInfo::dumpChunk(FILE* f)
{
	int indexX = x;
	int indexY = y;
	int indexZ = z;
	fwrite(&indexX, sizeof(int), 1, f);
	fwrite(&indexY, sizeof(int), 1, f);
	fwrite(&indexZ, sizeof(int), 1, f);

	fwrite(mcPoints, sizeof(vec4) * (MAX_BLOCK + 1) * (MAX_BLOCK + 1) * (MAX_BLOCK + 1), 1, f);
}


GameMap::GameMap()
  : x_offset(0)
  , y_offset(0)
  , z_offset(0)
  , m_maxHeight(1)
  , m_ratio(0)
  , m_minHeight(0)
  , m_mapType(MapType::Noise)
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
  BumpyFlatTerrain.SetScale(0.4);

  finalFlatTerrain.SetSourceModule(0, flatTerrain);
  finalFlatTerrain.SetSourceModule(1, BumpyFlatTerrain);

  baseMountainTerrain.SetSeed(233);
  baseMountainTerrain.SetFrequency(0.015);
  baseMountainTerrain.SetPersistence(0.3);

  mountainTerrain.SetSourceModule(0, baseMountainTerrain);
  mountainTerrain.SetScale(30.0);
  mountainTerrain.SetBias(30.0);

  finalTerrain.SetSourceModule(0, finalFlatTerrain);
  finalTerrain.SetSourceModule(1, mountainTerrain);

  terrainType.SetFrequency(0.005);

  finalTerrain.SetControlModule(terrainType);
  finalTerrain.SetBounds(0.2, 100.0);
  finalTerrain.SetEdgeFalloff(0.4);
}

void GameMap::init(float ratio, int width, int depth, int height)
{
	m_ratio = ratio;
	x_offset = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	y_offset = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	z_offset = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k < depth; k++)
			{
				m_chunkInfoArray[i][j][k] = new ChunkInfo(i, j, k);
			}
		}
	}
}

GameMap*
GameMap::shared()
{
  if (!m_instance) {
    m_instance = new GameMap();
  }
  return m_instance;
}

float
GameMap::maxHeight() const
{
  return m_maxHeight;
}

void
GameMap::setMaxHeight(float maxHeight)
{
  m_maxHeight = maxHeight;
}

double
GameMap::getNoiseValue(float x, float y, float z)
{
  double value =
    finalTerrain.GetValue(x_offset + x, y_offset + y, z_offset + z);
  return m_minHeight + value;
}

bool
GameMap::isBlock(Chunk* chunk, int x, int y, int z)
{
  switch (m_mapType) {
    case MapType::Noise: {
      vec3 worldPos = chunk->getGridPos(x, y, z);
      float height = getNoiseValue(worldPos.x, worldPos.y, worldPos.z);
      if (worldPos.y <= height) {
        return true;
      } else {

        return false;
      }
    }
    case MapType::Plain: {
      vec3 worldPos = chunk->getGridPos(x, y, z);
      float height = maxHeight();
      if (worldPos.y <= height) {
        return true;
      } else {
        return false;
      }
    }
    default:
      return false;
  }
}

bool
GameMap::isSurface(vec3 pos)
{
  switch (m_mapType) {
    case MapType::Noise: {
      float height = getNoiseValue(pos.x, pos.y, pos.z);
      if (pos.y <= int(height)) {
        return true;
      } else {
        return false;
      }
    }
    case MapType::Plain: {
      float height = maxHeight();
      if (pos.y <= int(height)) {
        return true;
      }
      return false;
    }
    default:
      return false;
  }
}

float
GameMap::getDensity(vec3 pos)
{

  switch (m_mapType) {
    case MapType::Noise: {
      static double oldX = -99999999.0;
      static double oldZ = -99999999.0;
      static float oldHeight = 0.0;
      if (fabs(pos.x - oldX) < 0.00001 && fabs(pos.z - oldZ) < 0.00001) {
        return oldHeight - pos.y;
      }
      float height = getNoiseValue(pos.x, 0.0, pos.z);
      float delta = height - pos.y;
      oldX = pos.x;
      oldZ = pos.z;
      oldHeight = height;
      return delta;
    }
    case MapType::Plain: {
      float height = maxHeight();
      return height - pos.y;
    }
    default:
      return 1;
  }
}

GameMap::MapType
GameMap::getMapType() const
{
  return m_mapType;
}

void
GameMap::setMapType(const MapType& mapType)
{
  m_mapType = mapType;
}

void
GameMap::setMinHeight(float minHeight)
{
  this->m_minHeight = minHeight;
}

float
GameMap::minHeight()
{
  return m_minHeight;
}

ChunkInfo * GameMap::getChunkInfo(int x, int y, int z)
{
	return m_chunkInfoArray[x][y][z];
}
} // namespace tzw
