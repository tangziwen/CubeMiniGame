#include "RailSpline.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace tzw {

namespace
{
float clamp01(float value)
{
	return std::max(0.0f, std::min(1.0f, value));
}

float clampDistance(float value, float maxValue)
{
	return std::max(0.0f, std::min(maxValue, value));
}

float distancePointToSegment(const vec3& point, const vec3& start, const vec3& end, float& outT)
{
	const vec3 segment = end - start;
	const float lenSq = segment.squaredLength();
	if (lenSq <= 0.000001f)
	{
		outT = 0.0f;
		return point.distance(start);
	}

	outT = clamp01(vec3::DotProduct(point - start, segment) / lenSq);
	const vec3 closest = start + segment * outT;
	return point.distance(closest);
}
}

RailSpline::RailSpline()
{
	rebuildArcLengthTable();
}

RailSpline::RailSpline(const vec3& start, const vec3& end, const vec3& startTangent, const vec3& endTangent)
{
	configure(start, end, startTangent, endTangent);
}

void RailSpline::configure(const vec3& start, const vec3& end, const vec3& startTangent, const vec3& endTangent)
{
	m_start = start;
	m_end = end;
	m_startTangent = startTangent;
	m_endTangent = endTangent;
	rebuildArcLengthTable();
}

const vec3& RailSpline::start() const
{
	return m_start;
}

const vec3& RailSpline::end() const
{
	return m_end;
}

const vec3& RailSpline::startTangent() const
{
	return m_startTangent;
}

const vec3& RailSpline::endTangent() const
{
	return m_endTangent;
}

float RailSpline::length() const
{
	return m_length;
}

vec3 RailSpline::positionByDistance(float distance) const
{
	return positionByT(tByDistance(distance));
}

vec3 RailSpline::tangentByDistance(float distance) const
{
	const vec3 chord = safeNormalized(m_end - m_start, vec3(1.0f, 0.0f, 0.0f));
	return safeNormalized(tangentByT(tByDistance(distance)), chord);
}

RailSplineNearestPoint RailSpline::nearestPoint(const vec3& point, int sampleCount) const
{
	RailSplineNearestPoint result;
	result.distanceToPoint = std::numeric_limits<float>::max();

	if (m_length <= 0.0001f)
	{
		result.position = m_start;
		result.distanceToPoint = point.distance(m_start);
		return result;
	}

	const int steps = std::max(sampleCount, 2);
	vec3 prevPos = positionByDistance(0.0f);
	float prevDistance = 0.0f;
	for (int i = 1; i <= steps; ++i)
	{
		const float currDistance = m_length * (static_cast<float>(i) / static_cast<float>(steps));
		const vec3 currPos = positionByDistance(currDistance);
		float segmentT = 0.0f;
		const float pointDistance = distancePointToSegment(point, prevPos, currPos, segmentT);
		if (pointDistance < result.distanceToPoint)
		{
			result.distanceToPoint = pointDistance;
			result.distanceOnSpline = prevDistance + (currDistance - prevDistance) * segmentT;
			result.position = prevPos + (currPos - prevPos) * segmentT;
		}
		prevPos = currPos;
		prevDistance = currDistance;
	}

	return result;
}

vec3 RailSpline::positionByT(float t) const
{
	t = clamp01(t);
	const float t2 = t * t;
	const float t3 = t2 * t;
	const float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
	const float h10 = t3 - 2.0f * t2 + t;
	const float h01 = -2.0f * t3 + 3.0f * t2;
	const float h11 = t3 - t2;
	return m_start * h00 + m_startTangent * h10 + m_end * h01 + m_endTangent * h11;
}

vec3 RailSpline::tangentByT(float t) const
{
	t = clamp01(t);
	const float t2 = t * t;
	const float h00 = 6.0f * t2 - 6.0f * t;
	const float h10 = 3.0f * t2 - 4.0f * t + 1.0f;
	const float h01 = -6.0f * t2 + 6.0f * t;
	const float h11 = 3.0f * t2 - 2.0f * t;
	return m_start * h00 + m_startTangent * h10 + m_end * h01 + m_endTangent * h11;
}

float RailSpline::tByDistance(float distance) const
{
	if (m_arcSamples.empty() || m_length <= 0.0001f)
	{
		return 0.0f;
	}

	distance = clampDistance(distance, m_length);
	for (size_t i = 1; i < m_arcSamples.size(); ++i)
	{
		const ArcSample& prev = m_arcSamples[i - 1];
		const ArcSample& curr = m_arcSamples[i];
		if (distance <= curr.distance)
		{
			const float span = curr.distance - prev.distance;
			if (span <= 0.000001f)
			{
				return curr.t;
			}
			const float localT = (distance - prev.distance) / span;
			return prev.t + (curr.t - prev.t) * localT;
		}
	}
	return 1.0f;
}

void RailSpline::rebuildArcLengthTable()
{
	m_arcSamples.clear();
	m_arcSamples.reserve(33);
	m_length = 0.0f;

	ArcSample first;
	first.t = 0.0f;
	first.distance = 0.0f;
	m_arcSamples.push_back(first);

	vec3 prev = positionByT(0.0f);
	const int sampleCount = 32;
	for (int i = 1; i <= sampleCount; ++i)
	{
		const float t = static_cast<float>(i) / static_cast<float>(sampleCount);
		const vec3 curr = positionByT(t);
		m_length += curr.distance(prev);

		ArcSample sample;
		sample.t = t;
		sample.distance = m_length;
		m_arcSamples.push_back(sample);
		prev = curr;
	}
}

} // namespace tzw
