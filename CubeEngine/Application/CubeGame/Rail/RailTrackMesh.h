#pragma once

#include "RailNetwork.h"
#include "EngineSrc/Math/Quaternion.h"

#include <vector>

namespace tzw {

struct RailLineVisual
{
	vec3 start;
	vec3 end;
	vec3 color;
};

struct RailSleeperVisual
{
	vec3 position;
	Quaternion rotation;
};

struct RailTrackVisualData
{
	std::vector<RailLineVisual> lines;
	std::vector<RailSleeperVisual> sleepers;
};

class RailTrackMesh
{
public:
	void build(const RailNetwork& network, const RailConfig& config, RailTrackVisualData& outData) const;
	void appendSegmentVisual(const RailSegment& segment, const RailConfig& config, RailTrackVisualData& outData) const;
};

} // namespace tzw
