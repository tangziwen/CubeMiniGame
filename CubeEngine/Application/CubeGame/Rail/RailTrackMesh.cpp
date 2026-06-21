#include "RailTrackMesh.h"

#include <algorithm>
#include <cmath>

namespace tzw {

namespace
{
constexpr float RailHalfGauge = 0.34f;

Quaternion sleeperRotationForTangent(const vec3& tangent)
{
	Quaternion rotation;
	rotation.fromDirection(vec3(tangent.x, 0.0f, tangent.z));
	return rotation;
}

vec3 railSideOffset(const vec3& tangent)
{
	const vec3 horizontalTangent = safeNormalized(vec3(tangent.x, 0.0f, tangent.z), vec3(1.0f, 0.0f, 0.0f));
	return vec3(horizontalTangent.z, 0.0f, -horizontalTangent.x) * RailHalfGauge;
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

		appendSegmentVisual(segment, config, outData);
	}
}

void RailTrackMesh::appendSegmentVisual(const RailSegment& segment, const RailConfig& config, RailTrackVisualData& outData) const
{
	if (segment.cachedLength <= 0.0001f)
	{
		return;
	}

	const float sampleSpacing = std::max(config.visualSampleSpacing * 0.5f, 0.1f);
	const int lineSteps = std::max(1, static_cast<int>(std::ceil(segment.cachedLength / sampleSpacing)));
	vec3 prev = segment.positionByDistance(0.0f);
	vec3 prevOffset = railSideOffset(segment.tangentByDistance(0.0f));
	for (int i = 1; i <= lineSteps; ++i)
	{
		const float distance = segment.cachedLength * (static_cast<float>(i) / static_cast<float>(lineSteps));
		const vec3 curr = segment.positionByDistance(distance);
		const vec3 currOffset = railSideOffset(segment.tangentByDistance(distance));

		RailLineVisual leftLine;
		leftLine.start = prev + prevOffset + vec3(0.0f, 0.08f, 0.0f);
		leftLine.end = curr + currOffset + vec3(0.0f, 0.08f, 0.0f);
		leftLine.color = vec3(0.82f, 0.82f, 0.88f);
		outData.lines.push_back(leftLine);

		RailLineVisual rightLine;
		rightLine.start = prev - prevOffset + vec3(0.0f, 0.08f, 0.0f);
		rightLine.end = curr - currOffset + vec3(0.0f, 0.08f, 0.0f);
		rightLine.color = vec3(0.82f, 0.82f, 0.88f);
		outData.lines.push_back(rightLine);

		prev = curr;
		prevOffset = currOffset;
	}

	const float sleeperSpacing = std::max(config.sleeperSpacing, 0.1f);
	const int sleeperCount = std::max(1, static_cast<int>(std::floor(segment.cachedLength / sleeperSpacing)));
	for (int i = 0; i <= sleeperCount; ++i)
	{
		const float distance = std::min(segment.cachedLength, i * sleeperSpacing);
		RailSleeperVisual sleeper;
		sleeper.position = segment.positionByDistance(distance) + vec3(0.0f, 0.03f, 0.0f);
		sleeper.rotation = sleeperRotationForTangent(segment.tangentByDistance(distance));
		outData.sleepers.push_back(sleeper);
	}
}

} // namespace tzw
