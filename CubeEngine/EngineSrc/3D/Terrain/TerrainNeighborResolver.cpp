#include "TerrainNeighborResolver.h"

namespace tzw {

namespace
{
	int component(const TerrainInt3& value, int axis)
	{
		switch (axis)
		{
		case 0:
			return value.x;
		case 1:
			return value.y;
		default:
			return value.z;
		}
	}

	bool rangesOverlapHalfOpen(int minA, int maxA, int minB, int maxB)
	{
		return minA < maxB && minB < maxA;
	}

	void faceTangentialAxes(TerrainNeighborFace face, int& tangent0, int& tangent1)
	{
		const int axis = static_cast<int>(face) / 2;
		tangent0 = (axis + 1) % 3;
		tangent1 = (axis + 2) % 3;
	}

	uint8_t finerCoverageMaskForFace(const TerrainRegion& selfRegion,
		const TerrainRegion& finerRegion, TerrainNeighborFace face)
	{
		const TerrainInt3 selfMax = selfRegion.cellMaxExclusive();
		const TerrainInt3 finerMax = finerRegion.cellMaxExclusive();
		int tangent0 = 0;
		int tangent1 = 0;
		faceTangentialAxes(face, tangent0, tangent1);

		const int min0 = component(selfRegion.voxelMin, tangent0);
		const int max0 = component(selfMax, tangent0);
		const int mid0 = (min0 + max0) / 2;
		const int min1 = component(selfRegion.voxelMin, tangent1);
		const int max1 = component(selfMax, tangent1);
		const int mid1 = (min1 + max1) / 2;

		const int finerMin0 = component(finerRegion.voxelMin, tangent0);
		const int finerMax0 = component(finerMax, tangent0);
		const int finerMin1 = component(finerRegion.voxelMin, tangent1);
		const int finerMax1 = component(finerMax, tangent1);

		uint8_t mask = 0;
		for (int half1 = 0; half1 < 2; ++half1)
		{
			const int halfMin1 = half1 == 0 ? min1 : mid1;
			const int halfMax1 = half1 == 0 ? mid1 : max1;
			if (!rangesOverlapHalfOpen(halfMin1, halfMax1, finerMin1, finerMax1))
			{
				continue;
			}
			for (int half0 = 0; half0 < 2; ++half0)
			{
				const int halfMin0 = half0 == 0 ? min0 : mid0;
				const int halfMax0 = half0 == 0 ? mid0 : max0;
				if (rangesOverlapHalfOpen(halfMin0, halfMax0, finerMin0, finerMax0))
				{
					mask |= static_cast<uint8_t>(1u << (half0 + half1 * 2));
				}
			}
		}
		return mask;
	}
}

TerrainNeighborRelation TerrainNeighborResolver::findNeighbor(const TerrainOctree& octree,
	const TerrainRenderSet& renderSet, const TerrainOctreeNode& node,
	TerrainNeighborFace face) const
{
	TerrainNeighborRelation relation = makeMissingRelation(node, face);
	TerrainNodeKey candidate;
	if (!offsetKey(node.key(), face, candidate))
	{
		relation.isBoundary = true;
		return relation;
	}

	if (TerrainOctreeNode* sameLevelNode = renderSet.findNode(candidate))
	{
		return makeRelation(node, sameLevelNode, face);
	}

	std::vector<TerrainNeighborRelation> finerNeighbors = findFinerNeighbors(renderSet, node, face);
	if (!finerNeighbors.empty())
	{
		relation.exists = true;
		relation.key = candidate;
		relation.levelDelta = finerNeighbors.front().levelDelta;
		relation.isFiner = true;
		relation.finerNeighborCount = static_cast<int>(finerNeighbors.size());
		for (const TerrainNeighborRelation& finerNeighbor : finerNeighbors)
		{
			if (finerNeighbor.node)
			{
				relation.finerCoverageMask |= finerCoverageMaskForFace(
					node.region(), finerNeighbor.node->region(), face);
			}
		}
		return relation;
	}

	TerrainNodeKey ancestor = node.key();
	while (!ancestor.isRoot())
	{
		ancestor = ancestor.parent();
		TerrainNodeKey coarserCandidate;
		if (!offsetKey(ancestor, face, coarserCandidate))
		{
			continue;
		}
		if (TerrainOctreeNode* coarserNode = renderSet.findNode(coarserCandidate))
		{
			return makeRelation(node, coarserNode, face);
		}
	}

	(void)octree;
	return relation;
}

std::array<TerrainNeighborRelation, 6> TerrainNeighborResolver::findNeighbors(
	const TerrainOctree& octree, const TerrainRenderSet& renderSet,
	const TerrainOctreeNode& node) const
{
	std::array<TerrainNeighborRelation, 6> result;
	for (int i = 0; i < static_cast<int>(TerrainNeighborFace::Count); ++i)
	{
		result[i] = findNeighbor(octree, renderSet, node, static_cast<TerrainNeighborFace>(i));
	}
	return result;
}

std::vector<TerrainNeighborRelation> TerrainNeighborResolver::findFinerNeighbors(
	const TerrainRenderSet& renderSet, const TerrainOctreeNode& node,
	TerrainNeighborFace face) const
{
	std::vector<TerrainNeighborRelation> result;
	for (TerrainOctreeNode* candidate : renderSet.nodes())
	{
		if (!candidate || candidate == &node || candidate->key().level <= node.key().level)
		{
			continue;
		}
		if (touchesFace(node.region(), candidate->region(), face))
		{
			result.push_back(makeRelation(node, candidate, face));
		}
	}
	return result;
}

TerrainNeighborRelation TerrainNeighborResolver::makeMissingRelation(
	const TerrainOctreeNode& node, TerrainNeighborFace face) const
{
	TerrainNeighborRelation relation;
	relation.face = face;
	relation.key = node.key();
	return relation;
}

TerrainNeighborRelation TerrainNeighborResolver::makeRelation(const TerrainOctreeNode& self,
	TerrainOctreeNode* neighbor, TerrainNeighborFace face) const
{
	TerrainNeighborRelation relation = makeMissingRelation(self, face);
	if (!neighbor)
	{
		return relation;
	}

	relation.node = neighbor;
	relation.key = neighbor->key();
	relation.exists = true;
	relation.levelDelta = neighbor->key().level - self.key().level;
	relation.isSameLevel = relation.levelDelta == 0;
	relation.isCoarser = relation.levelDelta < 0;
	relation.isFiner = relation.levelDelta > 0;
	relation.finerNeighborCount = relation.isFiner ? 1 : 0;
	return relation;
}

bool TerrainNeighborResolver::offsetKey(const TerrainNodeKey& key, TerrainNeighborFace face,
	TerrainNodeKey& outKey) const
{
	outKey = key;
	switch (face)
	{
	case TerrainNeighborFace::NegativeX:
		--outKey.x;
		break;
	case TerrainNeighborFace::PositiveX:
		++outKey.x;
		break;
	case TerrainNeighborFace::NegativeY:
		--outKey.y;
		break;
	case TerrainNeighborFace::PositiveY:
		++outKey.y;
		break;
	case TerrainNeighborFace::NegativeZ:
		--outKey.z;
		break;
	case TerrainNeighborFace::PositiveZ:
		++outKey.z;
		break;
	default:
		return false;
	}

	const int maxCoord = 1 << key.level;
	return outKey.x >= 0 && outKey.y >= 0 && outKey.z >= 0
		&& outKey.x < maxCoord && outKey.y < maxCoord && outKey.z < maxCoord;
}

bool TerrainNeighborResolver::touchesFace(const TerrainRegion& selfRegion,
	const TerrainRegion& otherRegion, TerrainNeighborFace face) const
{
	const TerrainInt3 selfMax = selfRegion.cellMaxExclusive();
	const TerrainInt3 otherMax = otherRegion.cellMaxExclusive();
	switch (face)
	{
	case TerrainNeighborFace::NegativeX:
		return otherMax.x == selfRegion.voxelMin.x
			&& rangesOverlap(selfRegion.voxelMin.y, selfMax.y, otherRegion.voxelMin.y, otherMax.y)
			&& rangesOverlap(selfRegion.voxelMin.z, selfMax.z, otherRegion.voxelMin.z, otherMax.z);
	case TerrainNeighborFace::PositiveX:
		return otherRegion.voxelMin.x == selfMax.x
			&& rangesOverlap(selfRegion.voxelMin.y, selfMax.y, otherRegion.voxelMin.y, otherMax.y)
			&& rangesOverlap(selfRegion.voxelMin.z, selfMax.z, otherRegion.voxelMin.z, otherMax.z);
	case TerrainNeighborFace::NegativeY:
		return otherMax.y == selfRegion.voxelMin.y
			&& rangesOverlap(selfRegion.voxelMin.x, selfMax.x, otherRegion.voxelMin.x, otherMax.x)
			&& rangesOverlap(selfRegion.voxelMin.z, selfMax.z, otherRegion.voxelMin.z, otherMax.z);
	case TerrainNeighborFace::PositiveY:
		return otherRegion.voxelMin.y == selfMax.y
			&& rangesOverlap(selfRegion.voxelMin.x, selfMax.x, otherRegion.voxelMin.x, otherMax.x)
			&& rangesOverlap(selfRegion.voxelMin.z, selfMax.z, otherRegion.voxelMin.z, otherMax.z);
	case TerrainNeighborFace::NegativeZ:
		return otherMax.z == selfRegion.voxelMin.z
			&& rangesOverlap(selfRegion.voxelMin.x, selfMax.x, otherRegion.voxelMin.x, otherMax.x)
			&& rangesOverlap(selfRegion.voxelMin.y, selfMax.y, otherRegion.voxelMin.y, otherMax.y);
	case TerrainNeighborFace::PositiveZ:
		return otherRegion.voxelMin.z == selfMax.z
			&& rangesOverlap(selfRegion.voxelMin.x, selfMax.x, otherRegion.voxelMin.x, otherMax.x)
			&& rangesOverlap(selfRegion.voxelMin.y, selfMax.y, otherRegion.voxelMin.y, otherMax.y);
	default:
		return false;
	}
}

bool TerrainNeighborResolver::rangesOverlap(int minA, int maxA, int minB, int maxB) const
{
	return minA < maxB && minB < maxA;
}

} // namespace tzw
