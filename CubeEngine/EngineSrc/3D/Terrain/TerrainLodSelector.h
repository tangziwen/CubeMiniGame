#pragma once

#include "TerrainOctreeNode.h"

namespace tzw {

enum class TerrainLodDecision
{
	RenderSelf,
	Subdivide,
	KeepParentFallback,
	DelayLoad,
};

struct TerrainLodContext
{
	vec3 viewerPosition;
	float splitDistanceScale = 2.5f;
	int maxDepthOverride = -1;
	bool forceLeaf = false;
};

class TerrainLodSelector
{
public:
	TerrainLodDecision select(const TerrainOctreeNode& node, const TerrainOctreeConfig& config,
		const TerrainLodContext& context) const;
};

} // namespace tzw
