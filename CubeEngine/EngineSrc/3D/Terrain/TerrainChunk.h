#ifndef TZW_TERRAINCHUNK_H
#define TZW_TERRAINCHUNK_H
#include "../../Interface/Drawable3D.h"
#include "../../Mesh/Mesh.h"
#include "../Material/Material.h"
namespace tzw {

class TerrainChunk : public Drawable3D
{
public:
    TerrainChunk();

    virtual void draw();
protected:
    Mesh * m_mesh;
};

} // namespace tzw

#endif // TZW_TERRAINCHUNK_H
