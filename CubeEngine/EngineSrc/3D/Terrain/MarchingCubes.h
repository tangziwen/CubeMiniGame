#ifndef TZW_MARCHINGCUBES_H
#define TZW_MARCHINGCUBES_H

#include "../../Engine/EngineDef.h"
#include "../../Math/vec4.h"
#include "../../Mesh/Mesh.h"
namespace tzw {
	struct voxelInfo;
class MarchingCubes
{
public:
    TZW_SINGLETON_DECL(MarchingCubes)
    void generate(Mesh * mesh, int ncellsX, int ncellsY, int ncellsZ, vec4 * points, float minValue);
    void generateWithoutNormal(Mesh * mesh, int ncellsX, int ncellsY, int ncellsZ, voxelInfo * srcData, float minValue = -1, int lodLevel = 0);
private:
    MarchingCubes();
};

} // namespace tzw

#endif // TZW_MARCHINGCUBES_H
