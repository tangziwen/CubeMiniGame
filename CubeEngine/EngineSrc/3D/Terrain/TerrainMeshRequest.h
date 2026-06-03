#pragma once

#include "TerrainOctreeTypes.h"

namespace tzw {

class TerrainOctreeNode;

struct TerrainMeshRequest
{
	TerrainNodeKey key;
	TerrainRegion region;
	int meshCellCount = 0;
	int sampleStride = 0;
	int sampleLodPower = -1;
	int voxelSize = 0;
	vec3 mapOffset;
	float blockSize = 1.0f;
	int revision = 0;

	bool isValid() const;
};

TerrainMeshRequest makeTerrainMeshRequest(const TerrainOctreeNode& node,
	const TerrainOctreeConfig& config, int revision);

} // namespace tzw
