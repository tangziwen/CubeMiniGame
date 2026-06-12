#include "TerrainMesher.h"

#include "SurfaceNets.h"

namespace tzw {

TerrainMeshBuildResult TerrainMesher::build(const TerrainMeshRequest& request,
	const TerrainSampleBuffer& sampleBuffer) const
{
	TerrainMeshBuildResult result;
	result.key = request.key;
	result.region = request.region;
	result.worldBounds = request.region.worldAABB(request.mapOffset, request.blockSize);

	if (!request.isValid() || !sampleBuffer.isValid()
		|| sampleBuffer.voxelSize != request.voxelSize
		|| sampleBuffer.sampleStride != request.sampleStride
		|| sampleBuffer.meshCellCount != request.meshCellCount)
	{
		result.state = TerrainMeshState::Failed;
		return result;
	}

	const vec3 basePoint = request.region.worldMin(request.mapOffset, request.blockSize);
	result.mesh = std::make_unique<Mesh>();

	SurfaceNetsGenerateConfig config;
	config.voxelSize = sampleBuffer.voxelSize;
	config.cellCount = request.meshCellCount;
	config.minPadding = sampleBuffer.minPadding;
	config.cellWorldSize = request.sampleStride * request.blockSize;
	config.isoLevel = 128;

	SurfaceNets::shared()->generate(basePoint, result.mesh.get(),
		sampleBuffer.voxels.data(), config);

	result.isEmpty = result.mesh->isEmpty();
	result.state = TerrainMeshState::Ready;
	if (!result.isEmpty)
	{
		result.mesh->calculateAABB();
		result.worldBounds = result.mesh->getAabb();
	}
	return result;
}

} // namespace tzw
