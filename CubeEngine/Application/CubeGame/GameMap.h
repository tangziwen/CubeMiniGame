#ifndef TZW_GAMEMAP_H
#define TZW_GAMEMAP_H

#include "noise/noise.h"
#include "noise/noiseutils.h"
#include "EngineSrc/Math/vec3.h"
#include "Math/vec4.h"
#include "Mesh/VertexData.h"
namespace tzw {
class Chunk;

struct voxelInfo
{
	float x;
	float y;
	float z;
	float w;
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
	voxelInfo * mcPoints;
	void loadChunk(FILE * file);
	void dumpChunk(FILE * f);
	void initData();
	int x;
	int y;
	int z;
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
    float getDensity(vec3 pos);
	int getMat(vec3 pos, float slope);
    MapType getMapType() const;
    void setMapType(const MapType &mapType);
	void setMinHeight(float minHeight);
	float minHeight();
	ChunkInfo * getChunkInfo(int x, int y, int z);
	ChunkInfo * m_chunkInfoArray[128][16][128];
	float edgeFallOffSelect(float lowBound, float upBound, float edgeVal, float val1, float val2, float selectVal);
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
};

} // namespace tzw

#endif // TZW_GAMEMAP_H
