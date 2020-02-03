#ifndef TZW_GAMEMAP_H
#define TZW_GAMEMAP_H

#include "noise/noise.h"
#include "noise/noiseutils.h"
#include "EngineSrc/Math/vec3.h"
#include "Math/vec4.h"

namespace tzw {
class Chunk;
struct voxelInfo
{
	float x;
	float y;
	float z;
	float w;
	char matIndex1;
	char matIndex2;
	float matFactor;
	void setV4(vec4 v);
	vec4 getV();
	voxelInfo(float x, float y, float z, float w);
	voxelInfo();
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
    MapType getMapType() const;
    void setMapType(const MapType &mapType);
	void setMinHeight(float minHeight);
	float minHeight();
	ChunkInfo * getChunkInfo(int x, int y, int z);
	ChunkInfo * m_chunkInfoArray[128][16][128];
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
