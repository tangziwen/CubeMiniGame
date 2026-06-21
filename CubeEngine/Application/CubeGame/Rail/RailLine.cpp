#include "RailLine.h"

#include "Utility/log/Log.h"

#include <algorithm>
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
}

bool RailLinePathfinder::rebuildLine(const RailNetwork& network, RailLine& line) const
{
	line.pathSteps.clear();
	line.totalLength = 0.0f;
	line.isLoop = false;
	line.isUsable = false;
	line.invalidReason.clear();

	if (line.controlNodes.size() < 2)
	{
		line.invalidReason = "Need at least 2 nodes";
		return false;
	}

	for (RailNodeId nodeId : line.controlNodes)
	{
		if (!network.node(nodeId))
		{
			line.invalidReason = "Missing rail node";
			return false;
		}
	}

	line.isLoop = line.controlNodes.front() == line.controlNodes.back();
	for (size_t i = 1; i < line.controlNodes.size(); ++i)
	{
		if (!buildPathBetween(network, line.controlNodes[i - 1], line.controlNodes[i],
			line.pathSteps, line.totalLength))
		{
			line.invalidReason = "Control nodes are not reachable";
			line.pathSteps.clear();
			line.totalLength = 0.0f;
			return false;
		}
	}

	if (line.pathSteps.empty() || line.totalLength <= 0.0001f)
	{
		line.invalidReason = "Empty rail path";
		return false;
	}

	line.isUsable = true;
	return true;
}

bool RailLinePathfinder::buildPathBetween(const RailNetwork& network, RailNodeId startNode, RailNodeId endNode,
	std::vector<RailLinePathStep>& outSteps, float& pathStartDistance) const
{
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
		step.startDistance = pathStartDistance;
		pathStartDistance += step.length;
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

RailLineId RailLineManager::createLine()
{
	RailLine line;
	line.id = m_nextLineId++;
	line.name = "Line " + std::to_string(line.id);
	line.invalidReason = "Need at least 2 nodes";
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
	if (m_addModeLineId == lineId)
	{
		m_addModeLineId = InvalidRailLineId;
	}
	return true;
}

bool RailLineManager::addControlNode(RailLineId lineId, RailNodeId nodeId, const RailNetwork& network)
{
	RailLine* targetLine = line(lineId);
	if (!targetLine || !network.node(nodeId))
	{
		return false;
	}

	if (!targetLine->controlNodes.empty())
	{
		RailLine testLine;
		testLine.controlNodes.push_back(targetLine->controlNodes.back());
		testLine.controlNodes.push_back(nodeId);
		if (!m_pathfinder.rebuildLine(network, testLine))
		{
			tlogError("RailLine add node rejected: node %d is not reachable from node %d",
				nodeId, targetLine->controlNodes.back());
			return false;
		}
	}

	targetLine->controlNodes.push_back(nodeId);
	rebuildLine(lineId, network);
	return true;
}

bool RailLineManager::removeControlNodeAt(RailLineId lineId, int index, const RailNetwork& network)
{
	RailLine* targetLine = line(lineId);
	if (!targetLine || index < 0 || index >= static_cast<int>(targetLine->controlNodes.size()))
	{
		return false;
	}

	targetLine->controlNodes.erase(targetLine->controlNodes.begin() + index);
	rebuildLine(lineId, network);
	return true;
}

void RailLineManager::rebuildAll(const RailNetwork& network)
{
	for (RailLine& line : m_lines)
	{
		m_pathfinder.rebuildLine(network, line);
	}
}

bool RailLineManager::rebuildLine(RailLineId lineId, const RailNetwork& network)
{
	RailLine* targetLine = line(lineId);
	if (!targetLine)
	{
		return false;
	}
	return m_pathfinder.rebuildLine(network, *targetLine);
}

void RailLineManager::clear()
{
	m_lines.clear();
	m_selectedLineId = InvalidRailLineId;
	m_addModeLineId = InvalidRailLineId;
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

void RailLineManager::setAddModeLineId(RailLineId lineId)
{
	m_addModeLineId = lineId;
}

RailLineId RailLineManager::addModeLineId() const
{
	return m_addModeLineId;
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

	float distanceOnSegment = distance - selectedStep->startDistance;
	if (selectedStep->reversed)
	{
		distanceOnSegment = selectedStep->length - distanceOnSegment;
	}
	distanceOnSegment = clampDistance(distanceOnSegment, selectedStep->length);

	sample.position = segment->positionByDistance(distanceOnSegment);
	sample.tangent = segment->tangentByDistance(distanceOnSegment);
	if (selectedStep->reversed)
	{
		sample.tangent = -sample.tangent;
	}
	sample.valid = true;
	return sample;
}

} // namespace tzw
