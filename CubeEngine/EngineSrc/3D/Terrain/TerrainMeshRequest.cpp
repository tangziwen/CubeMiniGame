#include "TerrainMeshRequest.h"

#include "TerrainOctreeNode.h"
#include "GameMapConfig.h"

namespace tzw {

bool TerrainMeshSeamFace::operator==(const TerrainMeshSeamFace& other) const
{
	return mode == other.mode
		&& levelDelta == other.levelDelta
		&& finerCoverageMask == other.finerCoverageMask;
}

bool TerrainMeshSeamFace::operator!=(const TerrainMeshSeamFace& other) const
{
	return !(*this == other);
}

bool TerrainMeshSeamSet::operator==(const TerrainMeshSeamSet& other) const
{
	for (size_t i = 0; i < faces.size(); ++i)
	{
		if (faces[i] != other.faces[i])
			return false;
	}
	return true;
}

bool TerrainMeshSeamSet::operator!=(const TerrainMeshSeamSet& other) const
{
	return !(*this == other);
}

uint32_t TerrainMeshSeamSet::signature() const
{
	uint32_t sig = 0;
	for (const auto& face : faces)
	{
		sig = sig * 31 + static_cast<uint32_t>(face.mode);
		sig = sig * 31 + static_cast<uint32_t>(face.levelDelta);
		sig = sig * 31 + static_cast<uint32_t>(face.finerCoverageMask);
	}
	return sig;
}

bool TerrainMeshSeamSet::hasTransition() const
{
	for (const auto& face : faces)
	{
		if (face.mode == TerrainMeshSeamMode::StitchToCoarser)
			return true;
	}
	return false;
}

bool TerrainMeshRequest::isValid() const
{
	return region.cellSize > 0
		&& meshCellCount > 0
		&& sampleStride > 0
		&& sampleLodPower >= 0
		&& voxelSize > 0
		&& blockSize > 0.0f
		&& domainSize.x > 0
		&& domainSize.y > 0
		&& domainSize.z > 0
		&& seamSignature == seams.signature()
		&& region.sampleStride(meshCellCount) == sampleStride
		&& region.sampleLodPower(meshCellCount) == sampleLodPower;
}

TerrainMeshRequest makeTerrainMeshRequest(const TerrainOctreeNode& node,
	const TerrainOctreeConfig& config, int revision)
{
	return makeTerrainMeshRequest(node, config, revision, TerrainMeshSeamSet());
}

TerrainMeshRequest makeTerrainMeshRequest(const TerrainOctreeNode& node,
	const TerrainOctreeConfig& config, int revision,
	const TerrainMeshSeamSet& seams)
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
	request.domainSize = config.domainSize;
	request.seams = seams;
	request.seamSignature = request.seams.signature();
	return request;
}

} // namespace tzw
