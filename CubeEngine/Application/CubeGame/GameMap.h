#ifndef TZW_GAMEMAP_H
#define TZW_GAMEMAP_H

#include "noise/noise.h"
#include "noise/noiseutils.h"
#include "EngineSrc/Math/vec3.h"
namespace tzw {
class Chunk;
class GameMap
{
public:
    enum class MapType
    {
        Noise,
        Plain,
    };
    GameMap();
    void init(float ratio);
    static GameMap * shared();
    float maxHeight() const;
    void setMaxHeight(float maxHeight);
	double getValue(float x, float y, float z);
    bool isBlock(Chunk *chunk, int x, int y, int z);
    bool isSurface(vec3 pos);
    float getDensity(vec3 pos);
    MapType getMapType() const;
    void setMapType(const MapType &mapType);
	void setMinHeight(float minHeight);
	float minHeight();
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
