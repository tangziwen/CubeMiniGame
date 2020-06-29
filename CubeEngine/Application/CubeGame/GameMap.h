#ifndef TZW_GAMEMAP_H
#define TZW_GAMEMAP_H
#define GAME_MAX_BUFFER_SIZE 64
#define LOD_SHIFT 4

#include "GameConfig.h"
#include "noise/noise.h"
#include "noise/noiseutils.h"
#include "EngineSrc/Math/vec3.h"
#include "Math/vec4.h"
#include "Mesh/VertexData.h"
namespace tzw {
class Chunk;

struct voxelInfo
{
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
struct ChunkInfo
{
	ChunkInfo(int x, int y, int z);
	bool isLoaded;
	voxelInfo * mcPoints[3];
	voxelInfo * mcPoints_lod1;
	void loadChunk(FILE * file);
	void dumpChunk(FILE * f);
	void initData();
	int x;
	int y;
	int z;
	bool isEdit;
};

struct GameMapBuffer 
{
	GameMapBuffer();
	voxelInfo get(int theX, int theY, int theZ);
	voxelInfo * m_buff;
	bool isEdit;
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
    void init(float ratio,int width, int depth, int height);
    static GameMap * shared();
    float maxHeight() const;
    void setMaxHeight(float maxHeight);
	double getNoiseValue(float x, float y, float z);
    bool isBlock(Chunk *chunk, int x, int y, int z);
    bool isSurface(vec3 pos);
	voxelInfo getDensityI(int x, int y, int z);
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
	ChunkInfo * getChunkInfo(int x, int y, int z);
	float edgeFallOffSelect(float lowBound, float upBound, float edgeVal, float val1, float val2, float selectVal);
	int getTreeId();
	int getGrassId();
	GameMapBuffer * m_totalBuffer;
	vec2 getCenterOfMap();
	ChunkInfo* fetchFromSource(int x, int y, int z, int lod);
	void saveTerrain(std::string filePath);
	void loadTerrain(std::string filePath);
	void proceduralGenMapBuffer(size_t buffID_x, size_t buffID_y, size_t buffID_z);
private:
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
	ChunkInfo * m_chunkInfo;

};

} // namespace tzw

#endif // TZW_GAMEMAP_H
