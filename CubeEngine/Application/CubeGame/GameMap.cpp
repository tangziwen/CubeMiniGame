#include "GameMap.h"
#include "Chunk.h"
#include "GameConfig.h"

#include "FastNoise/FastNoise.h"
#include <algorithm>
namespace tzw {
GameMap* GameMap::m_instance = nullptr;

FastNoise baseMountainTerrain;
FastNoise hightMountainTerrain;
FastNoise bumpMountainTerrain;

FastNoise baseFlatTerrain;
FastNoise baseBumpyFlatTerrain;

FastNoise terrainType;
FastNoise hillSelector;

FastNoise waterMud;
FastNoise grassRock;
FastNoise grassOrDirt;
void voxelInfo::setV4(vec4 v)
{
	w = v.w;
}

void voxelInfo::setV3(vec3 v)
{
	
}

vec3 voxelInfo::getV3()
{
	int YtimeZ = (MAX_BLOCK + 1) * (MAX_BLOCK + 1);
	int i = index / YtimeZ;
	int w = index % YtimeZ;
	int j = w / (MAX_BLOCK + 1);         // y in N(B)
	int k = w % (MAX_BLOCK + 1);        // z in N(C)
	return vec3(i * BLOCK_SIZE, j * BLOCK_SIZE, -1 * k * BLOCK_SIZE);
}

vec4 voxelInfo::getV()
{
	auto v = getV3();
	return vec4(v.x, v.y, v.z, w);
}

voxelInfo::voxelInfo(float _x, float _y, float _z, float _w)
{
	w = _w;
}

voxelInfo::voxelInfo()
{
	
}

void voxelInfo::setMat(char mat1, char mat2, char mat3, vec3 blendFactor)
{
	matInfo.matIndex1 = mat1;
	matInfo.matIndex2 = mat2;
	matInfo.matIndex3 = mat3;
	matInfo.matBlendFactor = blendFactor;
}

ChunkInfo::ChunkInfo(int theX, int theY, int theZ):isLoaded(false),
                                                   mcPoints(nullptr),x(theX),y(theY),z(theZ),isEdit(false)
{
}

void ChunkInfo::loadChunk(FILE* f)
{
	initData();
	fread(mcPoints,sizeof(voxelInfo)* (MAX_BLOCK + 1) * (MAX_BLOCK + 1) * (MAX_BLOCK + 1),1,f);
	isLoaded = true;
	isEdit = true;
}

void ChunkInfo::dumpChunk(FILE* f)
{
	int indexX = x;
	int indexY = y;
	int indexZ = z;
	fwrite(&indexX, sizeof(int), 1, f);
	fwrite(&indexY, sizeof(int), 1, f);
	fwrite(&indexZ, sizeof(int), 1, f);

	fwrite(mcPoints, sizeof(voxelInfo) * (MAX_BLOCK + 1) * (MAX_BLOCK + 1) * (MAX_BLOCK + 1), 1, f);
}

void ChunkInfo::initData()
{
	mcPoints = new voxelInfo[(MAX_BLOCK + 1) * (MAX_BLOCK + 1) * (MAX_BLOCK + 1)];
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
	baseFlatTerrain.SetFractalOctaves(20);
	baseFlatTerrain.SetNoiseType(FastNoise::PerlinFractal);
	baseFlatTerrain.SetFractalType(FastNoise::Billow);
	//flatTerrain.SetSourceModule(0, baseFlatTerrain);
	//flatTerrain.SetScale(8.0);
	//flatTerrain.SetBias(15.0);

	baseBumpyFlatTerrain.SetFrequency(0.1);
	baseBumpyFlatTerrain.SetFractalOctaves(10);
	baseBumpyFlatTerrain.SetNoiseType(FastNoise::PerlinFractal);
	//baseBumpyFlatTerrain.SetPersistence(0.25);

	//BumpyFlatTerrain.SetSourceModule(0, baseBumpyFlatTerrain);
	//BumpyFlatTerrain.SetScale(0.6);


	baseMountainTerrain.SetSeed(233);
	baseMountainTerrain.SetFrequency(0.018);
	baseMountainTerrain.SetFractalOctaves(15);
	baseMountainTerrain.SetNoiseType(FastNoise::PerlinFractal);

	hightMountainTerrain.SetSeed(100);
	hightMountainTerrain.SetFrequency(0.022);
	hightMountainTerrain.SetFractalOctaves(15);
	hightMountainTerrain.SetFractalLacunarity(2.5);
	hightMountainTerrain.SetNoiseType(FastNoise::PerlinFractal);


	terrainType.SetFrequency(0.01);
	terrainType.SetNoiseType(FastNoise::PerlinFractal);

	hillSelector.SetFrequency(0.005);
	
	waterMud.SetFrequency(0.5);
	grassRock.SetFrequency(0.1);
	grassRock.SetSeed(200);
	grassOrDirt.SetFrequency(0.07);
	grassOrDirt.SetNoiseType(FastNoise::SimplexFractal);
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

	
    VegetationBatInfo lod0(VegetationType::ModelType, "treeTest/tzwTree.tzw");
	VegetationBatInfo lod1(VegetationType::ModelType, "treeTest/tzwTree_lod1.tzw");
	VegetationBatInfo lod2(VegetationType::QUAD_TRI, "tzwTree_lod2.png");
	//reg Tree class
	m_treeID = Tree::shared()->regVegetation(&lod0, &lod1, &lod2);
	
	// m_treeID = Tree::shared()->regVegetation(VegetationType::QUAD_TRI, "Texture/grass.tga");
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
	//double value = finalTerrain.GetValue(x_offset + x, y_offset + y, z_offset + z);
	auto finalFlatTerrain = baseFlatTerrain.GetNoise(x_offset + x, y_offset + y, z_offset + z)* 8 + 8 + baseBumpyFlatTerrain.GetNoise(x_offset + x, y_offset + y, z_offset + z) * 0.6;
	auto mountainTerrain = baseMountainTerrain.GetNoise(x_offset + x, y_offset + y, z_offset + z) * 16 + 16;
	auto highHillTerrain = hightMountainTerrain.GetNoise(x_offset + x, y_offset + y, z_offset + z) * 28 + 28.0;
	auto finalMountainTerrain = edgeFallOffSelect(0.5, 100, 0.2, mountainTerrain, highHillTerrain, hillSelector.GetNoise(x_offset + x, y_offset + y, z_offset + z));
	double value = edgeFallOffSelect(0.05, 1.0, 0.27, finalFlatTerrain, finalMountainTerrain, terrainType.GetNoise(x_offset + x, y_offset + y, z_offset + z));//
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
      float delta = std::clamp (height - pos.y, -1.f, 1.f);
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

float GameMap::getHeight(vec2 posXZ)
{
	return getNoiseValue(posXZ.x, 0.0, posXZ.y);
}

vec3 GameMap::getNormal(vec2 posXZ)
{
	vec2 posZ = vec2(posXZ.x, posXZ.y - BLOCK_SIZE);
	vec2 posX = vec2(posXZ.x + BLOCK_SIZE, posXZ.y);

	vec3 p0 = vec3(posXZ.x, getHeight(posXZ), posXZ.y);
	vec3 p1 = vec3(posX.x, getHeight(posX), posX.y);
	vec3 p2 = vec3(posZ.x, getHeight(posZ), posZ.y);

	vec3 p1p0 = p1 - p0;
	vec3 p2p0 = p2 - p0;

	vec3 n = vec3::CrossProduct(p1p0, p2p0).normalized();
	return n;
}

int GameMap::getMat(vec3 pos, float slope)
{
	int mat = 15;//default is dirt material

	if(slope > 0.16)//cliff
	{
		mat = 5;
	}else// or plane
	{
		float grassOrDirtVal = grassOrDirt.GetNoise(pos.x, pos.y, pos.z);
		if(grassOrDirtVal> - 0.2)//it is Grass
		{
			float grassRockVal = grassRock.GetValue(pos.x, pos.y, pos.z);
			mat = 12;//default is grass
			if(grassRockVal > 0.5)
			{
				mat = 3;//Grass with Rock
			}
            else
            {
	            mat = 12;
            }
		}
		else // dirt
		{
			mat = 15;//dirt
			float waterMudVal = waterMud.GetValue(pos.x, pos.y, pos.z);
			if(waterMudVal > 0.7)
			{
				mat = 8;//dirt with water
			}
		}
	}
	//Mud & dirt
	return mat;
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
  static double LinearInterp (double n0, double n1, double a)
  {
    return ((1.0 - a) * n0) + (a * n1);
  }

  /// Maps a value onto a cubic S-curve.
  ///
  /// @param a The value to map onto a cubic S-curve.
  ///
  /// @returns The mapped value.
  ///
  /// @a a should range from 0.0 to 1.0.
  ///
  /// The derivitive of a cubic S-curve is zero at @a a = 0.0 and @a a =
  /// 1.0
  static double SCurve3 (double a)
  {
    return (a * a * (3.0 - 2.0 * a));
  }
float GameMap::edgeFallOffSelect(float lowBound, float upBound, float edgeVal, float val1, float val2, float selectVal)
{

    if (selectVal < (lowBound - edgeVal)) {
      // The output value from the control module is below the selector
      // threshold; return the output value from the first source module.
      return val1;

    } else if (selectVal < (lowBound + edgeVal)) 
	{
      // The output value from the control module is near the lower end of the
      // selector threshold and within the smooth curve. Interpolate between
      // the output values from the first and second source modules.
      double lowerCurve = (lowBound - edgeVal);
      double upperCurve = (lowBound + edgeVal);
      float alpha = SCurve3 (
        (selectVal - lowerCurve) / (upperCurve - lowerCurve));
      return LinearInterp (val1,
        val2,
        alpha);

    } else if (selectVal < (upBound - edgeVal)) {
      // The output value from the control module is within the selector
      // threshold; return the output value from the second source module.
      return val2;

    } else if (selectVal < (upBound + edgeVal)) {
      // The output value from the control module is near the upper end of the
      // selector threshold and within the smooth curve. Interpolate between
      // the output values from the first and second source modules.
      double lowerCurve = (upBound - edgeVal);
      double upperCurve = (upBound + edgeVal);
      float alpha = SCurve3 (
        (selectVal - lowerCurve) / (upperCurve - lowerCurve));
      return LinearInterp (val2,
        val1,
        alpha);

    } else {
      // Output value from the control module is above the selector threshold;
      // return the output value from the first source module.
      return val1;
    }
}

int GameMap::getTreeId()
{
	return m_treeID;
}
} // namespace tzw
