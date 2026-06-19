#pragma once

#include "TerrainOctreeTypes.h"

#include <array>
#include <cstdint>

namespace tzw {

class TerrainOctreeNode;

enum class TerrainMeshSeamMode : uint8_t
{
	None = 0,
	Boundary,
	SameLevel,
	StitchToCoarser,
	SuppressForFiner,
	Invalid
};

struct TerrainMeshSeamFace
{
	TerrainMeshSeamMode mode = TerrainMeshSeamMode::None;
	int levelDelta = 0;

	bool operator==(const TerrainMeshSeamFace& other) const;
	bool operator!=(const TerrainMeshSeamFace& other) const;
};

// Face order matches TerrainNeighborFace:
// NegativeX, PositiveX, NegativeY, PositiveY, NegativeZ, PositiveZ.
struct TerrainMeshSeamSet
{
	std::array<TerrainMeshSeamFace, 6> faces;

	bool operator==(const TerrainMeshSeamSet& other) const;
	bool operator!=(const TerrainMeshSeamSet& other) const;

	uint32_t signature() const;
	bool hasTransition() const;
};

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

	TerrainMeshSeamSet seams;
	uint32_t seamSignature = 0;
	TerrainInt3 domainSize;

	bool isValid() const;
};

TerrainMeshRequest makeTerrainMeshRequest(const TerrainOctreeNode& node,
	const TerrainOctreeConfig& config, int revision);

TerrainMeshRequest makeTerrainMeshRequest(const TerrainOctreeNode& node,
	const TerrainOctreeConfig& config, int revision,
	const TerrainMeshSeamSet& seams);

} // namespace tzw
