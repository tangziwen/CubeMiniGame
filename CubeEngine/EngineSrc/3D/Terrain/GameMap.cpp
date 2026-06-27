#include "GameMap.h"

#include "FastNoise/FastNoise.h"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <random>
#include <vector>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "3D/Vegetation/FoliageSystem.h"
namespace tzw {
GameMap* GameMap::m_instance = nullptr;

namespace {

int ceilDiv(int value, int divisor)
{
	return (value + divisor - 1) / divisor;
}

constexpr uint32_t TerrainSnapshotVersion = 1;
constexpr uint32_t TerrainSnapshotPageSize = GAME_MAX_BUFFER_SIZE;
constexpr uint32_t TerrainSnapshotVoxelCount =
	GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE;
constexpr uint32_t TerrainSnapshotMaterialBytesPerVoxel = 4;
const char TerrainSnapshotMagic[8] = { 'T', 'Z', 'W', 'T', 'E', 'R', 'R', '1' };

template<typename T>
bool writeBinaryValue(FILE* file, const T& value)
{
	return file && fwrite(&value, sizeof(T), 1, file) == 1;
}

template<typename T>
bool readBinaryValue(FILE* file, T& value)
{
	return file && fread(&value, sizeof(T), 1, file) == 1;
}

bool writeBinaryData(FILE* file, const void* data, size_t size)
{
	return file && (size == 0 || fwrite(data, size, 1, file) == 1);
}

bool readBinaryData(FILE* file, void* data, size_t size)
{
	return file && (size == 0 || fread(data, size, 1, file) == 1);
}

void addFloatArray(rapidjson::Value& obj, const char* name, float x, float y, float z,
	rapidjson::Document::AllocatorType& allocator)
{
	rapidjson::Value values(rapidjson::kArrayType);
	values.PushBack(x, allocator);
	values.PushBack(y, allocator);
	values.PushBack(z, allocator);
	obj.AddMember(rapidjson::Value(name, allocator), values, allocator);
}

bool readFloatArray3(const rapidjson::Value& obj, const char* name, float& x, float& y, float& z)
{
	if (!obj.HasMember(name) || !obj[name].IsArray() || obj[name].Size() != 3)
	{
		return false;
	}
	const rapidjson::Value& values = obj[name];
	if (!values[0].IsNumber() || !values[1].IsNumber() || !values[2].IsNumber())
	{
		return false;
	}
	x = values[0].GetFloat();
	y = values[1].GetFloat();
	z = values[2].GetFloat();
	return true;
}

bool readRequiredIntField(const rapidjson::Value& obj, const char* name, int& value)
{
	if (!obj.HasMember(name) || !obj[name].IsInt())
	{
		return false;
	}
	value = obj[name].GetInt();
	return true;
}

bool readRequiredUintField(const rapidjson::Value& obj, const char* name, uint32_t& value)
{
	if (!obj.HasMember(name) || !obj[name].IsUint())
	{
		return false;
	}
	value = obj[name].GetUint();
	return true;
}

bool readRequiredFloatField(const rapidjson::Value& obj, const char* name, float& value)
{
	if (!obj.HasMember(name) || !obj[name].IsNumber())
	{
		return false;
	}
	value = obj[name].GetFloat();
	return true;
}

unsigned char proceduralDensityAt(GameMap& map, int sampleX, int sampleY, int sampleZ)
{
	if (!map.isVoxelInDomain(sampleX, sampleY, sampleZ))
	{
		return 255;
	}

	const vec3 mapOffset = map.getMapOffset();
	auto targetH = map.getHeight(
		vec2(sampleX * map.blockSize(), sampleZ * map.blockSize()) + vec2(mapOffset.x, mapOffset.z));
	auto currH = sampleY * map.blockSize();
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
  , m_treeID(0)
  , m_grassID(0)
  , mapBufferSize_X(0)
  , mapBufferSize_Y(0)
  , mapBufferSize_Z(0)
  , m_totalBuffer(nullptr)
  , m_widthVoxels(GAME_MAP_WIDTH_VOXELS)
  , m_depthVoxels(GAME_MAP_DEPTH_VOXELS)
  , m_heightVoxels(GAME_MAP_HEIGHT_VOXELS)
  , m_blockSize(BLOCK_SIZE)
  , m_proceduralSeed(1337)
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
	m_mapOffset = vec3(m_widthVoxels * m_blockSize / -2.0f, 0,
		m_depthVoxels * m_blockSize / -2.0f);
}

GameMap::~GameMap()
{
	releaseTerrainBuffers();
	delete m_plane;
	m_plane = nullptr;
}

void GameMap::init(const GameMapInitInfo& initInfo)
{
	releaseTerrainBuffers();
	m_ratio = initInfo.ratio;
	m_widthVoxels = initInfo.widthVoxels;
	m_depthVoxels = initInfo.depthVoxels;
	m_heightVoxels = initInfo.heightVoxels;
	m_blockSize = initInfo.blockSize;
	m_proceduralSeed = initInfo.proceduralSeed;
	m_mapOffset = vec3(m_widthVoxels * m_blockSize / -2.0f, 0,
		m_depthVoxels * m_blockSize / -2.0f);
	applyProceduralSeed();

	mapBufferSize_X = ceilDiv(m_widthVoxels, GAME_MAX_BUFFER_SIZE);
	mapBufferSize_Y = ceilDiv(m_heightVoxels, GAME_MAX_BUFFER_SIZE);
	mapBufferSize_Z = ceilDiv(m_depthVoxels, GAME_MAX_BUFFER_SIZE);
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
}

void GameMap::init(float ratio, int width, int depth, int height)
{
	GameMapInitInfo initInfo;
	initInfo.ratio = ratio;
	initInfo.widthVoxels = width * MAX_BLOCK;
	initInfo.depthVoxels = depth * MAX_BLOCK;
	initInfo.heightVoxels = height * MAX_BLOCK;
	initInfo.blockSize = BLOCK_SIZE;
	init(initInfo);
}

GameMap*
GameMap::shared()
{
  if (!m_instance) {
    m_instance = new GameMap();
  }
  return m_instance;
}

int GameMap::widthVoxels() const
{
	return m_widthVoxels;
}

int GameMap::depthVoxels() const
{
	return m_depthVoxels;
}

int GameMap::heightVoxels() const
{
	return m_heightVoxels;
}

float GameMap::blockSize() const
{
	return m_blockSize;
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
	if (!isPageAllocated(buffIndex))
	{
		generateVoxelPage(
			static_cast<size_t>(x / GAME_MAX_BUFFER_SIZE),
			static_cast<size_t>(y / GAME_MAX_BUFFER_SIZE),
			static_cast<size_t>(z / GAME_MAX_BUFFER_SIZE));
	}
	if (isPageAllocated(buffIndex))
	{
		ensureVoxelDensity(x, y, z);
		ensureVoxelMaterial(x, y, z);
		return m_totalBuffer[buffIndex].m_buff[localIndexInPage(x, y, z)];
	}
	return sampleProceduralVoxel(x, y, z);
}

unsigned char GameMap::sampleVoxelDensity(int x, int y, int z)
{
	if (!isVoxelInDomain(x, y, z))
	{
		return 255;
	}

	const int buffIndex = pageIndexForVoxel(x, y, z);
	if (!isPageAllocated(buffIndex))
	{
		generateVoxelPage(
			static_cast<size_t>(x / GAME_MAX_BUFFER_SIZE),
			static_cast<size_t>(y / GAME_MAX_BUFFER_SIZE),
			static_cast<size_t>(z / GAME_MAX_BUFFER_SIZE));
	}

	if (isPageAllocated(buffIndex))
	{
		ensureVoxelDensity(x, y, z);
		return m_totalBuffer[buffIndex].m_buff[localIndexInPage(x, y, z)].w;
	}
	return sampleProceduralDensity(x, y, z);
}

voxelInfo GameMap::sampleProceduralVoxel(int x, int y, int z)
{
	if (!isVoxelInDomain(x, y, z))
	{
		return makeOutsideVoxel();
	}
	voxelInfo result;
	result.w = sampleProceduralDensity(x, y, z);
	auto x1 = sampleProceduralDensity(x - 1, y, z);
	auto x2 = sampleProceduralDensity(x + 1, y, z);
	auto y1 = sampleProceduralDensity(x, y - 1, z);
	auto y2 = sampleProceduralDensity(x, y + 1, z);
	auto z1 = sampleProceduralDensity(x, y, z - 1);
	auto z2 = sampleProceduralDensity(x, y, z + 1);
	auto gradientVec = vec3(x1 - x2, y1 - y2, z1 - z2);
	float slope = 0.0f;
	if (gradientVec.length() > 0.0001f)
	{
		slope = 1.0 - std::clamp(
			vec3::DotProduct(gradientVec.normalized(), vec3(0, -1, 0)), 0.0f, 1.0f);
	}
	vec3 samplePos(x * m_blockSize, y * m_blockSize, z * m_blockSize);
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
	return sampleVoxelDensity(x, y, z);
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
    return vec3((buffIDX * GAME_MAX_BUFFER_SIZE)  * m_blockSize, (buffIDY * GAME_MAX_BUFFER_SIZE)  * m_blockSize, (buffIDZ * GAME_MAX_BUFFER_SIZE)  * m_blockSize);
}

vec3 GameMap::voxelToWorldPos(int x, int y, int z)
{
	return vec3((x)  * m_blockSize, (y )  * m_blockSize, (z)  * m_blockSize) + m_mapOffset;
}

vec3 GameMap::worldPosToVoxelPos(vec3 pos)
{
	pos -= m_mapOffset;
	return vec3(int(pos.x / m_blockSize), int(pos.y / m_blockSize),int(pos.z / m_blockSize));
}

float GameMap::getHeight(vec2 posXZ)
{
	return getNoiseValue(posXZ.x, 0.0, posXZ.y) + 32.f;
}

vec3 GameMap::getNormal(vec2 posXZ)
{
	vec2 posZ = vec2(posXZ.x, posXZ.y - m_blockSize);
	vec2 posX = vec2(posXZ.x + m_blockSize, posXZ.y);

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
	return vec2(LOD_SHIFT * m_blockSize, LOD_SHIFT * m_blockSize);
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
			ensurePageDensityComplete(static_cast<int>(i));
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
		m_totalBuffer[index].m_densityReady = new unsigned char[GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE];
		std::fill(m_totalBuffer[index].m_densityReady,
			m_totalBuffer[index].m_densityReady + GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE, 1);
		m_totalBuffer[index].m_matReady = new unsigned char[GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE];
		std::fill(m_totalBuffer[index].m_matReady,
			m_totalBuffer[index].m_matReady + GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE, 1);
		m_totalBuffer[index].isEdit = true;
		loadCount++;
	}
	tlog("loadCount %ld", loadCount);
}

bool GameMap::serializeTerrainSnapshot(std::string manifestPath, std::string pageDataPath)
{
	if (!m_totalBuffer)
	{
		return false;
	}

	const int totalPageCount = mapBufferSize_X * mapBufferSize_Y * mapBufferSize_Z;
	std::vector<int> editedPageIds;
	editedPageIds.reserve(totalPageCount);
	for (int i = 0; i < totalPageCount; ++i)
	{
		if (m_totalBuffer[i].m_buff && m_totalBuffer[i].isEdit)
		{
			editedPageIds.push_back(i);
		}
	}
	const uint32_t editedPageCount = static_cast<uint32_t>(editedPageIds.size());

	rapidjson::Document doc;
	doc.SetObject();
	auto& allocator = doc.GetAllocator();
	doc.AddMember("Version", static_cast<int>(TerrainSnapshotVersion), allocator);
	doc.AddMember("PageDataFile", rapidjson::Value(pageDataPath.c_str(), allocator), allocator);
	doc.AddMember("WidthVoxels", m_widthVoxels, allocator);
	doc.AddMember("DepthVoxels", m_depthVoxels, allocator);
	doc.AddMember("HeightVoxels", m_heightVoxels, allocator);
	doc.AddMember("BlockSize", m_blockSize, allocator);
	doc.AddMember("PageSize", static_cast<int>(TerrainSnapshotPageSize), allocator);
	doc.AddMember("MapType", static_cast<int>(m_mapType), allocator);
	doc.AddMember("MinHeight", m_minHeight, allocator);
	doc.AddMember("MaxHeight", m_maxHeight, allocator);
	doc.AddMember("ProceduralSeed", m_proceduralSeed, allocator);
	addFloatArray(doc, "Offsets", x_offset, y_offset, z_offset, allocator);
	doc.AddMember("PageOrder", rapidjson::Value("LinearPageIdAscending", allocator), allocator);
	doc.AddMember("PageCount", editedPageCount, allocator);

	FILE* pageFile = fopen(pageDataPath.c_str(), "wb");
	if (!pageFile)
	{
		tlog("[error] can not write terrain page data %s", pageDataPath.c_str());
		return false;
	}

	bool ok = writeBinaryData(pageFile, TerrainSnapshotMagic, sizeof(TerrainSnapshotMagic))
		&& writeBinaryValue(pageFile, TerrainSnapshotVersion)
		&& writeBinaryValue(pageFile, TerrainSnapshotPageSize)
		&& writeBinaryValue(pageFile, editedPageCount);

	std::vector<unsigned char> densityBytes(TerrainSnapshotVoxelCount);
	std::vector<unsigned char> matReadyBytes(TerrainSnapshotVoxelCount);
	std::vector<unsigned char> materialBytes(
		static_cast<size_t>(TerrainSnapshotVoxelCount) * TerrainSnapshotMaterialBytesPerVoxel);
	// TerrainPages.bin records are part of the save format and must stay sorted by linear page id.
	for (int pageIndexValue : editedPageIds)
	{
		if (!ok)
		{
			break;
		}
		GameMapBuffer& buffer = m_totalBuffer[pageIndexValue];

		ensurePageDensityComplete(pageIndexValue);
		const uint32_t pageX = static_cast<uint32_t>(pageIndexValue / (mapBufferSize_Z * mapBufferSize_Y));
		const int rest = pageIndexValue % (mapBufferSize_Z * mapBufferSize_Y);
		const uint32_t pageY = static_cast<uint32_t>(rest / mapBufferSize_Z);
		const uint32_t pageZ = static_cast<uint32_t>(rest % mapBufferSize_Z);
		const uint32_t densitySize = TerrainSnapshotVoxelCount;
		const uint32_t matReadySize = TerrainSnapshotVoxelCount;
		const uint32_t materialSize = TerrainSnapshotVoxelCount * TerrainSnapshotMaterialBytesPerVoxel;

		for (uint32_t i = 0; i < TerrainSnapshotVoxelCount; ++i)
		{
			densityBytes[i] = buffer.m_buff[i].w;
			const bool materialReady = buffer.m_matReady && buffer.m_matReady[i];
			matReadyBytes[i] = materialReady ? 1 : 0;
			const size_t base = static_cast<size_t>(i) * TerrainSnapshotMaterialBytesPerVoxel;
			if (materialReady)
			{
				materialBytes[base + 0] = static_cast<unsigned char>(buffer.m_buff[i].matInfo.matIndex1);
				materialBytes[base + 1] = static_cast<unsigned char>(buffer.m_buff[i].matInfo.matIndex2);
				materialBytes[base + 2] = static_cast<unsigned char>(buffer.m_buff[i].matInfo.matIndex3);
				materialBytes[base + 3] = buffer.m_buff[i].matInfo.matBlendFactor;
			}
			else
			{
				materialBytes[base + 0] = 0;
				materialBytes[base + 1] = 0;
				materialBytes[base + 2] = 0;
				materialBytes[base + 3] = 0;
			}
		}

		ok = writeBinaryValue(pageFile, pageX)
			&& writeBinaryValue(pageFile, pageY)
			&& writeBinaryValue(pageFile, pageZ)
			&& writeBinaryValue(pageFile, densitySize)
			&& writeBinaryData(pageFile, densityBytes.data(), densityBytes.size())
			&& writeBinaryValue(pageFile, matReadySize)
			&& writeBinaryData(pageFile, matReadyBytes.data(), matReadyBytes.size())
			&& writeBinaryValue(pageFile, materialSize)
			&& writeBinaryData(pageFile, materialBytes.data(), materialBytes.size());
	}
	fclose(pageFile);
	if (!ok)
	{
		tlog("[error] failed to write terrain page data %s", pageDataPath.c_str());
		return false;
	}

	FILE* manifestFile = fopen(manifestPath.c_str(), "wb");
	if (!manifestFile)
	{
		tlog("[error] can not write terrain manifest %s", manifestPath.c_str());
		return false;
	}
	char manifestBuffer[65536];
	rapidjson::FileWriteStream manifestStream(manifestFile, manifestBuffer, sizeof(manifestBuffer));
	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(manifestStream);
	writer.SetIndent('\t', 1);
	doc.Accept(writer);
	fclose(manifestFile);
	return true;
}

bool GameMap::unserializeTerrainSnapshot(std::string manifestPath, std::string pageDataPath)
{
	FILE* manifestFile = fopen(manifestPath.c_str(), "rb");
	if (!manifestFile)
	{
		return false;
	}
	char manifestBuffer[65536];
	rapidjson::FileReadStream manifestStream(manifestFile, manifestBuffer, sizeof(manifestBuffer));
	rapidjson::Document doc;
	doc.ParseStream<rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag>(manifestStream);
	fclose(manifestFile);
	if (doc.HasParseError() || !doc.IsObject())
	{
		tlog("[error] terrain manifest parse failed %s", manifestPath.c_str());
		return false;
	}

	int manifestVersion = 0;
	int manifestWidth = 0;
	int manifestDepth = 0;
	int manifestHeight = 0;
	int manifestPageSize = 0;
	float manifestBlockSize = 0.0f;
	uint32_t manifestPageCount = 0;
	if (!readRequiredIntField(doc, "Version", manifestVersion)
		|| manifestVersion != static_cast<int>(TerrainSnapshotVersion))
	{
		tlog("[error] unsupported terrain snapshot version %s", manifestPath.c_str());
		return false;
	}
	if (!readRequiredIntField(doc, "WidthVoxels", manifestWidth)
		|| !readRequiredIntField(doc, "DepthVoxels", manifestDepth)
		|| !readRequiredIntField(doc, "HeightVoxels", manifestHeight)
		|| !readRequiredFloatField(doc, "BlockSize", manifestBlockSize)
		|| !readRequiredIntField(doc, "PageSize", manifestPageSize)
		|| !readRequiredUintField(doc, "PageCount", manifestPageCount))
	{
		tlog("[error] terrain manifest missing required fields %s", manifestPath.c_str());
		return false;
	}
	if (!doc.HasMember("PageOrder") || !doc["PageOrder"].IsString()
		|| std::strcmp(doc["PageOrder"].GetString(), "LinearPageIdAscending") != 0)
	{
		tlog("[error] terrain manifest page order mismatch %s", manifestPath.c_str());
		return false;
	}
	if (manifestWidth != m_widthVoxels
		|| manifestDepth != m_depthVoxels
		|| manifestHeight != m_heightVoxels
		|| manifestPageSize != static_cast<int>(TerrainSnapshotPageSize))
	{
		tlog("[error] terrain snapshot dimensions mismatch %s", manifestPath.c_str());
		return false;
	}
	if (std::fabs(manifestBlockSize - m_blockSize) > 0.0001f)
	{
		tlog("[error] terrain snapshot block size mismatch %s", manifestPath.c_str());
		return false;
	}

	if (doc.HasMember("MapType"))
	{
		if (!doc["MapType"].IsInt())
		{
			return false;
		}
		m_mapType = static_cast<MapType>(doc["MapType"].GetInt());
	}
	if (doc.HasMember("MinHeight"))
	{
		if (!doc["MinHeight"].IsNumber())
		{
			return false;
		}
		m_minHeight = doc["MinHeight"].GetFloat();
	}
	if (doc.HasMember("MaxHeight"))
	{
		if (!doc["MaxHeight"].IsNumber())
		{
			return false;
		}
		m_maxHeight = doc["MaxHeight"].GetFloat();
	}
	if (doc.HasMember("ProceduralSeed"))
	{
		if (!doc["ProceduralSeed"].IsInt())
		{
			return false;
		}
		m_proceduralSeed = doc["ProceduralSeed"].GetInt();
	}
	if (!readFloatArray3(doc, "Offsets", x_offset, y_offset, z_offset))
	{
		applyProceduralSeed();
	}

	FILE* pageFile = fopen(pageDataPath.c_str(), "rb");
	if (!pageFile)
	{
		if (manifestPageCount != 0)
		{
			return false;
		}
		releaseTerrainBuffers();
		mapBufferSize_X = ceilDiv(m_widthVoxels, GAME_MAX_BUFFER_SIZE);
		mapBufferSize_Y = ceilDiv(m_heightVoxels, GAME_MAX_BUFFER_SIZE);
		mapBufferSize_Z = ceilDiv(m_depthVoxels, GAME_MAX_BUFFER_SIZE);
		m_totalBuffer = new GameMapBuffer[mapBufferSize_X * mapBufferSize_Y * mapBufferSize_Z];
		return true;
	}

	char magic[sizeof(TerrainSnapshotMagic)];
	uint32_t version = 0;
	uint32_t pageSize = 0;
	uint32_t pageCount = 0;
	bool ok = readBinaryData(pageFile, magic, sizeof(magic))
		&& std::memcmp(magic, TerrainSnapshotMagic, sizeof(TerrainSnapshotMagic)) == 0
		&& readBinaryValue(pageFile, version)
		&& readBinaryValue(pageFile, pageSize)
		&& readBinaryValue(pageFile, pageCount)
		&& version == TerrainSnapshotVersion
		&& pageSize == TerrainSnapshotPageSize
		&& pageCount == manifestPageCount;
	if (!ok)
	{
		fclose(pageFile);
		tlog("[error] terrain page data header mismatch %s", pageDataPath.c_str());
		return false;
	}

	releaseTerrainBuffers();
	mapBufferSize_X = ceilDiv(m_widthVoxels, GAME_MAX_BUFFER_SIZE);
	mapBufferSize_Y = ceilDiv(m_heightVoxels, GAME_MAX_BUFFER_SIZE);
	mapBufferSize_Z = ceilDiv(m_depthVoxels, GAME_MAX_BUFFER_SIZE);
	m_totalBuffer = new GameMapBuffer[mapBufferSize_X * mapBufferSize_Y * mapBufferSize_Z];

	std::vector<unsigned char> densityBytes(TerrainSnapshotVoxelCount);
	std::vector<unsigned char> matReadyBytes(TerrainSnapshotVoxelCount);
	std::vector<unsigned char> materialBytes(
		static_cast<size_t>(TerrainSnapshotVoxelCount) * TerrainSnapshotMaterialBytesPerVoxel);
	int lastPageLinearId = -1;
	for (uint32_t page = 0; ok && page < pageCount; ++page)
	{
		uint32_t pageX = 0;
		uint32_t pageY = 0;
		uint32_t pageZ = 0;
		uint32_t densitySize = 0;
		uint32_t matReadySize = 0;
		uint32_t materialSize = 0;
		ok = readBinaryValue(pageFile, pageX)
			&& readBinaryValue(pageFile, pageY)
			&& readBinaryValue(pageFile, pageZ)
			&& readBinaryValue(pageFile, densitySize)
			&& densitySize == TerrainSnapshotVoxelCount
			&& readBinaryData(pageFile, densityBytes.data(), densityBytes.size())
			&& readBinaryValue(pageFile, matReadySize)
			&& matReadySize == TerrainSnapshotVoxelCount
			&& readBinaryData(pageFile, matReadyBytes.data(), matReadyBytes.size())
			&& readBinaryValue(pageFile, materialSize)
			&& materialSize == TerrainSnapshotVoxelCount * TerrainSnapshotMaterialBytesPerVoxel
			&& readBinaryData(pageFile, materialBytes.data(), materialBytes.size());
		if (!ok)
		{
			break;
		}
		if (pageX >= static_cast<uint32_t>(mapBufferSize_X)
			|| pageY >= static_cast<uint32_t>(mapBufferSize_Y)
			|| pageZ >= static_cast<uint32_t>(mapBufferSize_Z))
		{
			ok = false;
			break;
		}
		const int pageLinearId = static_cast<int>(pageX) * (mapBufferSize_Z * mapBufferSize_Y)
			+ static_cast<int>(pageY) * mapBufferSize_Z
			+ static_cast<int>(pageZ);
		if (pageLinearId <= lastPageLinearId)
		{
			ok = false;
			break;
		}
		lastPageLinearId = pageLinearId;

		generateVoxelPage(pageX, pageY, pageZ);
		GameMapBuffer& buffer = m_totalBuffer[pageLinearId];
		for (uint32_t i = 0; i < TerrainSnapshotVoxelCount; ++i)
		{
			buffer.m_buff[i].w = densityBytes[i];
			if (buffer.m_densityReady)
			{
				buffer.m_densityReady[i] = 1;
			}
			if (buffer.m_matReady)
			{
				buffer.m_matReady[i] = matReadyBytes[i] ? 1 : 0;
			}
			const size_t base = static_cast<size_t>(i) * TerrainSnapshotMaterialBytesPerVoxel;
			buffer.m_buff[i].matInfo.matIndex1 = static_cast<char>(materialBytes[base + 0]);
			buffer.m_buff[i].matInfo.matIndex2 = static_cast<char>(materialBytes[base + 1]);
			buffer.m_buff[i].matInfo.matIndex3 = static_cast<char>(materialBytes[base + 2]);
			buffer.m_buff[i].matInfo.matBlendFactor = materialBytes[base + 3];
		}
		buffer.isEdit = true;
	}
	fclose(pageFile);
	if (!ok)
	{
		tlog("[error] terrain page data read failed %s", pageDataPath.c_str());
	}
	return ok;
}

void GameMap::generateVoxelPage(size_t buffIDX, size_t buffIDY, size_t buffIDZ)
{
	int buffIndex = buffIDX * (mapBufferSize_Z * mapBufferSize_Y) + buffIDY * (mapBufferSize_Z) + buffIDZ;
	if (m_totalBuffer[buffIndex].m_buff)
	{
		return;
	}
	voxelInfo* buff = new voxelInfo[GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE];
	unsigned char* densityReady = new unsigned char[GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE];
	unsigned char* matReady = new unsigned char[GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE];
	std::fill(densityReady, densityReady + GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE, 0);
	std::fill(matReady, matReady + GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE, 0);
	m_totalBuffer[buffIndex].m_buff = buff;
	m_totalBuffer[buffIndex].m_densityReady = densityReady;
	m_totalBuffer[buffIndex].m_matReady = matReady;
}

vec3 GameMap::getMapOffset() const
{
	return m_mapOffset;
}

bool GameMap::isVoxelInDomain(int x, int y, int z) const
{
	return x >= 0 && y >= 0 && z >= 0
		&& x < m_widthVoxels
		&& y < m_heightVoxels
		&& z < m_depthVoxels;
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

unsigned char GameMap::sampleProceduralDensity(int x, int y, int z)
{
	return proceduralDensityAt(*this, x, y, z);
}

void GameMap::ensureVoxelDensity(int x, int y, int z)
{
	if (!isVoxelInDomain(x, y, z))
	{
		return;
	}

	const int buffIndex = pageIndexForVoxel(x, y, z);
	if (!isPageAllocated(buffIndex))
	{
		generateVoxelPage(
			static_cast<size_t>(x / GAME_MAX_BUFFER_SIZE),
			static_cast<size_t>(y / GAME_MAX_BUFFER_SIZE),
			static_cast<size_t>(z / GAME_MAX_BUFFER_SIZE));
	}
	if (!isPageAllocated(buffIndex))
	{
		return;
	}

	const int index = localIndexInPage(x, y, z);
	GameMapBuffer& buffer = m_totalBuffer[buffIndex];
	if (buffer.m_densityReady && buffer.m_densityReady[index])
	{
		return;
	}

	const int localX = x % GAME_MAX_BUFFER_SIZE;
	const int localZ = z % GAME_MAX_BUFFER_SIZE;
	const int pageY = (y / GAME_MAX_BUFFER_SIZE) * GAME_MAX_BUFFER_SIZE;
	const vec3 mapOffset = getMapOffset();
	const float targetH = getHeight(
		vec2(x * m_blockSize, z * m_blockSize) + vec2(mapOffset.x, mapOffset.z));
	for (int localY = 0; localY < GAME_MAX_BUFFER_SIZE; ++localY)
	{
		const int globalY = pageY + localY;
		const int columnIndex = localX * GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE
			+ localY * GAME_MAX_BUFFER_SIZE + localZ;
		buffer.m_buff[columnIndex] = makeOutsideVoxel();
		if (globalY < m_heightVoxels)
		{
			const float currH = globalY * m_blockSize;
			const float delta = std::clamp((currH - targetH) * 0.1f, -1.f, 1.f);
			buffer.m_buff[columnIndex].w = static_cast<unsigned char>((delta * 0.5f + 0.5f) * 255.f);
		}
		if (buffer.m_densityReady)
		{
			buffer.m_densityReady[columnIndex] = 1;
		}
		if (buffer.m_matReady)
		{
			buffer.m_matReady[columnIndex] = 0;
		}
	}
}

void GameMap::ensureVoxelMaterial(int x, int y, int z)
{
	if (!isVoxelInDomain(x, y, z))
	{
		return;
	}

	const int buffIndex = pageIndexForVoxel(x, y, z);
	if (!isPageAllocated(buffIndex))
	{
		return;
	}
	ensureVoxelDensity(x, y, z);

	const int index = localIndexInPage(x, y, z);
	GameMapBuffer& buffer = m_totalBuffer[buffIndex];
	if (buffer.m_matReady && buffer.m_matReady[index])
	{
		return;
	}

	auto x1 = sampleVoxelDensity(x - 1, y, z);
	auto x2 = sampleVoxelDensity(x + 1, y, z);
	auto y1 = sampleVoxelDensity(x, y - 1, z);
	auto y2 = sampleVoxelDensity(x, y + 1, z);
	auto z1 = sampleVoxelDensity(x, y, z - 1);
	auto z2 = sampleVoxelDensity(x, y, z + 1);
	auto gradientVec = vec3(x1 - x2, y1 - y2, z1 - z2);
	float slope = 0.0f;
	if (gradientVec.length() > 0.0001f)
	{
		slope = 1.0 - std::clamp(
			vec3::DotProduct(gradientVec.normalized(), vec3(0, -1, 0)), 0.0f, 1.0f);
	}
	vec3 samplePos(x * m_blockSize, y * m_blockSize, z * m_blockSize);
	auto matID = getMat(samplePos, slope);
	buffer.m_buff[index].setMat(matID, 0, 0, vec3(1, 0, 0));
	if (buffer.m_matReady)
	{
		buffer.m_matReady[index] = 1;
	}
}

void GameMap::ensurePageDensityComplete(int pageIndexValue)
{
	if (!isPageAllocated(pageIndexValue))
	{
		return;
	}

	const int pageX = pageIndexValue / (mapBufferSize_Z * mapBufferSize_Y);
	const int rest = pageIndexValue % (mapBufferSize_Z * mapBufferSize_Y);
	const int pageY = rest / mapBufferSize_Z;
	const int pageZ = rest % mapBufferSize_Z;
	for (int x = 0; x < GAME_MAX_BUFFER_SIZE; ++x)
	{
		for (int z = 0; z < GAME_MAX_BUFFER_SIZE; ++z)
		{
			for (int y = 0; y < GAME_MAX_BUFFER_SIZE; ++y)
			{
				const int globalX = pageX * GAME_MAX_BUFFER_SIZE + x;
				const int globalY = pageY * GAME_MAX_BUFFER_SIZE + y;
				const int globalZ = pageZ * GAME_MAX_BUFFER_SIZE + z;
				if (isVoxelInDomain(globalX, globalY, globalZ))
				{
					ensureVoxelDensity(globalX, globalY, globalZ);
					continue;
				}

				const int index = x * GAME_MAX_BUFFER_SIZE * GAME_MAX_BUFFER_SIZE
					+ y * GAME_MAX_BUFFER_SIZE + z;
				GameMapBuffer& buffer = m_totalBuffer[pageIndexValue];
				buffer.m_buff[index] = makeOutsideVoxel();
				if (buffer.m_densityReady)
				{
					buffer.m_densityReady[index] = 1;
				}
			}
		}
	}
}

void GameMap::setVoxelMaterialReady(int x, int y, int z, bool isReady)
{
	if (!isVoxelInDomain(x, y, z))
	{
		return;
	}
	const int buffIndex = pageIndexForVoxel(x, y, z);
	if (!isPageAllocated(buffIndex) || !m_totalBuffer[buffIndex].m_matReady)
	{
		return;
	}
	m_totalBuffer[buffIndex].m_matReady[localIndexInPage(x, y, z)] = isReady ? 1 : 0;
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
	ensureVoxelDensity(x, y, z);
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
	setVoxelMaterialReady(x, y, z, false);
	setVoxelMaterialReady(x - 1, y, z, false);
	setVoxelMaterialReady(x + 1, y, z, false);
	setVoxelMaterialReady(x, y - 1, z, false);
	setVoxelMaterialReady(x, y + 1, z, false);
	setVoxelMaterialReady(x, y, z - 1, false);
	setVoxelMaterialReady(x, y, z + 1, false);
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
	setVoxelMaterialReady(x, y, z, true);
	m_totalBuffer[pageIndexForVoxel(x, y, z)].isEdit = true;
	return true;
}

void GameMap::releaseTerrainBuffers()
{
	if (!m_totalBuffer)
	{
		return;
	}
	const int totalPageCount = mapBufferSize_X * mapBufferSize_Y * mapBufferSize_Z;
	for (int i = 0; i < totalPageCount; ++i)
	{
		delete[] m_totalBuffer[i].m_buff;
		m_totalBuffer[i].m_buff = nullptr;
		delete[] m_totalBuffer[i].m_densityReady;
		m_totalBuffer[i].m_densityReady = nullptr;
		delete[] m_totalBuffer[i].m_matReady;
		m_totalBuffer[i].m_matReady = nullptr;
		m_totalBuffer[i].isEdit = false;
	}
	delete[] m_totalBuffer;
	m_totalBuffer = nullptr;
}

void GameMap::setProceduralSeed(int seed)
{
	m_proceduralSeed = seed;
	applyProceduralSeed();
}

void GameMap::applyProceduralSeed()
{
	std::mt19937 rng(static_cast<uint32_t>(m_proceduralSeed));
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	x_offset = dist(rng);
	y_offset = dist(rng);
	z_offset = dist(rng);
}

int GameMap::proceduralSeed() const
{
	return m_proceduralSeed;
}

GameMapBuffer::GameMapBuffer()
{
    m_buff = nullptr;
	m_densityReady = nullptr;
	m_matReady = nullptr;
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
