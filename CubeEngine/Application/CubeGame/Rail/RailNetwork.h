#pragma once

#include "RailSpline.h"
#include "EngineSrc/Math/vec3.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace tzw {

using RailNodeId = int;
using RailSegmentId = int;

constexpr RailNodeId InvalidRailNodeId = -1;
constexpr RailSegmentId InvalidRailSegmentId = -1;

struct RailConfig
{
	float minSegmentLength = 0.2f;
	float maxSegmentLength = 10000.0f;
	float minTurnRadius = 0.5f;
	float maxSlope = 10.0f;
	float nodePickRadius = 0.8f;
	float segmentPickRadius = 0.8f;
	float visualSampleSpacing = 0.75f;
	float sleeperSpacing = 1.0f;
};

class RailNode
{
public:
	virtual ~RailNode();
	virtual bool canAutoRemoveWhenIsolated() const;

	RailNodeId id = InvalidRailNodeId;
	vec3 position;
	vec3 preferredTangent = vec3(1.0f, 0.0f, 0.0f);
	bool isConnectable = true;
	std::vector<RailSegmentId> segments;
};

struct RailSegment
{
	RailSegmentId id = InvalidRailSegmentId;
	RailNodeId startNode = InvalidRailNodeId;
	RailNodeId endNode = InvalidRailNodeId;
	vec3 startTangent;
	vec3 endTangent;
	RailSpline spline;
	float cachedLength = 0.0f;

	vec3 positionByDistance(float distance) const;
	vec3 tangentByDistance(float distance) const;
};

struct RailNodePick
{
	RailNodeId nodeId = InvalidRailNodeId;
	float distance = 0.0f;
};

struct RailSegmentPick
{
	RailSegmentId segmentId = InvalidRailSegmentId;
	float distanceOnSegment = 0.0f;
	float distance = 0.0f;
	vec3 position;
};

struct RailSplitResult
{
	RailNodeId nodeId = InvalidRailNodeId;
	RailSegmentId firstSegmentId = InvalidRailSegmentId;
	RailSegmentId secondSegmentId = InvalidRailSegmentId;
};

class RailNetwork
{
public:
	RailNetwork();

	RailNodeId createNode(const vec3& position, const vec3& preferredTangent = vec3(1.0f, 0.0f, 0.0f),
		bool isConnectable = true);
	RailSegmentId createSegment(RailNodeId startNode, RailNodeId endNode);
	RailNodeId unserializeNode(RailNodeId nodeId, const vec3& position, const vec3& preferredTangent, bool isConnectable);
	RailSegmentId unserializeSegment(RailSegmentId segmentId, RailNodeId startNode, RailNodeId endNode,
		const vec3& startTangent, const vec3& endTangent);
	bool deleteSegment(RailSegmentId segmentId);
	bool splitSegment(RailSegmentId segmentId, float distanceOnSegment, RailSplitResult& outResult);

	RailNode* node(RailNodeId nodeId);
	const RailNode* node(RailNodeId nodeId) const;
	RailSegment* segment(RailSegmentId segmentId);
	const RailSegment* segment(RailSegmentId segmentId) const;
	const std::unordered_map<RailNodeId, std::unique_ptr<RailNode>>& nodes() const;
	const std::unordered_map<RailSegmentId, RailSegment>& segments() const;

	RailNodePick findNearestNode(const vec3& point, float maxDistance) const;
	RailSegmentPick findNearestSegment(const vec3& point, float maxDistance) const;
	void deleteIsolatedOrdinaryNodes();
	void clear();

private:
	RailSegmentId createSegmentWithTangents(RailNodeId startNode, RailNodeId endNode,
		const vec3& startTangent, const vec3& endTangent);
	bool deleteSegmentInternal(RailSegmentId segmentId, bool cleanupIsolatedNodes);
	bool deleteNodeIfIsolated(RailNodeId nodeId);
	void attachSegmentToNode(RailNodeId nodeId, RailSegmentId segmentId);
	void detachSegmentFromNode(RailNodeId nodeId, RailSegmentId segmentId);
	void refreshNodePreferredTangent(RailNodeId nodeId);
	void rebuildSegmentSpline(RailSegment& segment);
	vec3 preferredDirectionForNewSegment(RailNodeId nodeId, const vec3& chordDirection, bool isStartNode) const;
	vec3 makeTangentVector(const vec3& direction, float length) const;

	std::unordered_map<RailNodeId, std::unique_ptr<RailNode>> m_nodes;
	std::unordered_map<RailSegmentId, RailSegment> m_segments;
	RailNodeId m_nextNodeId = 1;
	RailSegmentId m_nextSegmentId = 1;
};

} // namespace tzw
