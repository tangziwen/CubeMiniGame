#ifndef TZW_MARCHINGCUBES_H
#define TZW_MARCHINGCUBES_H

#include "../../Engine/EngineDef.h"
#include "../../Math/vec4.h"
#include "../../Mesh/Mesh.h"
namespace tzw {
	struct voxelInfo;
	struct VoxelVertex
	{
		vec3 vertex;
		int matIndex;
	};
class MarchingCubes
{
public:
    TZW_SINGLETON_DECL(MarchingCubes)
    void generateWithoutNormal(Mesh * mesh, int ncellsX, int ncellsY, int ncellsZ, voxelInfo * srcData, float minValue = -1, int lodLevel = 0);
private:
    MarchingCubes();
};

} // namespace tzw

#endif // TZW_MARCHINGCUBES_H
