#pragma once

#include "RailPoint.h"

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
	float segmentStartDistance = 0.0f;
	float segmentEndDistance = 0.0f;
};

struct RailLineSample
{
	bool valid = false;
	vec3 position;
	vec3 tangent = vec3(1.0f, 0.0f, 0.0f);
};

enum class RailLineControlPointKind
{
	Station,
	RoutePoint,
};

struct RailLineControlPoint
{
	RailLineControlPointKind kind = RailLineControlPointKind::Station;
	RailStationId stationId = InvalidRailStationId;
	RailRoutePointId routePointId = InvalidRailRoutePointId;
	bool isResolved = false;
	float distanceOnLine = 0.0f;
};

class RailLine
{
public:
	RailLineId id = InvalidRailLineId;
	std::string name;
	std::vector<RailLineControlPoint> controlPoints;
	std::vector<RailLinePathStep> pathSteps;
	bool isLoop = false;
	bool isUsable = false;
	std::string invalidReason;
	float totalLength = 0.0f;
};

class RailLinePathfinder
{
public:
	bool rebuildLine(const RailNetwork& network, const RailAnchorManager& anchorManager,
		const RailStationManager& stationManager, const RailRoutePointManager& routePointManager,
		RailLine& line) const;

private:
	struct AdjacentSegment
	{
		RailNodeId nodeId = InvalidRailNodeId;
		RailSegmentId segmentId = InvalidRailSegmentId;
		float length = 0.0f;
		bool reversed = false;
	};

	bool buildPathBetween(const RailNetwork& network, const RailAnchorManager& anchorManager,
		RailAnchorId startAnchorId, RailAnchorId endAnchorId,
		std::vector<RailLinePathStep>& outSteps, float& pathStartDistance) const;
	bool buildNodePath(const RailNetwork& network, RailNodeId startNode, RailNodeId endNode,
		std::vector<RailLinePathStep>& outSteps) const;
	std::unordered_map<RailNodeId, std::vector<AdjacentSegment>> buildAdjacency(const RailNetwork& network) const;
	bool resolveControlPoint(const RailAnchorManager& anchorManager, const RailStationManager& stationManager,
		const RailRoutePointManager& routePointManager, const RailLineControlPoint& controlPoint,
		RailAnchorId& outAnchorId, std::string& outInvalidReason) const;
};

class RailLineManager
{
public:
	RailLineId createLine();
	bool unserializeLine(const RailLine& line);
	bool deleteLine(RailLineId lineId);
	bool addControlPoint(RailLineId lineId, const RailLineControlPoint& controlPoint,
		const RailNetwork& network, const RailAnchorManager& anchorManager,
		const RailStationManager& stationManager, const RailRoutePointManager& routePointManager);
	bool removeControlPointAt(RailLineId lineId, int index, const RailNetwork& network,
		const RailAnchorManager& anchorManager, const RailStationManager& stationManager,
		const RailRoutePointManager& routePointManager);
	void rebuildAll(const RailNetwork& network, const RailAnchorManager& anchorManager,
		const RailStationManager& stationManager, const RailRoutePointManager& routePointManager);
	bool rebuildLine(RailLineId lineId, const RailNetwork& network, const RailAnchorManager& anchorManager,
		const RailStationManager& stationManager, const RailRoutePointManager& routePointManager);
	void clear();

	RailLine* line(RailLineId lineId);
	const RailLine* line(RailLineId lineId) const;
	const std::vector<RailLine>& lines() const;

	void setSelectedLineId(RailLineId lineId);
	RailLineId selectedLineId() const;

	RailLineSample sampleLine(const RailNetwork& network, const RailLine& line, float distance) const;

private:
	std::vector<RailLine> m_lines;
	RailLinePathfinder m_pathfinder;
	RailLineId m_selectedLineId = InvalidRailLineId;
	RailLineId m_nextLineId = 1;
};

} // namespace tzw
