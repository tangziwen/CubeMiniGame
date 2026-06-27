#pragma once

#include "RailNetwork.h"

#include <string>
#include <vector>

namespace tzw {

using RailAnchorId = int;
using RailStationId = int;
using RailPlatformId = int;
using RailRoutePointId = int;

constexpr RailAnchorId InvalidRailAnchorId = -1;
constexpr RailStationId InvalidRailStationId = -1;
constexpr RailPlatformId InvalidRailPlatformId = -1;
constexpr RailRoutePointId InvalidRailRoutePointId = -1;

struct RailTrackLocation
{
	RailSegmentId segmentId = InvalidRailSegmentId;
	float distanceOnSegment = 0.0f;
};

struct RailAnchor
{
	RailAnchorId id = InvalidRailAnchorId;
	RailTrackLocation location;
};

struct RailAnchorSample
{
	bool valid = false;
	vec3 position;
	vec3 tangent = vec3(1.0f, 0.0f, 0.0f);
};

class RailAnchorManager
{
public:
	RailAnchorId createAnchor(const RailTrackLocation& location);
	bool deleteAnchor(RailAnchorId anchorId);
	void clear();

	RailAnchor* anchor(RailAnchorId anchorId);
	const RailAnchor* anchor(RailAnchorId anchorId) const;
	const std::vector<RailAnchor>& anchors() const;

	void migrateAnchorsAfterSegmentSplit(RailSegmentId oldSegmentId, float splitDistance,
		RailSegmentId firstSegmentId, RailSegmentId secondSegmentId);
	bool isAnchorValid(const RailNetwork& network, RailAnchorId anchorId) const;
	RailAnchorSample sampleAnchor(const RailNetwork& network, RailAnchorId anchorId) const;
	bool pickLocationOnTrack(const RailNetwork& network, const vec3& point, float maxDistance,
		RailTrackLocation& outLocation, vec3* outPosition = nullptr) const;

private:
	std::vector<RailAnchor> m_anchors;
	RailAnchorId m_nextAnchorId = 1;
};

struct RailPlatform
{
	RailPlatformId id = InvalidRailPlatformId;
	RailStationId stationId = InvalidRailStationId;
	std::string name;
	RailAnchorId anchorId = InvalidRailAnchorId;
};

struct RailStation
{
	RailStationId id = InvalidRailStationId;
	std::string name;
	std::vector<RailPlatformId> platforms;
};

class RailStationManager
{
public:
	RailStationId createStation(RailAnchorId anchorId, const std::string& name = "");
	bool deleteStation(RailStationId stationId, RailAnchorManager& anchorManager);
	void clear(RailAnchorManager& anchorManager);
	void clearWithoutAnchors();

	RailStation* station(RailStationId stationId);
	const RailStation* station(RailStationId stationId) const;
	RailPlatform* platform(RailPlatformId platformId);
	const RailPlatform* platform(RailPlatformId platformId) const;
	const std::vector<RailStation>& stations() const;
	const std::vector<RailPlatform>& platforms() const;

	bool renameStation(RailStationId stationId, const std::string& name);
	const RailPlatform* firstPlatform(RailStationId stationId) const;
	RailAnchorId anchorForStation(RailStationId stationId) const;
	RailStationId findNearestStation(const RailNetwork& network, const RailAnchorManager& anchorManager,
		const vec3& point, float maxDistance) const;

private:
	std::vector<RailStation> m_stations;
	std::vector<RailPlatform> m_platforms;
	RailStationId m_nextStationId = 1;
	RailPlatformId m_nextPlatformId = 1;
};

struct RailRoutePoint
{
	RailRoutePointId id = InvalidRailRoutePointId;
	std::string name;
	RailAnchorId anchorId = InvalidRailAnchorId;
};

class RailRoutePointManager
{
public:
	RailRoutePointId createRoutePoint(RailAnchorId anchorId, const std::string& name = "");
	bool deleteRoutePoint(RailRoutePointId routePointId, RailAnchorManager& anchorManager);
	void clear(RailAnchorManager& anchorManager);
	void clearWithoutAnchors();

	RailRoutePoint* routePoint(RailRoutePointId routePointId);
	const RailRoutePoint* routePoint(RailRoutePointId routePointId) const;
	const std::vector<RailRoutePoint>& routePoints() const;

	bool renameRoutePoint(RailRoutePointId routePointId, const std::string& name);
	RailAnchorId anchorForRoutePoint(RailRoutePointId routePointId) const;
	RailRoutePointId findNearestRoutePoint(const RailNetwork& network, const RailAnchorManager& anchorManager,
		const vec3& point, float maxDistance) const;

private:
	std::vector<RailRoutePoint> m_routePoints;
	RailRoutePointId m_nextRoutePointId = 1;
};

} // namespace tzw
