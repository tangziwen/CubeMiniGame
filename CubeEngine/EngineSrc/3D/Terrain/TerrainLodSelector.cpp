#include "TerrainLodSelector.h"

#include <algorithm>

namespace tzw {

TerrainLodDecision TerrainLodSelector::select(const TerrainOctreeNode& node, const TerrainOctreeConfig& config,
	const TerrainLodContext& context) const
{
	if (!config.isValid() || !node.canRender())
	{
		return TerrainLodDecision::DelayLoad;
	}

	if (context.maxViewDistance > 0.0f)
	{
		const AABB worldBounds = node.region().worldAABB(config.mapOffset, config.blockSize);
		const float maxViewDistanceSq = context.maxViewDistance * context.maxViewDistance;
		if (worldBounds.distanceSquaredToPoint(context.viewerPosition) > maxViewDistanceSq)
		{
			return TerrainLodDecision::DelayLoad;
		}
	}

	const int maxDepth = context.maxDepthOverride >= 0
		? std::min(context.maxDepthOverride, config.maxDepth)
		: config.maxDepth;
	if (context.forceLeaf && node.key().level < maxDepth)
	{
		return TerrainLodDecision::Subdivide;
	}
	if (node.key().level >= maxDepth || !node.canSubdivide(config))
	{
		return TerrainLodDecision::RenderSelf;
	}

	const vec3 center = node.region().worldCenter(config.mapOffset, config.blockSize);
	const float distance = center.distance(context.viewerPosition);
	const float splitDistance = static_cast<float>(node.region().cellSize) * config.blockSize * context.splitDistanceScale;
	return distance < splitDistance ? TerrainLodDecision::Subdivide : TerrainLodDecision::RenderSelf;
}

} // namespace tzw
