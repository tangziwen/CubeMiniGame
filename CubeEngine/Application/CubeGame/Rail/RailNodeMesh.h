#pragma once

#include "RailNetwork.h"
#include "EngineSrc/Math/vec3.h"

#include <vector>

namespace tzw {

struct RailNodeVisual
{
	RailNodeId nodeId = InvalidRailNodeId;
	vec3 position;
};

class RailNodeMesh
{
public:
	void build(const RailNetwork& network, const RailConfig& config, std::vector<RailNodeVisual>& outData) const;
};

} // namespace tzw
