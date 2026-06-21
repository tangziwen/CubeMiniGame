#pragma once

#include "EngineSrc/Math/vec3.h"
#include <vector>

namespace tzw {

struct RailSplineNearestPoint
{
	float distanceOnSpline = 0.0f;
	float distanceToPoint = 0.0f;
	vec3 position;
};

class RailSpline
{
public:
	RailSpline();
	RailSpline(const vec3& start, const vec3& end, const vec3& startTangent, const vec3& endTangent);

	void configure(const vec3& start, const vec3& end, const vec3& startTangent, const vec3& endTangent);
	const vec3& start() const;
	const vec3& end() const;
	const vec3& startTangent() const;
	const vec3& endTangent() const;
	float length() const;

	vec3 positionByDistance(float distance) const;
	vec3 tangentByDistance(float distance) const;
	RailSplineNearestPoint nearestPoint(const vec3& point, int sampleCount = 64) const;

private:
	struct ArcSample
	{
		float t = 0.0f;
		float distance = 0.0f;
	};

	vec3 positionByT(float t) const;
	vec3 tangentByT(float t) const;
	float tByDistance(float distance) const;
	void rebuildArcLengthTable();

	vec3 m_start;
	vec3 m_end;
	vec3 m_startTangent;
	vec3 m_endTangent;
	std::vector<ArcSample> m_arcSamples;
	float m_length = 0.0f;
};

} // namespace tzw
