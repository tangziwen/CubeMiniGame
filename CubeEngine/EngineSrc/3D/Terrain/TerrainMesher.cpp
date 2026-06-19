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

	config.seams = request.seams;
	config.sampleOrigin = sampleBuffer.sampleOrigin;
	config.sampleStride = request.sampleStride;
	config.domainSize = request.domainSize;
	config.mapOffset = request.mapOffset;
	config.blockSize = request.blockSize;
	config.debugLodVertexColorEnabled = m_debugLodVertexColorEnabled;
	config.debugLodPower = request.sampleLodPower;

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

void TerrainMesher::setDebugLodVertexColorEnabled(bool enabled)
{
	m_debugLodVertexColorEnabled = enabled;
}

bool TerrainMesher::isDebugLodVertexColorEnabled() const
{
	return m_debugLodVertexColorEnabled;
}

} // namespace tzw
