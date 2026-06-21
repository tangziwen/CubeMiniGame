#include "RailNetwork.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace tzw {

namespace
{
bool containsSegment(const std::vector<RailSegmentId>& segments, RailSegmentId segmentId)
{
	return std::find(segments.begin(), segments.end(), segmentId) != segments.end();
}
}

RailNode::~RailNode()
{
}

bool RailNode::canAutoRemoveWhenIsolated() const
{
	return true;
}

vec3 RailSegment::positionByDistance(float distance) const
{
	return spline.positionByDistance(distance);
}

vec3 RailSegment::tangentByDistance(float distance) const
{
	return spline.tangentByDistance(distance);
}

RailNetwork::RailNetwork()
{
}

RailNodeId RailNetwork::createNode(const vec3& position, const vec3& preferredTangent, bool isConnectable)
{
	auto newNode = std::make_unique<RailNode>();
	const RailNodeId nodeId = m_nextNodeId++;
	newNode->id = nodeId;
	newNode->position = position;
	newNode->preferredTangent = safeNormalized(preferredTangent, vec3(1.0f, 0.0f, 0.0f));
	newNode->isConnectable = isConnectable;
	m_nodes.emplace(nodeId, std::move(newNode));
	return nodeId;
}

RailSegmentId RailNetwork::createSegment(RailNodeId startNode, RailNodeId endNode)
{
	const RailNode* start = node(startNode);
	const RailNode* end = node(endNode);
	if (!start || !end || startNode == endNode)
	{
		return InvalidRailSegmentId;
	}

	const vec3 chord = end->position - start->position;
	const float length = chord.length();
	if (length <= 0.0001f)
	{
		return InvalidRailSegmentId;
	}

	const vec3 chordDirection = chord / length;
	const vec3 startDirection = preferredDirectionForNewSegment(startNode, chordDirection, true);
	const vec3 endDirection = preferredDirectionForNewSegment(endNode, chordDirection, false);
	return createSegmentWithTangents(startNode, endNode,
		makeTangentVector(startDirection, length),
		makeTangentVector(endDirection, length));
}

bool RailNetwork::deleteSegment(RailSegmentId segmentId)
{
	return deleteSegmentInternal(segmentId, true);
}

bool RailNetwork::splitSegment(RailSegmentId segmentId, float distanceOnSegment, RailSplitResult& outResult)
{
	outResult = RailSplitResult();

	const RailSegment* original = segment(segmentId);
	if (!original || original->cachedLength <= 0.0001f)
	{
		return false;
	}

	distanceOnSegment = std::max(0.0f, std::min(original->cachedLength, distanceOnSegment));
	if (distanceOnSegment <= 0.05f || distanceOnSegment >= original->cachedLength - 0.05f)
	{
		return false;
	}

	const RailNode* start = node(original->startNode);
	const RailNode* end = node(original->endNode);
	if (!start || !end)
	{
		return false;
	}

	const vec3 splitPosition = original->positionByDistance(distanceOnSegment);
	const vec3 splitDirection = original->tangentByDistance(distanceOnSegment);
	const RailNodeId splitNodeId = createNode(splitPosition, splitDirection, true);

	const vec3 startDirection = safeNormalized(original->startTangent, splitDirection);
	const vec3 endDirection = safeNormalized(original->endTangent, splitDirection);
	const float firstLength = std::max(distanceOnSegment, 0.05f);
	const float secondLength = std::max(original->cachedLength - distanceOnSegment, 0.05f);

	const RailNodeId startNodeId = original->startNode;
	const RailNodeId endNodeId = original->endNode;

	deleteSegmentInternal(segmentId, false);
	const RailSegmentId firstSegmentId = createSegmentWithTangents(startNodeId, splitNodeId,
		makeTangentVector(startDirection, firstLength),
		makeTangentVector(splitDirection, firstLength));
	const RailSegmentId secondSegmentId = createSegmentWithTangents(splitNodeId, endNodeId,
		makeTangentVector(splitDirection, secondLength),
		makeTangentVector(endDirection, secondLength));

	if (firstSegmentId == InvalidRailSegmentId || secondSegmentId == InvalidRailSegmentId)
	{
		deleteSegmentInternal(firstSegmentId, false);
		deleteSegmentInternal(secondSegmentId, false);
		deleteNodeIfIsolated(splitNodeId);
		return false;
	}

	outResult.nodeId = splitNodeId;
	outResult.firstSegmentId = firstSegmentId;
	outResult.secondSegmentId = secondSegmentId;
	return true;
}

RailNode* RailNetwork::node(RailNodeId nodeId)
{
	auto iter = m_nodes.find(nodeId);
	return iter == m_nodes.end() ? nullptr : iter->second.get();
}

const RailNode* RailNetwork::node(RailNodeId nodeId) const
{
	auto iter = m_nodes.find(nodeId);
	return iter == m_nodes.end() ? nullptr : iter->second.get();
}

RailSegment* RailNetwork::segment(RailSegmentId segmentId)
{
	auto iter = m_segments.find(segmentId);
	return iter == m_segments.end() ? nullptr : &iter->second;
}

const RailSegment* RailNetwork::segment(RailSegmentId segmentId) const
{
	auto iter = m_segments.find(segmentId);
	return iter == m_segments.end() ? nullptr : &iter->second;
}

const std::unordered_map<RailNodeId, std::unique_ptr<RailNode>>& RailNetwork::nodes() const
{
	return m_nodes;
}

const std::unordered_map<RailSegmentId, RailSegment>& RailNetwork::segments() const
{
	return m_segments;
}

RailNodePick RailNetwork::findNearestNode(const vec3& point, float maxDistance) const
{
	RailNodePick result;
	result.distance = maxDistance;
	for (const auto& pair : m_nodes)
	{
		const RailNode* candidate = pair.second.get();
		if (!candidate || !candidate->isConnectable)
		{
			continue;
		}

		const float distance = candidate->position.distance(point);
		if (distance <= result.distance)
		{
			result.nodeId = candidate->id;
			result.distance = distance;
		}
	}
	return result;
}

RailSegmentPick RailNetwork::findNearestSegment(const vec3& point, float maxDistance) const
{
	RailSegmentPick result;
	result.distance = maxDistance;
	for (const auto& pair : m_segments)
	{
		const RailSegment& candidate = pair.second;
		RailSplineNearestPoint nearest = candidate.spline.nearestPoint(point);
		if (nearest.distanceToPoint <= result.distance)
		{
			result.segmentId = candidate.id;
			result.distanceOnSegment = nearest.distanceOnSpline;
			result.distance = nearest.distanceToPoint;
			result.position = nearest.position;
		}
	}
	return result;
}

void RailNetwork::deleteIsolatedOrdinaryNodes()
{
	std::vector<RailNodeId> nodesToDelete;
	for (const auto& pair : m_nodes)
	{
		const RailNode* candidate = pair.second.get();
		if (candidate && candidate->segments.empty() && candidate->canAutoRemoveWhenIsolated())
		{
			nodesToDelete.push_back(candidate->id);
		}
	}
	for (RailNodeId nodeId : nodesToDelete)
	{
		m_nodes.erase(nodeId);
	}
}

void RailNetwork::clear()
{
	m_segments.clear();
	m_nodes.clear();
	m_nextNodeId = 1;
	m_nextSegmentId = 1;
}

RailSegmentId RailNetwork::createSegmentWithTangents(RailNodeId startNode, RailNodeId endNode,
	const vec3& startTangent, const vec3& endTangent)
{
	if (!node(startNode) || !node(endNode) || startNode == endNode)
	{
		return InvalidRailSegmentId;
	}

	const RailSegmentId segmentId = m_nextSegmentId++;
	RailSegment segment;
	segment.id = segmentId;
	segment.startNode = startNode;
	segment.endNode = endNode;
	segment.startTangent = startTangent;
	segment.endTangent = endTangent;
	rebuildSegmentSpline(segment);
	m_segments.emplace(segmentId, segment);

	attachSegmentToNode(startNode, segmentId);
	attachSegmentToNode(endNode, segmentId);
	refreshNodePreferredTangent(startNode);
	refreshNodePreferredTangent(endNode);
	return segmentId;
}

bool RailNetwork::deleteSegmentInternal(RailSegmentId segmentId, bool cleanupIsolatedNodes)
{
	auto iter = m_segments.find(segmentId);
	if (iter == m_segments.end())
	{
		return false;
	}

	const RailNodeId startNode = iter->second.startNode;
	const RailNodeId endNode = iter->second.endNode;
	detachSegmentFromNode(startNode, segmentId);
	detachSegmentFromNode(endNode, segmentId);
	m_segments.erase(iter);

	refreshNodePreferredTangent(startNode);
	refreshNodePreferredTangent(endNode);
	if (cleanupIsolatedNodes)
	{
		deleteNodeIfIsolated(startNode);
		deleteNodeIfIsolated(endNode);
	}
	return true;
}

bool RailNetwork::deleteNodeIfIsolated(RailNodeId nodeId)
{
	RailNode* candidate = node(nodeId);
	if (!candidate || !candidate->segments.empty() || !candidate->canAutoRemoveWhenIsolated())
	{
		return false;
	}
	m_nodes.erase(nodeId);
	return true;
}

void RailNetwork::attachSegmentToNode(RailNodeId nodeId, RailSegmentId segmentId)
{
	RailNode* target = node(nodeId);
	if (target && !containsSegment(target->segments, segmentId))
	{
		target->segments.push_back(segmentId);
	}
}

void RailNetwork::detachSegmentFromNode(RailNodeId nodeId, RailSegmentId segmentId)
{
	RailNode* target = node(nodeId);
	if (!target)
	{
		return;
	}

	auto iter = std::remove(target->segments.begin(), target->segments.end(), segmentId);
	target->segments.erase(iter, target->segments.end());
}

void RailNetwork::refreshNodePreferredTangent(RailNodeId nodeId)
{
	RailNode* target = node(nodeId);
	if (!target || target->segments.empty())
	{
		return;
	}

	if (target->segments.size() == 1)
	{
		const RailSegment* onlySegment = segment(target->segments.front());
		if (!onlySegment)
		{
			return;
		}
		if (onlySegment->startNode == nodeId)
		{
			target->preferredTangent = -onlySegment->tangentByDistance(0.0f);
		}
		else
		{
			target->preferredTangent = onlySegment->tangentByDistance(onlySegment->cachedLength);
		}
		return;
	}

	vec3 sum;
	for (RailSegmentId segmentId : target->segments)
	{
		const RailSegment* linked = segment(segmentId);
		if (!linked)
		{
			continue;
		}
		if (linked->startNode == nodeId)
		{
			sum += -linked->tangentByDistance(0.0f);
		}
		else
		{
			sum += linked->tangentByDistance(linked->cachedLength);
		}
	}
	target->preferredTangent = safeNormalized(sum, target->preferredTangent);
}

void RailNetwork::rebuildSegmentSpline(RailSegment& segment)
{
	const RailNode* start = node(segment.startNode);
	const RailNode* end = node(segment.endNode);
	if (!start || !end)
	{
		segment.cachedLength = 0.0f;
		return;
	}

	segment.spline.configure(start->position, end->position, segment.startTangent, segment.endTangent);
	segment.cachedLength = segment.spline.length();
}

vec3 RailNetwork::preferredDirectionForNewSegment(RailNodeId nodeId, const vec3& chordDirection, bool) const
{
	const RailNode* target = node(nodeId);
	if (!target || target->segments.empty())
	{
		return chordDirection;
	}

	const vec3 desired = chordDirection;
	const vec3 preferred = safeNormalized(target->preferredTangent, desired);
	if (vec3::DotProduct(preferred, desired) > 0.15f)
	{
		return preferred;
	}
	return desired;
}

vec3 RailNetwork::makeTangentVector(const vec3& direction, float length) const
{
	return safeNormalized(direction, vec3(1.0f, 0.0f, 0.0f)) * std::max(length * 0.7f, 0.05f);
}

} // namespace tzw
