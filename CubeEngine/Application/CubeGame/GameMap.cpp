#include "GameMap.h"
#include "GameConfig.h"

#include "FastNoise/FastNoise.h"
#include <algorithm>
#include "CropSystem.h"
namespace tzw {
GameMap* GameMap::m_instance = nullptr;

namespace {

int ceilDiv(int value, int divisor)
{
	return (value + divisor - 1) / divisor;
}

unsigned char proceduralDensityAt(GameMap& map, int sampleX, int sampleY, int sampleZ)
{
	if (!map.isVoxelInDomain(sampleX, sampleY, sampleZ))
	{
		return 255;
	}

	const vec3 mapOffset = map.getMapOffset();
	auto targetH = map.getHeight(
		vec2(sampleX * BLOCK_SIZE, sampleZ * BLOCK_SIZE) + vec2(mapOffset.x, mapOffset.z));
	auto currH = sampleY * BLOCK_SIZE;
	float delta = std::clamp((currH - targetH) * 0.1f, -1.f, 1.f);
	return static_cast<unsigned char>((delta * 0.5f + 0.5f) * 255.f);
}

} // namespace

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
	abort();//do not use this function!
	return vec3(0, 0, 0);
	/*
	int YtimeZ = (MAX_BLOCK + 1) * (MAX_BLOCK + 1);
	int i = index / YtimeZ;
	int w = index % YtimeZ;
	int j = w / (MAX_BLOCK + 1);         // y in N(B)
	int k = w % (MAX_BLOCK + 1);        // z in N(C)
	return vec3(i * BLOCK_SIZE, j * BLOCK_SIZE, -1 * k * BLOCK_SIZE);
	*/
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
	SET_HIGH_FOUR_BIT(matInfo.matBlendFactor, std::clamp(static_cast<int>(blendFactor.x * 15.f), 0 , 15));
	SET_LOW_FOUR_BIT( matInfo.matBlendFactor, std::clamp(static_cast<int>(blendFactor.y * 15.f), 0 , 15));
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
	m_mapOffset = vec3(GAME_MAP_WIDTH_VOXELS * BLOCK_SIZE / -2.0f, 0,
		GAME_MAP_DEPTH_VOXELS * BLOCK_SIZE / -2.0f);
}

void GameMap::init(float ratio, int width, int depth, int height)
{
	m_ratio = ratio;
	x_offset = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	y_offset = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	z_offset = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

	mapBufferSize_X = ceilDiv(GAME_MAP_WIDTH_VOXELS, GAME_MAX_BUFFER_SIZE);
	mapBufferSize_Y = ceilDiv(GAME_MAP_HEIGHT_VOXELS, GAME_MAX_BUFFER_SIZE);
	mapBufferSize_Z = ceilDiv(GAME_MAP_DEPTH_VOXELS, GAME_MAX_BUFFER_SIZE);
	m_totalBuffer = new GameMapBuffer[(mapBufferSize_X) * (mapBufferSize_Y) * (mapBufferSize_Z)];


	{
	    VegetationBatInfo lod0(VegetationType::ModelType, "treeTest/tzwTree.tzw");
		VegetationBatInfo lod1(VegetationType::ModelType, "treeTest/tzwTreeLod1.tzw");
		VegetationBatInfo lod2(VegetationType::QUAD, "treeTest/treeLod2.png", vec2(10, 10));

		auto info = new VegetationInfo("SimpleTree");
		info->init(&lod0, &lod1, &lod2);
		m_treeID = FoliageSystem::shared()->regVegetation(info);
	}

	{
	    VegetationBatInfo lod0(VegetationType::QUAD_TRI, "Texture/grass_billboard.tga");
		VegetationBatInfo lod1(VegetationType::QUAD_TRI, "Texture/grass_billboard.tga");
		VegetationBatInfo lod2(VegetationType::QUAD_TRI, "Texture/grass_billboard.tga");

		auto info = new VegetationInfo("SimpleGrass");
		info->init(&lod0, &lod1, &lod2);
		m_grassID = FoliageSystem::shared()->regVegetation(info);
	}

	CropSystem::shared()->initFromFile();

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
	auto finalFlatTerrain = baseFlatTerrain.GetNoise(x_offset + x, y_offset + y, z_offset + z)* 8 + 8 + baseBumpyFlatTerrain.GetNoise(x_offset + x, y_offset + y, z_offset + z) * 0.1;
	auto mountainTerrain = baseMountainTerrain.GetNoise(x_offset + x, y_offset + y, z_offset + z) * 16 + 16;
	auto highHillTerrain = hightMountainTerrain.GetNoise(x_offset + x, y_offset + y, z_offset + z) * 28 + 28.0;
	auto finalMountainTerrain = edgeFallOffSelect(0.5, 100, 0.2, mountainTerrain, highHillTerrain, hillSelector.GetNoise(x_offset + x, y_offset + y, z_offset + z));
	double value = edgeFallOffSelect(0.25, 1.0, 0.27, finalFlatTerrain, finalMountainTerrain, terrainType.GetNoise(x_offset + x, y_offset + y, z_offset + z));//
	return m_minHeight + value;	
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

voxelInfo GameMap::sampleVoxel(int x, int y, int z)
{
	if (!isVoxelInDomain(x, y, z))
	{
		return makeOutsideVoxel();
	}

	const int buffIndex = pageIndexForVoxel(x, y, z);
	if (isPageAllocated(buffIndex))
	{
		return m_totalBuffer[buffIndex].m_buff[localIndexInPage(x, y, z)];
	}

	return sampleProceduralVoxel(x, y, z);
}

voxelInfo GameMap::sampleProceduralVoxel(int x, int y, int z)
{
	if (!isVoxelInDomain(x, y, z))
	{
		return makeOutsideVoxel();
	}
	voxelInfo result;
	result.w = proceduralDensityAt(*this, x, y, z);
	auto x1 = proceduralDensityAt(*this, x - 1, y, z);
	auto x2 = proceduralDensityAt(*this, x + 1, y, z);
	auto y1 = proceduralDensityAt(*this, x, y - 1, z);
	auto y2 = proceduralDensityAt(*this, x, y + 1, z);
	auto z1 = proceduralDensityAt(*this, x, y, z - 1);
	auto z2 = proceduralDensityAt(*this, x, y, z + 1);
	auto gradientVec = vec3(x1 - x2, y1 - y2, z1 - z2);
	float slope = 0.0f;
	if (gradientVec.length() > 0.0001f)
	{
		slope = 1.0 - std::clamp(
			vec3::DotProduct(gradientVec.normalized(), vec3(0, -1, 0)), 0.0f, 1.0f);
	}
	vec3 samplePos(x * BLOCK_SIZE, y * BLOCK_SIZE, z * BLOCK_SIZE);
	auto matID = getMat(samplePos, slope);
	result.setMat(matID, 0, 0, vec3(1, 0, 0));
	return result;
}

unsigned char GameMap::getDensity(vec3 pos)
{

  switch (m_mapType) {
    case MapType::Noise: {
      static double oldX = -99999999.0;
      static double oldZ = -99999999.0;
      static float oldHeight = 0.0;
      /*
      if (fabs(pos.x - oldX) < 0.00001 && fabs(pos.z - oldZ) < 0.00001) {
      	auto w = std::clamp ((pos.y - oldHeight) * 0.1f , -1.f, 1.f);
        return (w * 0.5f + 0.5f) * 255.f;
      }
      */
      float detectSize = 0.5;
      float height = getNoiseValue(pos.x, 0.0, pos.z);
      /*
      float h1 = getNoiseValue(pos.x - detectSize, 0.0, pos.z);
      float h2 = getNoiseValue(pos.x, 0.0, pos.z - detectSize);
      float h3 = getNoiseValue(pos.x + detectSize, 0.0, pos.z);
      float h4 = getNoiseValue(pos.x, 0.0, pos.z + detectSize);
      float density = 0;
      */
      float delta = std::clamp ((pos.y - height)  * 0.2f, -1.f, 1.f);
      oldX = pos.x;
      oldZ = pos.z;
      oldHeight = height;
      return (delta * 0.5f + 0.5f) * 255.f;
    }
    case MapType::Plain: {
      float height = maxHeight();
      return height - pos.y;
    }
    default:
      return 1;
  }
}

unsigned char GameMap::getVoxelW(int x, int y, int z)
{
	return sampleVoxel(x, y, z).w;
}

voxelInfo*
GameMap::getVoxel(int x, int y, int z)
{
	voxelInfo* voxel = ensureVoxelForWrite(x, y, z);
	if (voxel)
	{
		m_totalBuffer[pageIndexForVoxel(x, y, z)].isEdit = true;
	}
	return voxel;
}

void GameMap::setVoxel(int x, int y, int z, unsigned char w)
{
	writeVoxelDensity(x, y, z, w);
}

vec3 GameMap::voxelToBuffWorldPos(int x, int y, int z)
{
    int buffIDX = (x/GAME_MAX_BUFFER_SIZE);
    int buffIDY = (y/GAME_MAX_BUFFER_SIZE);
    int buffIDZ = (z/GAME_MAX_BUFFER_SIZE);
    return vec3((buffIDX * GAME_MAX_BUFFER_SIZE)  * BLOCK_SIZE, (buffIDY * GAME_MAX_BUFFER_SIZE)  * BLOCK_SIZE, (buffIDZ * GAME_MAX_BUFFER_SIZE)  * BLOCK_SIZE);
}

vec3 GameMap::voxelToWorldPos(int x, int y, int z)
{
	return vec3((x)  * BLOCK_SIZE, (y )  * BLOCK_SIZE, (z)  * BLOCK_SIZE) + m_mapOffset;
}

vec3 GameMap::worldPosToVoxelPos(vec3 pos)
{
	pos -= m_mapOffset;
	return vec3(int(pos.x / BLOCK_SIZE), int(pos.y / BLOCK_SIZE),int(pos.z / BLOCK_SIZE));
}

float GameMap::getHeight(vec2 posXZ)
{
	return getNoiseValue(posXZ.x, 0.0, posXZ.y) + 32.f;
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

	if(slope > 0.1)//cliff
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

int GameMap::getGrassId()
{
	return m_grassID;
}

vec2 GameMap::getCenterOfMap()
{
	//float x = (mapBufferSize_X * GAME_MAX_BUFFER_SIZE * BLOCK_SIZE) / 2.0f + LOD_SHIFT * BLOCK_SIZE;
	//float z = (mapBufferSize_Z * GAME_MAX_BUFFER_SIZE * BLOCK_SIZE) / 2.0f + LOD_SHIFT * BLOCK_SIZE;
	return vec2(LOD_SHIFT * BLOCK_SIZE, LOD_SHIFT * BLOCK_SIZE);
}

void GameMap::saveTerrain(std::string filePath)
{
    auto terrainFile = fopen(filePath.c_str(), "wb");
	size_t count = (mapBufferSize_X) * (mapBufferSize_Y) * (mapBufferSize_Z);
	size_t writeCount = 0;
	for(size_t i = 0; i < count; i++)
	{
		auto buff = m_totalBuffer[i].m_buff;
		if(buff && m_totalBuffer[i].isEdit)
		{
			fwrite(&i, sizeof(size_t), 1, terrainFile);
			fwrite(buff, sizeof(voxelInfo) * GAME_MAX_BUFFER_SIZE* GAME_MAX_BUFFER_SIZE *GAME_MAX_BUFFER_SIZE, 1, terrainFile);
			writeCount += 1;
		}
	}
    fclose(terrainFile);
	tlog("write %ld chunk, size of the voxel info %ld (%d %d %d)",writeCount, sizeof(voxelInfo),mapBufferSize_X ,mapBufferSize_Y, mapBufferSize_Z);
	
}

void GameMap::loadTerrain(std::string filePath)
{
	auto terrainFile = fopen(filePath.c_str(), "rb");
	size_t index;
	size_t loadCount = 0;
	fseek(terrainFile, 0, SEEK_END);
	size_t buffSize = ftell(terrainFile);
	fseek(terrainFile, 0, SEEK_SET);

	while(ftell(terrainFile) < buffSize)
	{
		voxelInfo * buff = new voxelInfo[GAME_MAX_BUFFER_SIZE* GAME_MAX_BUFFER_SIZE *GAME_MAX_BUFFER_SIZE];
		fread(&index,sizeof(size_t),1,terrainFile);
		fread(buff, sizeof(voxelInfo) * GAME_MAX_BUFFER_SIZE* GAME_MAX_BUFFER_SIZE *GAME_MAX_BUFFER_SIZE,1 , terrainFile);
		m_totalBuffer[index].m_buff = buff;
		m_totalBuffer[index].isEdit = true;
		loadCount++;
	}
	tlog("loadCount %ld", loadCount);
}

void GameMap::generateVoxelPage(size_t buffIDX, size_t buffIDY, size_t buffIDZ)
{
	int buffIndex = buffIDX * (mapBufferSize_Z * mapBufferSize_Y) + buffIDY * (mapBufferSize_Z) + buffIDZ;
	if (m_totalBuffer[buffIndex].m_buff)
	{
		return;
	}
	m_totalBuffer[buffIndex].m_buff = new voxelInfo[GAME_MAX_BUFFER_SIZE* GAME_MAX_BUFFER_SIZE *GAME_MAX_BUFFER_SIZE];
	for (int i = 0; i < GAME_MAX_BUFFER_SIZE; i++)
	{
		for (int k = 0; k < GAME_MAX_BUFFER_SIZE; k++)
		{
			for (int j = 0; j < GAME_MAX_BUFFER_SIZE;j++) // Y in the most inner loop, cache friendly
			{
				int cellIndex = i * GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE + j * GAME_MAX_BUFFER_SIZE + k;
				const int globalX = static_cast<int>(buffIDX * GAME_MAX_BUFFER_SIZE + i);
				const int globalY = static_cast<int>(buffIDY * GAME_MAX_BUFFER_SIZE + j);
				const int globalZ = static_cast<int>(buffIDZ * GAME_MAX_BUFFER_SIZE + k);
				m_totalBuffer[buffIndex].m_buff[cellIndex] = sampleProceduralVoxel(globalX, globalY, globalZ);
			}
		}
	}
}

vec3 GameMap::getMapOffset() const
{
	return m_mapOffset;
}

bool GameMap::isVoxelInDomain(int x, int y, int z) const
{
	return x >= 0 && y >= 0 && z >= 0
		&& x < GAME_MAP_WIDTH_VOXELS
		&& y < GAME_MAP_HEIGHT_VOXELS
		&& z < GAME_MAP_DEPTH_VOXELS;
}

int GameMap::pageIndex(int pageX, int pageY, int pageZ) const
{
	return pageX * (mapBufferSize_Z * mapBufferSize_Y) + pageY * mapBufferSize_Z + pageZ;
}

int GameMap::pageIndexForVoxel(int x, int y, int z) const
{
	return pageIndex(x / GAME_MAX_BUFFER_SIZE, y / GAME_MAX_BUFFER_SIZE, z / GAME_MAX_BUFFER_SIZE);
}

int GameMap::localIndexInPage(int x, int y, int z) const
{
	const int currX = x % GAME_MAX_BUFFER_SIZE;
	const int currY = y % GAME_MAX_BUFFER_SIZE;
	const int currZ = z % GAME_MAX_BUFFER_SIZE;
	return currX * GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE + currY * GAME_MAX_BUFFER_SIZE + currZ;
}

bool GameMap::isPageAllocated(int thePageIndex) const
{
	return thePageIndex >= 0
		&& thePageIndex < mapBufferSize_X * mapBufferSize_Y * mapBufferSize_Z
		&& m_totalBuffer[thePageIndex].m_buff;
}

voxelInfo GameMap::makeOutsideVoxel() const
{
	voxelInfo result;
	result.w = 255;
	result.setMat(0, 0, 0, vec3(1, 0, 0));
	return result;
}

bool GameMap::hasVoxelPageFor(int x, int y, int z) const
{
	if (!isVoxelInDomain(x, y, z))
	{
		return false;
	}
	return isPageAllocated(pageIndexForVoxel(x, y, z));
}

bool GameMap::ensureVoxelPageFor(int x, int y, int z)
{
	if (!isVoxelInDomain(x, y, z))
	{
		return false;
	}

	const int buffIDX = x / GAME_MAX_BUFFER_SIZE;
	const int buffIDY = y / GAME_MAX_BUFFER_SIZE;
	const int buffIDZ = z / GAME_MAX_BUFFER_SIZE;
	const int buffIndex = pageIndex(buffIDX, buffIDY, buffIDZ);
	if (!m_totalBuffer[buffIndex].m_buff)
	{
		generateVoxelPage(buffIDX, buffIDY, buffIDZ);
	}
	return true;
}

voxelInfo* GameMap::ensureVoxelForWrite(int x, int y, int z)
{
	if (!ensureVoxelPageFor(x, y, z))
	{
		return nullptr;
	}
	const int buffIndex = pageIndexForVoxel(x, y, z);
	return &m_totalBuffer[buffIndex].m_buff[localIndexInPage(x, y, z)];
}

bool GameMap::writeVoxelDensity(int x, int y, int z, unsigned char w)
{
	voxelInfo* voxel = ensureVoxelForWrite(x, y, z);
	if (!voxel)
	{
		return false;
	}
	voxel->w = w;
	m_totalBuffer[pageIndexForVoxel(x, y, z)].isEdit = true;
	return true;
}

bool GameMap::writeVoxelMaterial(int x, int y, int z, int matIndex)
{
	voxelInfo* voxel = ensureVoxelForWrite(x, y, z);
	if (!voxel)
	{
		return false;
	}
	voxel->setMat(matIndex, 0, 0, vec3(1, 0, 0));
	m_totalBuffer[pageIndexForVoxel(x, y, z)].isEdit = true;
	return true;
}

GameMapBuffer::GameMapBuffer()
{
    m_buff = nullptr;
	isEdit = false;
}

voxelInfo GameMapBuffer::get(int theX, int theY, int theZ)
{
	theX = std::clamp(theX, 0, GAME_MAX_BUFFER_SIZE - 1);
	theY = std::clamp(theY, 0, GAME_MAX_BUFFER_SIZE - 1);
	theZ = std::clamp(theZ, 0, GAME_MAX_BUFFER_SIZE - 1);
	int cellIndex = theX * GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE + theY * GAME_MAX_BUFFER_SIZE + theZ;
	return m_buff[cellIndex];
}
} // namespace tzw
