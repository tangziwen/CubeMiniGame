#ifndef TZW_VOXELCHUNK_H
#define TZW_VOXELCHUNK_H

#include "../../Math/vec4.h"
#include "../../Mesh/Mesh.h"
#include "../../Interface/Drawable3D.h"
namespace tzw {

class VoxelChunk : public Drawable3D
{
public:
    VoxelChunk();
    virtual void draw();
    void init(int ncellsX, int ncellsY, int ncellsZ, vec4 * v4, float minValue);
    Mesh * m_mesh;
};

} // namespace tzw

#endif // TZW_VOXELCHUNK_H
