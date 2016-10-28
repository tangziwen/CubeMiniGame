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
    double getValue(int x,int z);
    bool isBlock(Chunk *chunk, int x, int y, int z);
    bool isSurface(vec3 pos);
    float getDensity(vec3 pos);
    MapType getMapType() const;
    void setMapType(const MapType &mapType);

private:
    float x_offset,y_offset;
    float m_maxHeight;
    float m_ratio;
    MapType m_mapType;
    noise::model::Plane * m_plane;
    noise::module::Perlin myModule;
    utils::NoiseMap heightMap;
    static GameMap * m_instance;
};

} // namespace tzw

#endif // TZW_GAMEMAP_H
