#include "RailTrackMesh.h"

#include <algorithm>
#include <cmath>

namespace tzw {

namespace
{
vec3 safeNormalized(const vec3& value, const vec3& fallback)
{
	const float length = value.length();
	if (length <= 0.0001f)
	{
		return fallback;
	}
	return value / length;
}

Quaternion sleeperRotationForTangent(const vec3& tangent)
{
	const vec3 forward = safeNormalized(vec3(tangent.x, 0.0f, tangent.z), vec3(0.0f, 0.0f, -1.0f));
	const vec3 up(0.0f, 1.0f, 0.0f);
	const vec3 right = safeNormalized(vec3::CrossProduct(up, forward), vec3(1.0f, 0.0f, 0.0f));
	Quaternion rotation;
	rotation.fromAxises(right, up, forward);
	rotation.normalize();
	return rotation;
}
}

void RailTrackMesh::build(const RailNetwork& network, const RailConfig& config, RailTrackVisualData& outData) const
{
	outData.lines.clear();
	outData.sleepers.clear();

	for (const auto& pair : network.segments())
	{
		const RailSegment& segment = pair.second;
		if (segment.cachedLength <= 0.0001f)
		{
			continue;
		}

		const float sampleSpacing = std::max(config.visualSampleSpacing, 0.1f);
		const int lineSteps = std::max(1, static_cast<int>(std::ceil(segment.cachedLength / sampleSpacing)));
		vec3 prev = segment.positionByDistance(0.0f);
		for (int i = 1; i <= lineSteps; ++i)
		{
			const float distance = segment.cachedLength * (static_cast<float>(i) / static_cast<float>(lineSteps));
			const vec3 curr = segment.positionByDistance(distance);
			RailLineVisual line;
			line.start = prev + vec3(0.0f, 0.04f, 0.0f);
			line.end = curr + vec3(0.0f, 0.04f, 0.0f);
			line.color = vec3(0.85f, 0.85f, 0.9f);
			outData.lines.push_back(line);
			prev = curr;
		}

		const float sleeperSpacing = std::max(config.sleeperSpacing, 0.1f);
		const int sleeperCount = std::max(1, static_cast<int>(std::floor(segment.cachedLength / sleeperSpacing)));
		for (int i = 0; i <= sleeperCount; ++i)
		{
			const float distance = std::min(segment.cachedLength, i * sleeperSpacing);
			RailSleeperVisual sleeper;
			sleeper.position = segment.positionByDistance(distance) + vec3(0.0f, 0.02f, 0.0f);
			sleeper.rotation = sleeperRotationForTangent(segment.tangentByDistance(distance));
			outData.sleepers.push_back(sleeper);
		}
	}
}

} // namespace tzw
