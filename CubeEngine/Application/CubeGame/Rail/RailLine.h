#pragma once

#include "RailNetwork.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace tzw {

using RailLineId = int;

constexpr RailLineId InvalidRailLineId = -1;

struct RailLinePathStep
{
	RailSegmentId segmentId = InvalidRailSegmentId;
	RailNodeId fromNode = InvalidRailNodeId;
	RailNodeId toNode = InvalidRailNodeId;
	bool reversed = false;
	float length = 0.0f;
	float startDistance = 0.0f;
};

struct RailLineSample
{
	bool valid = false;
	vec3 position;
	vec3 tangent = vec3(1.0f, 0.0f, 0.0f);
};

class RailLine
{
public:
	RailLineId id = InvalidRailLineId;
	std::string name;
	std::vector<RailNodeId> controlNodes;
	std::vector<RailLinePathStep> pathSteps;
	bool isLoop = false;
	bool isUsable = false;
	std::string invalidReason;
	float totalLength = 0.0f;
};

class RailLinePathfinder
{
public:
	bool rebuildLine(const RailNetwork& network, RailLine& line) const;

private:
	struct AdjacentSegment
	{
		RailNodeId nodeId = InvalidRailNodeId;
		RailSegmentId segmentId = InvalidRailSegmentId;
		float length = 0.0f;
		bool reversed = false;
	};

	bool buildPathBetween(const RailNetwork& network, RailNodeId startNode, RailNodeId endNode,
		std::vector<RailLinePathStep>& outSteps, float& pathStartDistance) const;
	std::unordered_map<RailNodeId, std::vector<AdjacentSegment>> buildAdjacency(const RailNetwork& network) const;
};

class RailLineManager
{
public:
	RailLineId createLine();
	bool deleteLine(RailLineId lineId);
	bool addControlNode(RailLineId lineId, RailNodeId nodeId, const RailNetwork& network);
	bool removeControlNodeAt(RailLineId lineId, int index, const RailNetwork& network);
	void rebuildAll(const RailNetwork& network);
	bool rebuildLine(RailLineId lineId, const RailNetwork& network);
	void clear();

	RailLine* line(RailLineId lineId);
	const RailLine* line(RailLineId lineId) const;
	const std::vector<RailLine>& lines() const;

	void setSelectedLineId(RailLineId lineId);
	RailLineId selectedLineId() const;
	void setAddModeLineId(RailLineId lineId);
	RailLineId addModeLineId() const;

	RailLineSample sampleLine(const RailNetwork& network, const RailLine& line, float distance) const;

private:
	std::vector<RailLine> m_lines;
	RailLinePathfinder m_pathfinder;
	RailLineId m_selectedLineId = InvalidRailLineId;
	RailLineId m_addModeLineId = InvalidRailLineId;
	RailLineId m_nextLineId = 1;
};

} // namespace tzw
