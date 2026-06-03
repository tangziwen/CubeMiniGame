#pragma once

#include "TerrainMeshRequest.h"
#include "CubeGame/GameMap.h"

#include <vector>

namespace tzw {

struct TerrainSampleBuffer
{
	std::vector<voxelInfo> voxels;
	int voxelSize = 0;
	int meshCellCount = 0;
	int sampleStride = 0;
	int sampleLodPower = -1;
	int minPadding = 0;
	int maxPadding = 0;
	TerrainInt3 sampleOrigin;
	TerrainRegion region;

	void clear();
	bool isValid() const;
	int index(int x, int y, int z) const;
};

class TerrainSampler
{
public:
	TerrainSampler();
	explicit TerrainSampler(const TerrainInt3& domainSize);

	void setDomainSize(const TerrainInt3& domainSize);
	const TerrainInt3& domainSize() const;

	bool sample(const TerrainMeshRequest& request, TerrainSampleBuffer& outBuffer) const;
	bool sample(GameMap* map, const TerrainMeshRequest& request, TerrainSampleBuffer& outBuffer) const;

private:
	bool isInDomain(const TerrainInt3& sample) const;
	voxelInfo makeEmptyVoxel() const;

	TerrainInt3 m_domainSize;
};

} // namespace tzw
