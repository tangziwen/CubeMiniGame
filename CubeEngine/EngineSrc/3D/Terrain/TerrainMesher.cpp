#include "TerrainMesher.h"

#include "SurfaceNets.h"
#include "CubeGame/GameConfig.h"

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
		|| sampleBuffer.meshCellCount != request.meshCellCount
		|| request.meshCellCount != MAX_BLOCK)
	{
		result.state = TerrainMeshState::Failed;
		return result;
	}

	const vec3 basePoint = request.region.worldMin(request.mapOffset, request.blockSize);
	result.mesh = std::make_unique<Mesh>();
	SurfaceNets::shared()->generate(basePoint, result.mesh.get(), sampleBuffer.voxelSize,
		const_cast<voxelInfo*>(sampleBuffer.voxels.data()), 0.0f, 0, nullptr);

	if (request.sampleStride != 1)
	{
		const float scale = static_cast<float>(request.sampleStride);
		for (VertexData& vertex : result.mesh->m_vertices)
		{
			vertex.m_pos = basePoint + (vertex.m_pos - basePoint) * scale;
			vertex.m_texCoord = vertex.m_pos.xz();
		}
	}

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
