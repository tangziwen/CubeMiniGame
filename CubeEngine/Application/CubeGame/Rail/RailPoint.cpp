#include "RailPoint.h"

#include <algorithm>

namespace tzw {

RailAnchorId RailAnchorManager::createAnchor(const RailTrackLocation& location)
{
	RailAnchor anchor;
	anchor.id = m_nextAnchorId++;
	anchor.location = location;
	m_anchors.push_back(anchor);
	return anchor.id;
}

RailAnchorId RailAnchorManager::unserializeAnchor(RailAnchorId anchorId, const RailTrackLocation& location)
{
	if (anchorId == InvalidRailAnchorId || anchor(anchorId))
	{
		return InvalidRailAnchorId;
	}
	RailAnchor anchorData;
	anchorData.id = anchorId;
	anchorData.location = location;
	m_anchors.push_back(anchorData);
	m_nextAnchorId = std::max(m_nextAnchorId, anchorId + 1);
	return anchorId;
}

bool RailAnchorManager::deleteAnchor(RailAnchorId anchorId)
{
	auto iter = std::remove_if(m_anchors.begin(), m_anchors.end(), [anchorId](const RailAnchor& anchor)
	{
		return anchor.id == anchorId;
	});
	if (iter == m_anchors.end())
	{
		return false;
	}
	m_anchors.erase(iter, m_anchors.end());
	return true;
}

void RailAnchorManager::clear()
{
	m_anchors.clear();
	m_nextAnchorId = 1;
}

RailAnchor* RailAnchorManager::anchor(RailAnchorId anchorId)
{
	for (RailAnchor& candidate : m_anchors)
	{
		if (candidate.id == anchorId)
		{
			return &candidate;
		}
	}
	return nullptr;
}

const RailAnchor* RailAnchorManager::anchor(RailAnchorId anchorId) const
{
	for (const RailAnchor& candidate : m_anchors)
	{
		if (candidate.id == anchorId)
		{
			return &candidate;
		}
	}
	return nullptr;
}

const std::vector<RailAnchor>& RailAnchorManager::anchors() const
{
	return m_anchors;
}

void RailAnchorManager::migrateAnchorsAfterSegmentSplit(RailSegmentId oldSegmentId, float splitDistance,
	RailSegmentId firstSegmentId, RailSegmentId secondSegmentId)
{
	for (RailAnchor& anchor : m_anchors)
	{
		if (anchor.location.segmentId != oldSegmentId)
		{
			continue;
		}
		if (anchor.location.distanceOnSegment <= splitDistance)
		{
			anchor.location.segmentId = firstSegmentId;
		}
		else
		{
			anchor.location.segmentId = secondSegmentId;
			anchor.location.distanceOnSegment -= splitDistance;
		}
	}
}

bool RailAnchorManager::isAnchorValid(const RailNetwork& network, RailAnchorId anchorId) const
{
	const RailAnchor* targetAnchor = anchor(anchorId);
	if (!targetAnchor)
	{
		return false;
	}
	const RailSegment* segment = network.segment(targetAnchor->location.segmentId);
	if (!segment || segment->cachedLength <= 0.0001f)
	{
		return false;
	}
	return targetAnchor->location.distanceOnSegment >= 0.0f
		&& targetAnchor->location.distanceOnSegment <= segment->cachedLength;
}

RailAnchorSample RailAnchorManager::sampleAnchor(const RailNetwork& network, RailAnchorId anchorId) const
{
	RailAnchorSample sample;
	const RailAnchor* targetAnchor = anchor(anchorId);
	if (!targetAnchor)
	{
		return sample;
	}
	const RailSegment* segment = network.segment(targetAnchor->location.segmentId);
	if (!segment || segment->cachedLength <= 0.0001f)
	{
		return sample;
	}
	const float distance = std::max(0.0f, std::min(targetAnchor->location.distanceOnSegment, segment->cachedLength));
	sample.position = segment->positionByDistance(distance);
	sample.tangent = segment->tangentByDistance(distance);
	sample.valid = true;
	return sample;
}

bool RailAnchorManager::pickLocationOnTrack(const RailNetwork& network, const vec3& point, float maxDistance,
	RailTrackLocation& outLocation, vec3* outPosition) const
{
	RailSegmentPick segmentPick = network.findNearestSegment(point, maxDistance);
	if (segmentPick.segmentId == InvalidRailSegmentId)
	{
		return false;
	}
	outLocation.segmentId = segmentPick.segmentId;
	outLocation.distanceOnSegment = segmentPick.distanceOnSegment;
	if (outPosition)
	{
		*outPosition = segmentPick.position;
	}
	return true;
}

RailStationId RailStationManager::createStation(RailAnchorId anchorId, const std::string& name)
{
	RailStation station;
	station.id = m_nextStationId++;
	station.name = name.empty() ? "Station" + std::to_string(station.id) : name;

	RailPlatform platform;
	platform.id = m_nextPlatformId++;
	platform.stationId = station.id;
	platform.name = "Platform" + std::to_string(platform.id);
	platform.anchorId = anchorId;

	station.platforms.push_back(platform.id);
	m_platforms.push_back(platform);
	m_stations.push_back(station);
	return station.id;
}

bool RailStationManager::unserializeStation(const RailStation& station)
{
	if (station.id == InvalidRailStationId || this->station(station.id))
	{
		return false;
	}
	m_stations.push_back(station);
	m_nextStationId = std::max(m_nextStationId, station.id + 1);
	for (RailPlatformId platformId : station.platforms)
	{
		m_nextPlatformId = std::max(m_nextPlatformId, platformId + 1);
	}
	return true;
}

bool RailStationManager::unserializePlatform(const RailPlatform& platform)
{
	if (platform.id == InvalidRailPlatformId || this->platform(platform.id))
	{
		return false;
	}
	m_platforms.push_back(platform);
	m_nextPlatformId = std::max(m_nextPlatformId, platform.id + 1);
	return true;
}

bool RailStationManager::deleteStation(RailStationId stationId, RailAnchorManager& anchorManager)
{
	RailStation* targetStation = station(stationId);
	if (!targetStation)
	{
		return false;
	}

	for (RailPlatformId platformId : targetStation->platforms)
	{
		const RailPlatform* targetPlatform = platform(platformId);
		if (targetPlatform)
		{
			anchorManager.deleteAnchor(targetPlatform->anchorId);
		}
	}

	m_platforms.erase(std::remove_if(m_platforms.begin(), m_platforms.end(), [stationId](const RailPlatform& platform)
	{
		return platform.stationId == stationId;
	}), m_platforms.end());

	m_stations.erase(std::remove_if(m_stations.begin(), m_stations.end(), [stationId](const RailStation& station)
	{
		return station.id == stationId;
	}), m_stations.end());
	return true;
}

void RailStationManager::clear(RailAnchorManager& anchorManager)
{
	for (const RailPlatform& platform : m_platforms)
	{
		anchorManager.deleteAnchor(platform.anchorId);
	}
	clearWithoutAnchors();
}

void RailStationManager::clearWithoutAnchors()
{
	m_stations.clear();
	m_platforms.clear();
	m_nextStationId = 1;
	m_nextPlatformId = 1;
}

RailStation* RailStationManager::station(RailStationId stationId)
{
	for (RailStation& candidate : m_stations)
	{
		if (candidate.id == stationId)
		{
			return &candidate;
		}
	}
	return nullptr;
}

const RailStation* RailStationManager::station(RailStationId stationId) const
{
	for (const RailStation& candidate : m_stations)
	{
		if (candidate.id == stationId)
		{
			return &candidate;
		}
	}
	return nullptr;
}

RailPlatform* RailStationManager::platform(RailPlatformId platformId)
{
	for (RailPlatform& candidate : m_platforms)
	{
		if (candidate.id == platformId)
		{
			return &candidate;
		}
	}
	return nullptr;
}

const RailPlatform* RailStationManager::platform(RailPlatformId platformId) const
{
	for (const RailPlatform& candidate : m_platforms)
	{
		if (candidate.id == platformId)
		{
			return &candidate;
		}
	}
	return nullptr;
}

const std::vector<RailStation>& RailStationManager::stations() const
{
	return m_stations;
}

const std::vector<RailPlatform>& RailStationManager::platforms() const
{
	return m_platforms;
}

bool RailStationManager::renameStation(RailStationId stationId, const std::string& name)
{
	if (name.empty())
	{
		return false;
	}
	RailStation* targetStation = station(stationId);
	if (!targetStation)
	{
		return false;
	}
	targetStation->name = name;
	return true;
}

const RailPlatform* RailStationManager::firstPlatform(RailStationId stationId) const
{
	const RailStation* targetStation = station(stationId);
	if (!targetStation || targetStation->platforms.empty())
	{
		return nullptr;
	}
	return platform(targetStation->platforms.front());
}

RailAnchorId RailStationManager::anchorForStation(RailStationId stationId) const
{
	const RailPlatform* targetPlatform = firstPlatform(stationId);
	return targetPlatform ? targetPlatform->anchorId : InvalidRailAnchorId;
}

RailStationId RailStationManager::findNearestStation(const RailNetwork& network, const RailAnchorManager& anchorManager,
	const vec3& point, float maxDistance) const
{
	RailStationId result = InvalidRailStationId;
	float bestDistance = maxDistance;
	for (const RailStation& candidate : m_stations)
	{
		const RailAnchorId anchorId = anchorForStation(candidate.id);
		RailAnchorSample sample = anchorManager.sampleAnchor(network, anchorId);
		if (!sample.valid)
		{
			continue;
		}
		const float distance = sample.position.distance(point);
		if (distance <= bestDistance)
		{
			bestDistance = distance;
			result = candidate.id;
		}
	}
	return result;
}

RailRoutePointId RailRoutePointManager::createRoutePoint(RailAnchorId anchorId, const std::string& name)
{
	RailRoutePoint routePoint;
	routePoint.id = m_nextRoutePointId++;
	routePoint.name = name.empty() ? "RoutePoint" + std::to_string(routePoint.id) : name;
	routePoint.anchorId = anchorId;
	m_routePoints.push_back(routePoint);
	return routePoint.id;
}

bool RailRoutePointManager::unserializeRoutePoint(const RailRoutePoint& routePoint)
{
	if (routePoint.id == InvalidRailRoutePointId || this->routePoint(routePoint.id))
	{
		return false;
	}
	m_routePoints.push_back(routePoint);
	m_nextRoutePointId = std::max(m_nextRoutePointId, routePoint.id + 1);
	return true;
}

bool RailRoutePointManager::deleteRoutePoint(RailRoutePointId routePointId, RailAnchorManager& anchorManager)
{
	const RailRoutePoint* targetRoutePoint = routePoint(routePointId);
	if (!targetRoutePoint)
	{
		return false;
	}
	anchorManager.deleteAnchor(targetRoutePoint->anchorId);
	m_routePoints.erase(std::remove_if(m_routePoints.begin(), m_routePoints.end(),
		[routePointId](const RailRoutePoint& routePoint)
	{
		return routePoint.id == routePointId;
	}), m_routePoints.end());
	return true;
}

void RailRoutePointManager::clear(RailAnchorManager& anchorManager)
{
	for (const RailRoutePoint& routePoint : m_routePoints)
	{
		anchorManager.deleteAnchor(routePoint.anchorId);
	}
	clearWithoutAnchors();
}

void RailRoutePointManager::clearWithoutAnchors()
{
	m_routePoints.clear();
	m_nextRoutePointId = 1;
}

RailRoutePoint* RailRoutePointManager::routePoint(RailRoutePointId routePointId)
{
	for (RailRoutePoint& candidate : m_routePoints)
	{
		if (candidate.id == routePointId)
		{
			return &candidate;
		}
	}
	return nullptr;
}

const RailRoutePoint* RailRoutePointManager::routePoint(RailRoutePointId routePointId) const
{
	for (const RailRoutePoint& candidate : m_routePoints)
	{
		if (candidate.id == routePointId)
		{
			return &candidate;
		}
	}
	return nullptr;
}

const std::vector<RailRoutePoint>& RailRoutePointManager::routePoints() const
{
	return m_routePoints;
}

bool RailRoutePointManager::renameRoutePoint(RailRoutePointId routePointId, const std::string& name)
{
	if (name.empty())
	{
		return false;
	}
	RailRoutePoint* targetRoutePoint = routePoint(routePointId);
	if (!targetRoutePoint)
	{
		return false;
	}
	targetRoutePoint->name = name;
	return true;
}

RailAnchorId RailRoutePointManager::anchorForRoutePoint(RailRoutePointId routePointId) const
{
	const RailRoutePoint* targetRoutePoint = routePoint(routePointId);
	return targetRoutePoint ? targetRoutePoint->anchorId : InvalidRailAnchorId;
}

RailRoutePointId RailRoutePointManager::findNearestRoutePoint(const RailNetwork& network,
	const RailAnchorManager& anchorManager, const vec3& point, float maxDistance) const
{
	RailRoutePointId result = InvalidRailRoutePointId;
	float bestDistance = maxDistance;
	for (const RailRoutePoint& candidate : m_routePoints)
	{
		RailAnchorSample sample = anchorManager.sampleAnchor(network, candidate.anchorId);
		if (!sample.valid)
		{
			continue;
		}
		const float distance = sample.position.distance(point);
		if (distance <= bestDistance)
		{
			bestDistance = distance;
			result = candidate.id;
		}
	}
	return result;
}

} // namespace tzw
