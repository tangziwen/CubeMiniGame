#pragma once

#include "../../Engine/EngineDef.h"
#include "../../Math/vec4.h"
#include "../../Mesh/Mesh.h"
#include "Mesh/VertexData.h"

namespace tzw {
	struct voxelInfo;
class TransVoxel :public Singleton<TransVoxel>
{
public:
    void generateWithoutNormal(vec3 basePoint,Mesh * mesh, int VOXEL_SIZE, voxelInfo * srcData, float minValue = -1, int lodLevel = 0);
	void build_transition(vec3 basePoint,Mesh * mesh, int BlockSize, voxelInfo * srcData, int direction, int lodLevel = 0);
    TransVoxel();
};

} // namespace tzw
