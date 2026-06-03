#include "TerrainMeshRequest.h"

#include "TerrainOctreeNode.h"
#include "Transvoxel.h"

namespace tzw {

bool TerrainMeshRequest::isValid() const
{
	return region.cellSize > 0
		&& meshCellCount > 0
		&& sampleStride > 0
		&& sampleLodPower >= 0
		&& voxelSize > 0
		&& blockSize > 0.0f
		&& region.sampleStride(meshCellCount) == sampleStride
		&& region.sampleLodPower(meshCellCount) == sampleLodPower;
}

TerrainMeshRequest makeTerrainMeshRequest(const TerrainOctreeNode& node,
	const TerrainOctreeConfig& config, int revision)
{
	TerrainMeshRequest request;
	if (!config.isValid())
	{
		return request;
	}

	request.key = node.key();
	request.region = node.region();
	request.meshCellCount = config.meshCellCount;
	request.sampleStride = request.region.sampleStride(request.meshCellCount);
	request.sampleLodPower = request.region.sampleLodPower(request.meshCellCount);
	request.voxelSize = request.meshCellCount + MIN_PADDING + MAX_PADDING;
	request.mapOffset = config.mapOffset;
	request.blockSize = config.blockSize;
	request.revision = revision;
	return request;
}

} // namespace tzw
