#include "RailLine.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <queue>
#include <unordered_set>

namespace tzw {

namespace
{
struct QueueNode
{
	RailNodeId nodeId = InvalidRailNodeId;
	float distance = 0.0f;
};

bool operator>(const QueueNode& left, const QueueNode& right)
{
	return left.distance > right.distance;
}

float clampDistance(float value, float maxValue)
{
	return std::max(0.0f, std::min(value, maxValue));
}

RailLinePathStep makeStep(const RailSegment& segment, float fromDistance, float toDistance)
{
	RailLinePathStep step;
	step.segmentId = segment.id;
	step.fromNode = InvalidRailNodeId;
	step.toNode = InvalidRailNodeId;
	step.reversed = toDistance < fromDistance;
	step.length = std::fabs(toDistance - fromDistance);
	step.segmentStartDistance = clampDistance(fromDistance, segment.cachedLength);
	step.segmentEndDistance = clampDistance(toDistance, segment.cachedLength);
	return step;
}

float totalStepLength(const std::vector<RailLinePathStep>& steps)
{
	float result = 0.0f;
	for (const RailLinePathStep& step : steps)
	{
		result += step.length;
	}
	return result;
}

void appendStepsWithLineDistance(const std::vector<RailLinePathStep>& source, std::vector<RailLinePathStep>& target,
	float& pathStartDistance)
{
	for (RailLinePathStep step : source)
	{
		if (step.length <= 0.0001f)
		{
			continue;
		}
		step.startDistance = pathStartDistance;
		pathStartDistance += step.length;
		target.push_back(step);
	}
}
}

bool RailLinePathfinder::rebuildLine(const RailNetwork& network, const RailAnchorManager& anchorManager,
	const RailStationManager& stationManager, const RailRoutePointManager& routePointManager, RailLine& line) const
{
	line.pathSteps.clear();
	line.totalLength = 0.0f;
	line.isLoop = false;
	line.isUsable = false;
	line.invalidReason.clear();
	for (RailLineControlPoint& controlPoint : line.controlPoints)
	{
		controlPoint.isResolved = false;
		controlPoint.distanceOnLine = 0.0f;
	}

	if (line.controlPoints.size() < 2)
	{
		line.invalidReason = "Need at least 2 different stations";
		return false;
	}

	std::unordered_set<RailStationId> stationIds;
	for (const RailLineControlPoint& controlPoint : line.controlPoints)
	{
		if (controlPoint.kind == RailLineControlPointKind::Station
			&& controlPoint.stationId != InvalidRailStationId)
		{
			stationIds.insert(controlPoint.stationId);
		}
	}
	if (stationIds.size() < 2)
	{
		line.invalidReason = "Need at least 2 different stations";
		return false;
	}

	std::vector<RailAnchorId> anchorIds;
	anchorIds.reserve(line.controlPoints.size());
	for (RailLineControlPoint& controlPoint : line.controlPoints)
	{
		RailAnchorId anchorId = InvalidRailAnchorId;
		std::string invalidReason;
		if (!resolveControlPoint(anchorManager, stationManager, routePointManager, controlPoint,
			anchorId, invalidReason))
		{
			line.invalidReason = invalidReason;
			return false;
		}
		if (!anchorManager.isAnchorValid(network, anchorId))
		{
			line.invalidReason = "Control point anchor is invalid";
			return false;
		}
		controlPoint.isResolved = true;
		anchorIds.push_back(anchorId);
	}

	line.isLoop = anchorIds.front() == anchorIds.back();
	line.controlPoints.front().distanceOnLine = 0.0f;
	for (size_t i = 1; i < anchorIds.size(); ++i)
	{
		if (!buildPathBetween(network, anchorManager, anchorIds[i - 1], anchorIds[i],
			line.pathSteps, line.totalLength))
		{
			line.invalidReason = "Control points are not reachable";
			line.pathSteps.clear();
			line.totalLength = 0.0f;
			return false;
		}
		line.controlPoints[i].distanceOnLine = line.totalLength;
	}

	if (line.pathSteps.empty() || line.totalLength <= 0.0001f)
	{
		line.invalidReason = "Empty rail path";
		return false;
	}

	line.isUsable = true;
	return true;
}

bool RailLinePathfinder::buildPathBetween(const RailNetwork& network, const RailAnchorManager& anchorManager,
	RailAnchorId startAnchorId, RailAnchorId endAnchorId, std::vector<RailLinePathStep>& outSteps,
	float& pathStartDistance) const
{
	const RailAnchor* startAnchor = anchorManager.anchor(startAnchorId);
	const RailAnchor* endAnchor = anchorManager.anchor(endAnchorId);
	if (!startAnchor || !endAnchor)
	{
		return false;
	}

	const RailSegment* startSegment = network.segment(startAnchor->location.segmentId);
	const RailSegment* endSegment = network.segment(endAnchor->location.segmentId);
	if (!startSegment || !endSegment || startSegment->cachedLength <= 0.0001f || endSegment->cachedLength <= 0.0001f)
	{
		return false;
	}

	const float startDistance = clampDistance(startAnchor->location.distanceOnSegment, startSegment->cachedLength);
	const float endDistance = clampDistance(endAnchor->location.distanceOnSegment, endSegment->cachedLength);
	if (startSegment->id == endSegment->id)
	{
		std::vector<RailLinePathStep> localSteps;
		localSteps.push_back(makeStep(*startSegment, startDistance, endDistance));
		appendStepsWithLineDistance(localSteps, outSteps, pathStartDistance);
		return true;
	}

	struct NodeSide
	{
		RailNodeId nodeId = InvalidRailNodeId;
		std::vector<RailLinePathStep> steps;
	};

	std::vector<NodeSide> startSides;
	NodeSide startAtStartNode;
	startAtStartNode.nodeId = startSegment->startNode;
	startAtStartNode.steps.push_back(makeStep(*startSegment, startDistance, 0.0f));
	startSides.push_back(startAtStartNode);

	NodeSide startAtEndNode;
	startAtEndNode.nodeId = startSegment->endNode;
	startAtEndNode.steps.push_back(makeStep(*startSegment, startDistance, startSegment->cachedLength));
	startSides.push_back(startAtEndNode);

	std::vector<NodeSide> endSides;
	NodeSide endAtStartNode;
	endAtStartNode.nodeId = endSegment->startNode;
	endAtStartNode.steps.push_back(makeStep(*endSegment, 0.0f, endDistance));
	endSides.push_back(endAtStartNode);

	NodeSide endAtEndNode;
	endAtEndNode.nodeId = endSegment->endNode;
	endAtEndNode.steps.push_back(makeStep(*endSegment, endSegment->cachedLength, endDistance));
	endSides.push_back(endAtEndNode);

	float bestLength = std::numeric_limits<float>::max();
	std::vector<RailLinePathStep> bestSteps;
	for (const NodeSide& startSide : startSides)
	{
		for (const NodeSide& endSide : endSides)
		{
			std::vector<RailLinePathStep> nodePath;
			if (!buildNodePath(network, startSide.nodeId, endSide.nodeId, nodePath))
			{
				continue;
			}

			std::vector<RailLinePathStep> candidate;
			candidate.insert(candidate.end(), startSide.steps.begin(), startSide.steps.end());
			candidate.insert(candidate.end(), nodePath.begin(), nodePath.end());
			candidate.insert(candidate.end(), endSide.steps.begin(), endSide.steps.end());
			const float candidateLength = totalStepLength(candidate);
			if (candidateLength < bestLength)
			{
				bestLength = candidateLength;
				bestSteps = candidate;
			}
		}
	}

	if (bestSteps.empty() && bestLength == std::numeric_limits<float>::max())
	{
		return false;
	}

	appendStepsWithLineDistance(bestSteps, outSteps, pathStartDistance);
	return true;
}

bool RailLinePathfinder::buildNodePath(const RailNetwork& network, RailNodeId startNode, RailNodeId endNode,
	std::vector<RailLinePathStep>& outSteps) const
{
	outSteps.clear();
	if (startNode == endNode)
	{
		return true;
	}

	const auto adjacency = buildAdjacency(network);
	std::priority_queue<QueueNode, std::vector<QueueNode>, std::greater<QueueNode>> openSet;
	std::unordered_map<RailNodeId, float> distanceByNode;
	std::unordered_map<RailNodeId, RailNodeId> previousNode;
	std::unordered_map<RailNodeId, RailSegmentId> previousSegment;
	std::unordered_set<RailNodeId> visited;

	distanceByNode[startNode] = 0.0f;
	openSet.push({ startNode, 0.0f });

	while (!openSet.empty())
	{
		const QueueNode current = openSet.top();
		openSet.pop();
		if (visited.find(current.nodeId) != visited.end())
		{
			continue;
		}
		visited.insert(current.nodeId);

		if (current.nodeId == endNode)
		{
			break;
		}

		auto adjIter = adjacency.find(current.nodeId);
		if (adjIter == adjacency.end())
		{
			continue;
		}

		for (const AdjacentSegment& adjacent : adjIter->second)
		{
			const float newDistance = current.distance + adjacent.length;
			auto distanceIter = distanceByNode.find(adjacent.nodeId);
			if (distanceIter == distanceByNode.end() || newDistance < distanceIter->second)
			{
				distanceByNode[adjacent.nodeId] = newDistance;
				previousNode[adjacent.nodeId] = current.nodeId;
				previousSegment[adjacent.nodeId] = adjacent.segmentId;
				openSet.push({ adjacent.nodeId, newDistance });
			}
		}
	}

	if (distanceByNode.find(endNode) == distanceByNode.end())
	{
		return false;
	}

	std::vector<RailNodeId> nodePath;
	for (RailNodeId nodeId = endNode; nodeId != startNode; nodeId = previousNode[nodeId])
	{
		nodePath.push_back(nodeId);
	}
	nodePath.push_back(startNode);
	std::reverse(nodePath.begin(), nodePath.end());

	for (size_t i = 1; i < nodePath.size(); ++i)
	{
		const RailNodeId fromNode = nodePath[i - 1];
		const RailNodeId toNode = nodePath[i];
		const RailSegmentId segmentId = previousSegment[toNode];
		const RailSegment* segment = network.segment(segmentId);
		if (!segment)
		{
			return false;
		}

		RailLinePathStep step;
		step.segmentId = segmentId;
		step.fromNode = fromNode;
		step.toNode = toNode;
		step.reversed = segment->endNode == fromNode && segment->startNode == toNode;
		step.length = segment->cachedLength;
		step.segmentStartDistance = step.reversed ? segment->cachedLength : 0.0f;
		step.segmentEndDistance = step.reversed ? 0.0f : segment->cachedLength;
		outSteps.push_back(step);
	}

	return true;
}

std::unordered_map<RailNodeId, std::vector<RailLinePathfinder::AdjacentSegment>> RailLinePathfinder::buildAdjacency(
	const RailNetwork& network) const
{
	std::unordered_map<RailNodeId, std::vector<AdjacentSegment>> adjacency;
	for (const auto& pair : network.segments())
	{
		const RailSegment& segment = pair.second;
		if (segment.cachedLength <= 0.0001f)
		{
			continue;
		}

		adjacency[segment.startNode].push_back({ segment.endNode, segment.id, segment.cachedLength, false });
		adjacency[segment.endNode].push_back({ segment.startNode, segment.id, segment.cachedLength, true });
	}
	return adjacency;
}

bool RailLinePathfinder::resolveControlPoint(const RailAnchorManager& /*anchorManager*/,
	const RailStationManager& stationManager, const RailRoutePointManager& routePointManager,
	const RailLineControlPoint& controlPoint, RailAnchorId& outAnchorId, std::string& outInvalidReason) const
{
	outAnchorId = InvalidRailAnchorId;
	if (controlPoint.kind == RailLineControlPointKind::Station)
	{
		outAnchorId = stationManager.anchorForStation(controlPoint.stationId);
		if (outAnchorId == InvalidRailAnchorId)
		{
			outInvalidReason = "Missing station platform";
			return false;
		}
		return true;
	}

	outAnchorId = routePointManager.anchorForRoutePoint(controlPoint.routePointId);
	if (outAnchorId == InvalidRailAnchorId)
	{
		outInvalidReason = "Missing route point";
		return false;
	}
	return true;
}

RailLineId RailLineManager::createLine()
{
	RailLine line;
	line.id = m_nextLineId++;
	line.name = "Line " + std::to_string(line.id);
	line.invalidReason = "Need at least 2 different stations";
	m_lines.push_back(line);
	m_selectedLineId = line.id;
	return line.id;
}

bool RailLineManager::deleteLine(RailLineId lineId)
{
	auto iter = std::remove_if(m_lines.begin(), m_lines.end(), [lineId](const RailLine& line)
	{
		return line.id == lineId;
	});
	if (iter == m_lines.end())
	{
		return false;
	}
	m_lines.erase(iter, m_lines.end());
	if (m_selectedLineId == lineId)
	{
		m_selectedLineId = m_lines.empty() ? InvalidRailLineId : m_lines.front().id;
	}
	return true;
}

bool RailLineManager::addControlPoint(RailLineId lineId, const RailLineControlPoint& controlPoint,
	const RailNetwork& network, const RailAnchorManager& anchorManager, const RailStationManager& stationManager,
	const RailRoutePointManager& routePointManager)
{
	RailLine* targetLine = line(lineId);
	if (!targetLine)
	{
		return false;
	}

	targetLine->controlPoints.push_back(controlPoint);
	rebuildLine(lineId, network, anchorManager, stationManager, routePointManager);
	return true;
}

bool RailLineManager::removeControlPointAt(RailLineId lineId, int index, const RailNetwork& network,
	const RailAnchorManager& anchorManager, const RailStationManager& stationManager,
	const RailRoutePointManager& routePointManager)
{
	RailLine* targetLine = line(lineId);
	if (!targetLine || index < 0 || index >= static_cast<int>(targetLine->controlPoints.size()))
	{
		return false;
	}

	targetLine->controlPoints.erase(targetLine->controlPoints.begin() + index);
	rebuildLine(lineId, network, anchorManager, stationManager, routePointManager);
	return true;
}

void RailLineManager::rebuildAll(const RailNetwork& network, const RailAnchorManager& anchorManager,
	const RailStationManager& stationManager, const RailRoutePointManager& routePointManager)
{
	for (RailLine& line : m_lines)
	{
		m_pathfinder.rebuildLine(network, anchorManager, stationManager, routePointManager, line);
	}
}

bool RailLineManager::rebuildLine(RailLineId lineId, const RailNetwork& network, const RailAnchorManager& anchorManager,
	const RailStationManager& stationManager, const RailRoutePointManager& routePointManager)
{
	RailLine* targetLine = line(lineId);
	if (!targetLine)
	{
		return false;
	}
	return m_pathfinder.rebuildLine(network, anchorManager, stationManager, routePointManager, *targetLine);
}

void RailLineManager::clear()
{
	m_lines.clear();
	m_selectedLineId = InvalidRailLineId;
	m_nextLineId = 1;
}

RailLine* RailLineManager::line(RailLineId lineId)
{
	for (RailLine& candidate : m_lines)
	{
		if (candidate.id == lineId)
		{
			return &candidate;
		}
	}
	return nullptr;
}

const RailLine* RailLineManager::line(RailLineId lineId) const
{
	for (const RailLine& candidate : m_lines)
	{
		if (candidate.id == lineId)
		{
			return &candidate;
		}
	}
	return nullptr;
}

const std::vector<RailLine>& RailLineManager::lines() const
{
	return m_lines;
}

void RailLineManager::setSelectedLineId(RailLineId lineId)
{
	m_selectedLineId = lineId;
}

RailLineId RailLineManager::selectedLineId() const
{
	return m_selectedLineId;
}

RailLineSample RailLineManager::sampleLine(const RailNetwork& network, const RailLine& line, float distance) const
{
	RailLineSample sample;
	if (!line.isUsable || line.pathSteps.empty())
	{
		return sample;
	}

	distance = clampDistance(distance, line.totalLength);
	const RailLinePathStep* selectedStep = &line.pathSteps.back();
	for (const RailLinePathStep& step : line.pathSteps)
	{
		if (distance <= step.startDistance + step.length)
		{
			selectedStep = &step;
			break;
		}
	}

	const RailSegment* segment = network.segment(selectedStep->segmentId);
	if (!segment || selectedStep->length <= 0.0001f)
	{
		return sample;
	}

	const float localDistance = clampDistance(distance - selectedStep->startDistance, selectedStep->length);
	const float ratio = selectedStep->length <= 0.0001f ? 0.0f : localDistance / selectedStep->length;
	const float distanceOnSegment = selectedStep->segmentStartDistance
		+ (selectedStep->segmentEndDistance - selectedStep->segmentStartDistance) * ratio;

	sample.position = segment->positionByDistance(clampDistance(distanceOnSegment, segment->cachedLength));
	sample.tangent = segment->tangentByDistance(clampDistance(distanceOnSegment, segment->cachedLength));
	if (selectedStep->reversed)
	{
		sample.tangent = -sample.tangent;
	}
	sample.valid = true;
	return sample;
}

} // namespace tzw
