#pragma once

#include "../../Engine/EngineDef.h"
#include "../../Math/vec4.h"
#include "../../Mesh/Mesh.h"
#include "Mesh/VertexData.h"

namespace tzw {
	struct voxelInfo;
static const int MIN_PADDING = 1;
static const int MAX_PADDING = 2;
class TransVoxel :public Singleton<TransVoxel>
{
public:
    void generateWithoutNormal(vec3 basePoint,Mesh * mesh, Mesh * transitionMesh, int VOXEL_SIZE, voxelInfo * srcData, float minValue = -1, int lodLevel = 0);
	void build_transition(vec3 basePoint,Mesh * mesh, int BlockSize, voxelInfo * srcData, int direction, int lodLevel = 0);
    TransVoxel();
};

} // namespace tzw
