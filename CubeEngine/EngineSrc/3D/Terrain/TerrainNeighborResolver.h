#pragma once

#include "TerrainOctree.h"

#include <array>
#include <cstdint>
#include <vector>

namespace tzw {

enum class TerrainNeighborFace
{
	NegativeX = 0,
	PositiveX,
	NegativeY,
	PositiveY,
	NegativeZ,
	PositiveZ,
	Count
};

struct TerrainNeighborRelation
{
	TerrainNeighborFace face = TerrainNeighborFace::NegativeX;
	TerrainOctreeNode* node = nullptr;
	TerrainNodeKey key;
	bool exists = false;
	int levelDelta = 0;
	bool isSameLevel = false;
	bool isCoarser = false;
	bool isFiner = false;
	bool isBoundary = false;
	int finerNeighborCount = 0;
	uint8_t finerCoverageMask = 0;
};

class TerrainNeighborResolver
{
public:
	TerrainNeighborRelation findNeighbor(const TerrainOctree& octree,
		const TerrainRenderSet& renderSet, const TerrainOctreeNode& node,
		TerrainNeighborFace face) const;

	std::array<TerrainNeighborRelation, 6> findNeighbors(const TerrainOctree& octree,
		const TerrainRenderSet& renderSet, const TerrainOctreeNode& node) const;

	std::vector<TerrainNeighborRelation> findFinerNeighbors(const TerrainRenderSet& renderSet,
		const TerrainOctreeNode& node, TerrainNeighborFace face) const;

private:
	TerrainNeighborRelation makeMissingRelation(const TerrainOctreeNode& node,
		TerrainNeighborFace face) const;
	TerrainNeighborRelation makeRelation(const TerrainOctreeNode& self,
		TerrainOctreeNode* neighbor, TerrainNeighborFace face) const;
	bool offsetKey(const TerrainNodeKey& key, TerrainNeighborFace face,
		TerrainNodeKey& outKey) const;
	bool touchesFace(const TerrainRegion& selfRegion, const TerrainRegion& otherRegion,
		TerrainNeighborFace face) const;
	bool rangesOverlap(int minA, int maxA, int minB, int maxB) const;
};

} // namespace tzw
