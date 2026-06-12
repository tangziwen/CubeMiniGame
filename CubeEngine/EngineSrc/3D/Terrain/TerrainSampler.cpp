#include "TerrainSampler.h"

#include "CubeGame/GameConfig.h"

namespace tzw {

void TerrainSampleBuffer::clear()
{
	voxels.clear();
	voxelSize = 0;
	meshCellCount = 0;
	sampleStride = 0;
	sampleLodPower = -1;
	minPadding = 0;
	maxPadding = 0;
	sampleOrigin = TerrainInt3();
	region = TerrainRegion();
}

bool TerrainSampleBuffer::isValid() const
{
	return voxelSize > 0
		&& meshCellCount > 0
		&& sampleStride > 0
		&& sampleLodPower >= 0
		&& voxels.size() == static_cast<size_t>(voxelSize * voxelSize * voxelSize);
}

int TerrainSampleBuffer::index(int x, int y, int z) const
{
	return x * voxelSize * voxelSize + y * voxelSize + z;
}

TerrainSampler::TerrainSampler()
	: m_domainSize(GAME_MAP_WIDTH_VOXELS, GAME_MAP_HEIGHT_VOXELS, GAME_MAP_DEPTH_VOXELS)
{
}

TerrainSampler::TerrainSampler(const TerrainInt3& domainSize)
	: m_domainSize(domainSize)
{
}

void TerrainSampler::setDomainSize(const TerrainInt3& domainSize)
{
	m_domainSize = domainSize;
}

const TerrainInt3& TerrainSampler::domainSize() const
{
	return m_domainSize;
}

bool TerrainSampler::sample(const TerrainMeshRequest& request, TerrainSampleBuffer& outBuffer) const
{
	return sample(GameMap::shared(), request, outBuffer);
}

bool TerrainSampler::sample(GameMap* map, const TerrainMeshRequest& request,
	TerrainSampleBuffer& outBuffer) const
{
	outBuffer.clear();
	if (!map || !request.isValid())
	{
		return false;
	}

	outBuffer.voxelSize = request.voxelSize;
	outBuffer.meshCellCount = request.meshCellCount;
	outBuffer.sampleStride = request.sampleStride;
	outBuffer.sampleLodPower = request.sampleLodPower;
	outBuffer.minPadding = MIN_PADDING;
	outBuffer.maxPadding = MAX_PADDING;
	outBuffer.region = request.region;
	outBuffer.sampleOrigin = request.region.sampleMin()
		- TerrainInt3(MIN_PADDING, MIN_PADDING, MIN_PADDING) * request.sampleStride;
	outBuffer.voxels.resize(static_cast<size_t>(outBuffer.voxelSize * outBuffer.voxelSize * outBuffer.voxelSize));

	for (int x = 0; x < outBuffer.voxelSize; ++x)
	{
		for (int z = 0; z < outBuffer.voxelSize; ++z)
		{
			for (int y = 0; y < outBuffer.voxelSize; ++y)
			{
				const TerrainInt3 globalSample = outBuffer.sampleOrigin
					+ TerrainInt3(x, y, z) * request.sampleStride;
				const int index = outBuffer.index(x, y, z);
				outBuffer.voxels[index] = isInDomain(globalSample)
					? map->sampleVoxel(globalSample.x, globalSample.y, globalSample.z)
					: makeEmptyVoxel();
			}
		}
	}
	return true;
}

bool TerrainSampler::isInDomain(const TerrainInt3& sample) const
{
	return sample.x >= 0 && sample.y >= 0 && sample.z >= 0
		&& sample.x < m_domainSize.x
		&& sample.y < m_domainSize.y
		&& sample.z < m_domainSize.z;
}

voxelInfo TerrainSampler::makeEmptyVoxel() const
{
	voxelInfo result;
	result.w = 255;
	result.matInfo.matIndex1 = 0;
	result.matInfo.matIndex2 = 0;
	result.matInfo.matIndex3 = 0;
	result.matInfo.matBlendFactor = 0;
	return result;
}

} // namespace tzw
