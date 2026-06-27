#ifndef TZW_GAMEMAP_H
#define TZW_GAMEMAP_H
#define GAME_MAX_BUFFER_SIZE 64
#define LOD_SHIFT 4

#include <array>
#include <string>
#include <vector>
#include "GameMapConfig.h"
#include "noise/noise.h"
#include "noise/noiseutils.h"
#include "EngineSrc/Math/vec3.h"
#include "Math/vec4.h"
#include "Mesh/VertexData.h"
namespace tzw {

struct voxelInfo
{
	// below 127 inside, 127 surface, above 127 outside
	unsigned char w;
	MatBlendInfo matInfo;
	void setV4(vec4 v);
	void setV3(vec3 v);
	vec3 getV3();
	vec4 getV();
	voxelInfo(float x, float y, float z, float w);
	voxelInfo();
	void setMat(char mat1, char mat2, char mat3, vec3 blendFactor);
};

struct GameMapBuffer 
{
	GameMapBuffer();
	voxelInfo get(int theX, int theY, int theZ);
	voxelInfo * m_buff;
	unsigned char* m_densityReady;
	unsigned char* m_matReady;
	bool isEdit;
};

struct GameMapInitInfo
{
	float ratio = 0.05f;
	int widthVoxels = 2048;
	int depthVoxels = 2048;
	int heightVoxels = 128;
	float blockSize = 1.0f;
	int proceduralSeed = 1337;
};

class GameMap
{
public:
    enum class MapType
    {
        Noise,
        Plain,
    };
    GameMap();
	~GameMap();
    void init(const GameMapInitInfo& initInfo);
    void init(float ratio,int width, int depth, int height);
    static GameMap * shared();
	int widthVoxels() const;
	int depthVoxels() const;
	int heightVoxels() const;
	float blockSize() const;
    float maxHeight() const;
    void setMaxHeight(float maxHeight);
	double getNoiseValue(float x, float y, float z);
    bool isSurface(vec3 pos);
	// Lazy read: allocates the owning page and initializes requested procedural columns on demand.
	voxelInfo sampleVoxel(int x, int y, int z);
	unsigned char sampleVoxelDensity(int x, int y, int z);
	voxelInfo sampleProceduralVoxel(int x, int y, int z);
    unsigned char getDensity(vec3 pos);
	unsigned char getVoxelW(int x, int y, int z);
	voxelInfo*  getVoxel(int x, int y ,int z);
	void setVoxel(int x, int y, int z, unsigned char w);
	vec3 voxelToBuffWorldPos(int x, int y, int z);
	vec3 voxelToWorldPos(int x, int y, int z);
	vec3 worldPosToVoxelPos(vec3 pos);
	float getHeight(vec2 posXZ);
	vec3 getNormal(vec2 posXZ);
	int getMat(vec3 pos, float slope);
    MapType getMapType() const;
    void setMapType(const MapType &mapType);
	void setMinHeight(float minHeight);
	float minHeight();
	float edgeFallOffSelect(float lowBound, float upBound, float edgeVal, float val1, float val2, float selectVal);
	int getTreeId();
	int getGrassId();
	vec2 getCenterOfMap();
	void saveTerrain(std::string filePath);
	void loadTerrain(std::string filePath);
	bool serializeTerrainSnapshot(std::string manifestPath, std::string pageDataPath);
	bool unserializeTerrainSnapshot(std::string manifestPath, std::string pageDataPath);
	vec3 getMapOffset() const;
	int proceduralSeed() const;
	bool isVoxelInDomain(int x, int y, int z) const;
	bool hasVoxelPageFor(int x, int y, int z) const;
	bool ensureVoxelPageFor(int x, int y, int z);
	// Writable access: explicitly materializes the owning page before returning or writing.
	voxelInfo* ensureVoxelForWrite(int x, int y, int z);
	bool writeVoxelDensity(int x, int y, int z, unsigned char w);
	bool writeVoxelMaterial(int x, int y, int z, int matIndex);
private:
	int pageIndex(int pageX, int pageY, int pageZ) const;
	int pageIndexForVoxel(int x, int y, int z) const;
	int localIndexInPage(int x, int y, int z) const;
	bool isPageAllocated(int pageIndex) const;
	unsigned char sampleProceduralDensity(int x, int y, int z);
	void ensureVoxelDensity(int x, int y, int z);
	void ensureVoxelMaterial(int x, int y, int z);
	void ensurePageDensityComplete(int pageIndex);
	void setVoxelMaterialReady(int x, int y, int z, bool isReady);
	voxelInfo makeOutsideVoxel() const;
	void generateVoxelPage(size_t buffID_x, size_t buffID_y, size_t buffID_z);
	void releaseTerrainBuffers();
	void setProceduralSeed(int seed);
	void applyProceduralSeed();
    float x_offset,y_offset,z_offset;
    float m_maxHeight;
    float m_ratio;
	float m_minHeight;
    MapType m_mapType;
    noise::model::Plane * m_plane;
    noise::module::Perlin myModule;
    utils::NoiseMap heightMap;
    static GameMap * m_instance;
	int m_treeID;
	int m_grassID;
	int mapBufferSize_X;
	int mapBufferSize_Y;
	int mapBufferSize_Z;
	GameMapBuffer * m_totalBuffer;
	vec3 m_mapOffset;
	int m_widthVoxels;
	int m_depthVoxels;
	int m_heightVoxels;
	float m_blockSize;
	int m_proceduralSeed;
};

} // namespace tzw

#endif // TZW_GAMEMAP_H
