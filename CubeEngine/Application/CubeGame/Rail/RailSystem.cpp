#include "RailSystem.h"

#include "CubeGame/BuildingSystem.h"
#include "EngineSrc/Scene/ScenePickingSystem.h"

#include <algorithm>

namespace tzw {

namespace
{
constexpr int RailSaveVersion = 1;

float stationPickRadius(const RailConfig& config)
{
	return std::max(config.nodePickRadius * 2.0f, 1.75f);
}

float routePointPickRadius(const RailConfig& config)
{
	return std::max(config.nodePickRadius * 1.5f, 1.25f);
}

bool isSameControlPoint(const RailLineControlPoint& lhs, const RailLineControlPoint& rhs)
{
	if (lhs.kind != rhs.kind)
	{
		return false;
	}
	if (lhs.kind == RailLineControlPointKind::Station)
	{
		return lhs.stationId == rhs.stationId;
	}
	return lhs.routePointId == rhs.routePointId;
}

rapidjson::Value makeVec3Value(const vec3& value, rapidjson::Document::AllocatorType& allocator)
{
	rapidjson::Value result(rapidjson::kArrayType);
	result.PushBack(value.x, allocator);
	result.PushBack(value.y, allocator);
	result.PushBack(value.z, allocator);
	return result;
}

bool readVec3Value(const rapidjson::Value& value, vec3& outValue)
{
	if (!value.IsArray() || value.Size() != 3 || !value[0].IsNumber() || !value[1].IsNumber() || !value[2].IsNumber())
	{
		return false;
	}
	outValue = vec3(value[0].GetFloat(), value[1].GetFloat(), value[2].GetFloat());
	return true;
}

bool readRequiredInt(const rapidjson::Value& objectValue, const char* memberName, int& outValue)
{
	if (!objectValue.IsObject() || !objectValue.HasMember(memberName) || !objectValue[memberName].IsInt())
	{
		return false;
	}
	outValue = objectValue[memberName].GetInt();
	return true;
}

bool readRequiredFloat(const rapidjson::Value& objectValue, const char* memberName, float& outValue)
{
	if (!objectValue.IsObject() || !objectValue.HasMember(memberName) || !objectValue[memberName].IsNumber())
	{
		return false;
	}
	outValue = objectValue[memberName].GetFloat();
	return true;
}

bool readRequiredBool(const rapidjson::Value& objectValue, const char* memberName, bool& outValue)
{
	if (!objectValue.IsObject() || !objectValue.HasMember(memberName) || !objectValue[memberName].IsBool())
	{
		return false;
	}
	outValue = objectValue[memberName].GetBool();
	return true;
}

bool readOptionalBool(const rapidjson::Value& objectValue, const char* memberName, bool& outValue)
{
	if (!objectValue.IsObject() || !objectValue.HasMember(memberName))
	{
		return true;
	}
	if (!objectValue[memberName].IsBool())
	{
		return false;
	}
	outValue = objectValue[memberName].GetBool();
	return true;
}

bool readOptionalInt(const rapidjson::Value& objectValue, const char* memberName, int& outValue)
{
	if (!objectValue.IsObject() || !objectValue.HasMember(memberName))
	{
		return true;
	}
	if (!objectValue[memberName].IsInt())
	{
		return false;
	}
	outValue = objectValue[memberName].GetInt();
	return true;
}

bool readOptionalFloat(const rapidjson::Value& objectValue, const char* memberName, float& outValue)
{
	if (!objectValue.IsObject() || !objectValue.HasMember(memberName))
	{
		return true;
	}
	if (!objectValue[memberName].IsNumber())
	{
		return false;
	}
	outValue = objectValue[memberName].GetFloat();
	return true;
}

std::string readOptionalString(const rapidjson::Value& objectValue, const char* memberName)
{
	if (!objectValue.IsObject() || !objectValue.HasMember(memberName) || !objectValue[memberName].IsString())
	{
		return "";
	}
	return objectValue[memberName].GetString();
}

bool readRequiredVec3(const rapidjson::Value& objectValue, const char* memberName, vec3& outValue)
{
	if (!objectValue.IsObject() || !objectValue.HasMember(memberName))
	{
		return false;
	}
	return readVec3Value(objectValue[memberName], outValue);
}

void addStringMember(rapidjson::Value& objectValue, const char* memberName, const std::string& value,
	rapidjson::Document::AllocatorType& allocator)
{
	objectValue.AddMember(rapidjson::Value(memberName, allocator),
		rapidjson::Value(value.c_str(), allocator), allocator);
}
}

RailInspectTarget RailInspectTarget::train(RailTrainId trainId)
{
	RailInspectTarget target;
	target.kind = RailInspectTargetKind::Train;
	target.trainId = trainId;
	return target;
}

RailInspectTarget RailInspectTarget::station(RailStationId stationId)
{
	RailInspectTarget target;
	target.kind = RailInspectTargetKind::Station;
	target.stationId = stationId;
	return target;
}

RailInspectTarget RailInspectTarget::routePoint(RailRoutePointId routePointId)
{
	RailInspectTarget target;
	target.kind = RailInspectTargetKind::RoutePoint;
	target.routePointId = routePointId;
	return target;
}

bool RailInspectTarget::isValid() const
{
	switch (kind)
	{
	case RailInspectTargetKind::Train:
		return trainId != InvalidRailTrainId;
	case RailInspectTargetKind::Station:
		return stationId != InvalidRailStationId;
	case RailInspectTargetKind::RoutePoint:
		return routePointId != InvalidRailRoutePointId;
	case RailInspectTargetKind::None:
	default:
		return false;
	}
}

bool RailInspectTarget::equals(const RailInspectTarget& other) const
{
	if (kind != other.kind)
	{
		return false;
	}
	switch (kind)
	{
	case RailInspectTargetKind::Train:
		return trainId == other.trainId;
	case RailInspectTargetKind::Station:
		return stationId == other.stationId;
	case RailInspectTargetKind::RoutePoint:
		return routePointId == other.routePointId;
	case RailInspectTargetKind::None:
	default:
		return true;
	}
}

RailSystem::RailSystem()
{
	m_buildTool.bind(&m_network, &m_anchorManager, &m_config);
}

RailSystem::~RailSystem()
{
	clear();
}

void RailSystem::init()
{
	m_buildTool.bind(&m_network, &m_anchorManager, &m_config);
	m_persistentVisuals.markDirty();
}

void RailSystem::update(Node* sceneRoot, float deltaSeconds)
{
	if (!sceneRoot)
	{
		return;
	}

	m_editorVisuals.prepare(sceneRoot);
	m_trainManager.update(deltaSeconds, m_lineManager, m_stationManager);
	m_trainManager.updateWorldPoses(m_network, m_lineManager);
	m_persistentVisuals.sync(sceneRoot, m_network, m_config, m_anchorManager, m_stationManager,
		m_routePointManager, m_lineManager, m_trainManager);
	if (m_lineOverviewVisible)
	{
		m_editorVisuals.showLineOverview(sceneRoot, m_network, m_lineManager, m_previewLineId);
	}
	else
	{
		m_editorVisuals.clearLineOverview();
	}
	if (m_previewLineId != InvalidRailLineId)
	{
		const RailLine* previewLine = m_lineManager.line(m_previewLineId);
		m_editorVisuals.showLinePreview(sceneRoot, m_network, previewLine);
		m_editorVisuals.showSelectedLineControlPointLabels(m_network, m_anchorManager, m_stationManager,
			m_routePointManager, previewLine);
	}
	else
	{
		m_editorVisuals.hideSelectedLineControlPointLabels();
	}
}

void RailSystem::clear()
{
	m_buildTool.clearPending();
	m_network.clear();
	m_stationManager.clear(m_anchorManager);
	m_routePointManager.clear(m_anchorManager);
	m_anchorManager.clear();
	m_lineManager.clear();
	m_trainManager.clear();
	m_persistentVisuals.clear();
	m_editorVisuals.clear();
	m_previewLineId = InvalidRailLineId;
	m_lineOverviewVisible = false;
}

void RailSystem::serialize(rapidjson::Value& railData, rapidjson::Document::AllocatorType& allocator) const
{
	railData.SetObject();
	railData.AddMember("Version", RailSaveVersion, allocator);

	rapidjson::Value nodes(rapidjson::kArrayType);
	for (const auto& pair : m_network.nodes())
	{
		const RailNode* node = pair.second.get();
		if (!node)
		{
			continue;
		}
		rapidjson::Value nodeData(rapidjson::kObjectType);
		nodeData.AddMember("Id", node->id, allocator);
		nodeData.AddMember("Position", makeVec3Value(node->position, allocator), allocator);
		nodeData.AddMember("PreferredTangent", makeVec3Value(node->preferredTangent, allocator), allocator);
		nodeData.AddMember("IsConnectable", node->isConnectable, allocator);
		nodes.PushBack(nodeData, allocator);
	}
	railData.AddMember("Nodes", nodes, allocator);

	rapidjson::Value segments(rapidjson::kArrayType);
	for (const auto& pair : m_network.segments())
	{
		const RailSegment& segment = pair.second;
		rapidjson::Value segmentData(rapidjson::kObjectType);
		segmentData.AddMember("Id", segment.id, allocator);
		segmentData.AddMember("StartNode", segment.startNode, allocator);
		segmentData.AddMember("EndNode", segment.endNode, allocator);
		segmentData.AddMember("StartTangent", makeVec3Value(segment.startTangent, allocator), allocator);
		segmentData.AddMember("EndTangent", makeVec3Value(segment.endTangent, allocator), allocator);
		segments.PushBack(segmentData, allocator);
	}
	railData.AddMember("Segments", segments, allocator);

	rapidjson::Value anchors(rapidjson::kArrayType);
	for (const RailAnchor& anchor : m_anchorManager.anchors())
	{
		rapidjson::Value anchorData(rapidjson::kObjectType);
		anchorData.AddMember("Id", anchor.id, allocator);
		anchorData.AddMember("SegmentId", anchor.location.segmentId, allocator);
		anchorData.AddMember("DistanceOnSegment", anchor.location.distanceOnSegment, allocator);
		anchors.PushBack(anchorData, allocator);
	}
	railData.AddMember("Anchors", anchors, allocator);

	rapidjson::Value stations(rapidjson::kArrayType);
	for (const RailStation& station : m_stationManager.stations())
	{
		rapidjson::Value stationData(rapidjson::kObjectType);
		stationData.AddMember("Id", station.id, allocator);
		addStringMember(stationData, "Name", station.name, allocator);
		rapidjson::Value platformIds(rapidjson::kArrayType);
		for (RailPlatformId platformId : station.platforms)
		{
			platformIds.PushBack(platformId, allocator);
		}
		stationData.AddMember("Platforms", platformIds, allocator);
		stations.PushBack(stationData, allocator);
	}
	railData.AddMember("Stations", stations, allocator);

	rapidjson::Value platforms(rapidjson::kArrayType);
	for (const RailPlatform& platform : m_stationManager.platforms())
	{
		rapidjson::Value platformData(rapidjson::kObjectType);
		platformData.AddMember("Id", platform.id, allocator);
		platformData.AddMember("StationId", platform.stationId, allocator);
		addStringMember(platformData, "Name", platform.name, allocator);
		platformData.AddMember("AnchorId", platform.anchorId, allocator);
		platforms.PushBack(platformData, allocator);
	}
	railData.AddMember("Platforms", platforms, allocator);

	rapidjson::Value routePoints(rapidjson::kArrayType);
	for (const RailRoutePoint& routePoint : m_routePointManager.routePoints())
	{
		rapidjson::Value routePointData(rapidjson::kObjectType);
		routePointData.AddMember("Id", routePoint.id, allocator);
		addStringMember(routePointData, "Name", routePoint.name, allocator);
		routePointData.AddMember("AnchorId", routePoint.anchorId, allocator);
		routePoints.PushBack(routePointData, allocator);
	}
	railData.AddMember("RoutePoints", routePoints, allocator);

	rapidjson::Value lines(rapidjson::kArrayType);
	for (const RailLine& line : m_lineManager.lines())
	{
		rapidjson::Value lineData(rapidjson::kObjectType);
		lineData.AddMember("Id", line.id, allocator);
		addStringMember(lineData, "Name", line.name, allocator);
		rapidjson::Value controlPoints(rapidjson::kArrayType);
		for (const RailLineControlPoint& controlPoint : line.controlPoints)
		{
			rapidjson::Value controlPointData(rapidjson::kObjectType);
			controlPointData.AddMember("Kind", static_cast<int>(controlPoint.kind), allocator);
			controlPointData.AddMember("StationId", controlPoint.stationId, allocator);
			controlPointData.AddMember("RoutePointId", controlPoint.routePointId, allocator);
			controlPoints.PushBack(controlPointData, allocator);
		}
		lineData.AddMember("ControlPoints", controlPoints, allocator);
		lines.PushBack(lineData, allocator);
	}
	railData.AddMember("Lines", lines, allocator);

	rapidjson::Value trains(rapidjson::kArrayType);
	for (const RailTrain& train : m_trainManager.trains())
	{
		rapidjson::Value trainData(rapidjson::kObjectType);
		trainData.AddMember("Id", train.id, allocator);
		addStringMember(trainData, "Name", train.name, allocator);
		trainData.AddMember("LineId", train.lineId, allocator);
		trainData.AddMember("PlacementMode", static_cast<int>(train.placementMode), allocator);
		trainData.AddMember("CarriageCount", train.carriageCount, allocator);
		trainData.AddMember("Speed", train.speed, allocator);
		trainData.AddMember("HeadDistance", train.headDistance, allocator);
		trainData.AddMember("Direction", train.direction, allocator);
		trainData.AddMember("Active", train.active, allocator);
		trainData.AddMember("IsManuallyStopped", train.isManuallyStopped, allocator);
		trainData.AddMember("IsDwelling", train.isDwelling, allocator);
		trainData.AddMember("DwellRemainingSeconds", train.dwellRemainingSeconds, allocator);
		trainData.AddMember("DwellingStationId", train.dwellingStationId, allocator);
		trainData.AddMember("DwellingPlatformId", train.dwellingPlatformId, allocator);
		trainData.AddMember("LastStoppedStationId", train.lastStoppedStationId, allocator);
		trainData.AddMember("LastStoppedPlatformId", train.lastStoppedPlatformId, allocator);
		trainData.AddMember("LastStopDistance", train.lastStopDistance, allocator);
		trainData.AddMember("PoseValid", train.pose.valid, allocator);
		if (train.pose.valid)
		{
			trainData.AddMember("PosePosition", makeVec3Value(train.pose.position, allocator), allocator);
			trainData.AddMember("PoseTangent", makeVec3Value(train.pose.tangent, allocator), allocator);
		}
		trains.PushBack(trainData, allocator);
	}
	railData.AddMember("Trains", trains, allocator);
}

bool RailSystem::unserialize(const rapidjson::Value& railData)
{
	if (!railData.IsObject())
	{
		return false;
	}
	if (railData.HasMember("Version") && railData["Version"].IsInt()
		&& railData["Version"].GetInt() != RailSaveVersion)
	{
		return false;
	}

	auto hasArray = [&railData](const char* memberName)
	{
		return railData.HasMember(memberName) && railData[memberName].IsArray();
	};
	if (!hasArray("Nodes") || !hasArray("Segments") || !hasArray("Anchors") || !hasArray("Stations")
		|| !hasArray("Platforms") || !hasArray("RoutePoints") || !hasArray("Lines") || !hasArray("Trains"))
	{
		return false;
	}

	clear();

	for (const rapidjson::Value& nodeData : railData["Nodes"].GetArray())
	{
		int nodeId = InvalidRailNodeId;
		vec3 position;
		vec3 preferredTangent;
		bool isConnectable = true;
		if (!readRequiredInt(nodeData, "Id", nodeId)
			|| !readRequiredVec3(nodeData, "Position", position)
			|| !readRequiredVec3(nodeData, "PreferredTangent", preferredTangent)
			|| !readRequiredBool(nodeData, "IsConnectable", isConnectable))
		{
			return false;
		}
		if (m_network.unserializeNode(nodeId, position, preferredTangent, isConnectable) == InvalidRailNodeId)
		{
			return false;
		}
	}

	for (const rapidjson::Value& segmentData : railData["Segments"].GetArray())
	{
		int segmentId = InvalidRailSegmentId;
		int startNode = InvalidRailNodeId;
		int endNode = InvalidRailNodeId;
		vec3 startTangent;
		vec3 endTangent;
		if (!readRequiredInt(segmentData, "Id", segmentId)
			|| !readRequiredInt(segmentData, "StartNode", startNode)
			|| !readRequiredInt(segmentData, "EndNode", endNode)
			|| !readRequiredVec3(segmentData, "StartTangent", startTangent)
			|| !readRequiredVec3(segmentData, "EndTangent", endTangent))
		{
			return false;
		}
		if (m_network.unserializeSegment(segmentId, startNode, endNode, startTangent, endTangent) == InvalidRailSegmentId)
		{
			return false;
		}
	}

	for (const rapidjson::Value& anchorData : railData["Anchors"].GetArray())
	{
		int anchorId = InvalidRailAnchorId;
		RailTrackLocation location;
		if (!readRequiredInt(anchorData, "Id", anchorId)
			|| !readRequiredInt(anchorData, "SegmentId", location.segmentId)
			|| !readRequiredFloat(anchorData, "DistanceOnSegment", location.distanceOnSegment))
		{
			return false;
		}
		if (m_anchorManager.unserializeAnchor(anchorId, location) == InvalidRailAnchorId)
		{
			return false;
		}
	}

	for (const rapidjson::Value& stationData : railData["Stations"].GetArray())
	{
		RailStation station;
		if (!readRequiredInt(stationData, "Id", station.id)
			|| !stationData.HasMember("Platforms") || !stationData["Platforms"].IsArray())
		{
			return false;
		}
		station.name = readOptionalString(stationData, "Name");
		for (const rapidjson::Value& platformValue : stationData["Platforms"].GetArray())
		{
			if (!platformValue.IsInt())
			{
				return false;
			}
			station.platforms.push_back(platformValue.GetInt());
		}
		if (!m_stationManager.unserializeStation(station))
		{
			return false;
		}
	}

	for (const rapidjson::Value& platformData : railData["Platforms"].GetArray())
	{
		RailPlatform platform;
		if (!readRequiredInt(platformData, "Id", platform.id)
			|| !readRequiredInt(platformData, "StationId", platform.stationId)
			|| !readRequiredInt(platformData, "AnchorId", platform.anchorId))
		{
			return false;
		}
		platform.name = readOptionalString(platformData, "Name");
		if (!m_stationManager.unserializePlatform(platform))
		{
			return false;
		}
	}

	for (const rapidjson::Value& routePointData : railData["RoutePoints"].GetArray())
	{
		RailRoutePoint routePoint;
		if (!readRequiredInt(routePointData, "Id", routePoint.id)
			|| !readRequiredInt(routePointData, "AnchorId", routePoint.anchorId))
		{
			return false;
		}
		routePoint.name = readOptionalString(routePointData, "Name");
		if (!m_routePointManager.unserializeRoutePoint(routePoint))
		{
			return false;
		}
	}

	for (const rapidjson::Value& lineData : railData["Lines"].GetArray())
	{
		RailLine line;
		if (!readRequiredInt(lineData, "Id", line.id)
			|| !lineData.HasMember("ControlPoints") || !lineData["ControlPoints"].IsArray())
		{
			return false;
		}
		line.name = readOptionalString(lineData, "Name");
		for (const rapidjson::Value& controlPointData : lineData["ControlPoints"].GetArray())
		{
			RailLineControlPoint controlPoint;
			int kind = static_cast<int>(RailLineControlPointKind::Station);
			if (!readRequiredInt(controlPointData, "Kind", kind)
				|| !readOptionalInt(controlPointData, "StationId", controlPoint.stationId)
				|| !readOptionalInt(controlPointData, "RoutePointId", controlPoint.routePointId))
			{
				return false;
			}
			controlPoint.kind = static_cast<RailLineControlPointKind>(kind);
			line.controlPoints.push_back(controlPoint);
		}
		if (!m_lineManager.unserializeLine(line))
		{
			return false;
		}
	}

	for (const rapidjson::Value& trainData : railData["Trains"].GetArray())
	{
		RailTrain train;
		int placementMode = static_cast<int>(RailTrainPlacementMode::Unplaced);
		if (!readRequiredInt(trainData, "Id", train.id)
			|| !readOptionalInt(trainData, "LineId", train.lineId)
			|| !readOptionalInt(trainData, "PlacementMode", placementMode)
			|| !readOptionalInt(trainData, "CarriageCount", train.carriageCount)
			|| !readOptionalFloat(trainData, "Speed", train.speed)
			|| !readOptionalFloat(trainData, "HeadDistance", train.headDistance)
			|| !readOptionalInt(trainData, "Direction", train.direction)
			|| !readOptionalBool(trainData, "Active", train.active)
			|| !readOptionalBool(trainData, "IsManuallyStopped", train.isManuallyStopped)
			|| !readOptionalBool(trainData, "IsDwelling", train.isDwelling)
			|| !readOptionalFloat(trainData, "DwellRemainingSeconds", train.dwellRemainingSeconds)
			|| !readOptionalInt(trainData, "DwellingStationId", train.dwellingStationId)
			|| !readOptionalInt(trainData, "DwellingPlatformId", train.dwellingPlatformId)
			|| !readOptionalInt(trainData, "LastStoppedStationId", train.lastStoppedStationId)
			|| !readOptionalInt(trainData, "LastStoppedPlatformId", train.lastStoppedPlatformId)
			|| !readOptionalFloat(trainData, "LastStopDistance", train.lastStopDistance))
		{
			return false;
		}
		train.name = readOptionalString(trainData, "Name");
		train.placementMode = static_cast<RailTrainPlacementMode>(placementMode);
		if (!readOptionalBool(trainData, "PoseValid", train.pose.valid))
		{
			return false;
		}
		if (train.pose.valid)
		{
			if (!readRequiredVec3(trainData, "PosePosition", train.pose.position)
				|| !readRequiredVec3(trainData, "PoseTangent", train.pose.tangent))
			{
				return false;
			}
		}
		if (!m_trainManager.unserializeTrain(train))
		{
			return false;
		}
	}

	rebuildAllRailLines();
	m_trainManager.refreshAfterLineRebuild(m_lineManager);
	m_persistentVisuals.markDirty();
	m_editorVisuals.clear();
	m_previewLineId = InvalidRailLineId;
	return true;
}

bool RailSystem::handleTrackPrimaryClick(PlacementMode placementMode)
{
	const bool changed = m_buildTool.handlePrimaryClick(placementMode);
	if (changed)
	{
		markVisualDirty();
		rebuildAllRailLines();
	}
	return changed;
}

bool RailSystem::handleTrackAddPrimaryClick(PlacementMode placementMode)
{
	m_buildTool.setMode(RailBuildMode::Add);
	return handleTrackPrimaryClick(placementMode);
}

bool RailSystem::handleTrackDeletePrimaryClick(PlacementMode placementMode)
{
	m_buildTool.setMode(RailBuildMode::Delete);
	RailSegmentId segmentId = InvalidRailSegmentId;
	if (!m_buildTool.pickSegment(placementMode, segmentId))
	{
		return false;
	}
	m_buildTool.clearPending();
	const bool deletedSegment = m_network.deleteSegment(segmentId);
	if (!deletedSegment)
	{
		return false;
	}

	deletePointsAnchoredToSegment(segmentId);
	markVisualDirty();
	rebuildAllRailLines();
	return true;
}

void RailSystem::cancelTrackEdit()
{
	m_buildTool.handleSecondaryClick();
	markVisualDirty();
}

RailNetwork& RailSystem::network()
{
	return m_network;
}

const RailNetwork& RailSystem::network() const
{
	return m_network;
}

RailLineManager& RailSystem::lineManager()
{
	return m_lineManager;
}

const RailLineManager& RailSystem::lineManager() const
{
	return m_lineManager;
}

RailTrainManager& RailSystem::trainManager()
{
	return m_trainManager;
}

const RailTrainManager& RailSystem::trainManager() const
{
	return m_trainManager;
}

RailAnchorManager& RailSystem::anchorManager()
{
	return m_anchorManager;
}

const RailAnchorManager& RailSystem::anchorManager() const
{
	return m_anchorManager;
}

RailStationManager& RailSystem::stationManager()
{
	return m_stationManager;
}

const RailStationManager& RailSystem::stationManager() const
{
	return m_stationManager;
}

RailRoutePointManager& RailSystem::routePointManager()
{
	return m_routePointManager;
}

const RailRoutePointManager& RailSystem::routePointManager() const
{
	return m_routePointManager;
}

RailEditResult RailSystem::handleStationAddPrimaryClick(PlacementMode placementMode, const std::string& name)
{
	RailAnchorId anchorId = InvalidRailAnchorId;
	if (!createAnchorAtHit(placementMode, anchorId))
	{
		return RailEditResult::InvalidOperation;
	}
	m_stationManager.createStation(anchorId, name);
	rebuildAllRailLines();
	return RailEditResult::Success;
}

bool RailSystem::handleStationDeletePrimaryClick(PlacementMode placementMode)
{
	const vec3 terrainHit = BuildingSystem::shared()->hitTerrain(placementMode);
	if (!BuildingSystem::isValidHitPoint(terrainHit))
	{
		return false;
	}

	const RailStationId stationId = m_stationManager.findNearestStation(
		m_network, m_anchorManager, terrainHit, stationPickRadius(m_config));
	if (stationId == InvalidRailStationId)
	{
		return false;
	}

	const bool changed = m_stationManager.deleteStation(stationId, m_anchorManager);
	if (changed)
	{
		rebuildAllRailLines();
	}
	return changed;
}

RailEditResult RailSystem::handleRoutePointAddPrimaryClick(PlacementMode placementMode, const std::string& name)
{
	RailAnchorId anchorId = InvalidRailAnchorId;
	if (!createAnchorAtHit(placementMode, anchorId))
	{
		return RailEditResult::InvalidOperation;
	}
	m_routePointManager.createRoutePoint(anchorId, name);
	rebuildAllRailLines();
	return RailEditResult::Success;
}

bool RailSystem::handleRoutePointDeletePrimaryClick(PlacementMode placementMode)
{
	const vec3 terrainHit = BuildingSystem::shared()->hitTerrain(placementMode);
	if (!BuildingSystem::isValidHitPoint(terrainHit))
	{
		return false;
	}

	const RailRoutePointId routePointId = m_routePointManager.findNearestRoutePoint(
		m_network, m_anchorManager, terrainHit, routePointPickRadius(m_config));
	if (routePointId == InvalidRailRoutePointId)
	{
		return false;
	}

	const bool changed = m_routePointManager.deleteRoutePoint(routePointId, m_anchorManager);
	if (changed)
	{
		rebuildAllRailLines();
	}
	return changed;
}

bool RailSystem::renameStation(RailStationId stationId, const std::string& name)
{
	return m_stationManager.renameStation(stationId, name);
}

bool RailSystem::renameRoutePoint(RailRoutePointId routePointId, const std::string& name)
{
	return m_routePointManager.renameRoutePoint(routePointId, name);
}

bool RailSystem::pickInspectTarget(PlacementMode placementMode, RailInspectTarget& outTarget) const
{
	outTarget = RailInspectTarget();
	if (placementMode != PlacementMode::CursorBased)
	{
		return false;
	}

	Ray pickRay;
	if (!ScenePickingSystem::makeCursorRay(pickRay))
	{
		return false;
	}

	PickResult pickResult;
	if (!ScenePickingSystem::shared()->pick(pickRay, RailInspectPickCategory, pickResult))
	{
		return false;
	}

	const RailInspectTargetKind kind = static_cast<RailInspectTargetKind>(pickResult.payload.type);
	switch (kind)
	{
	case RailInspectTargetKind::Train:
		if (m_trainManager.train(pickResult.payload.id0))
		{
			outTarget = RailInspectTarget::train(pickResult.payload.id0);
			return true;
		}
		break;
	case RailInspectTargetKind::Station:
		if (m_stationManager.station(pickResult.payload.id0))
		{
			outTarget = RailInspectTarget::station(pickResult.payload.id0);
			return true;
		}
		break;
	case RailInspectTargetKind::RoutePoint:
		if (m_routePointManager.routePoint(pickResult.payload.id0))
		{
			outTarget = RailInspectTarget::routePoint(pickResult.payload.id0);
			return true;
		}
		break;
	case RailInspectTargetKind::None:
	default:
		break;
	}

	return false;
}

void RailSystem::setTrainOutline(RailTrainId trainId, bool enabled, vec4 color)
{
	m_persistentVisuals.setTrainOutline(trainId, enabled, color);
}

void RailSystem::setStationOutline(RailStationId stationId, bool enabled, vec4 color)
{
	m_persistentVisuals.setStationOutline(stationId, enabled, color);
}

void RailSystem::setRoutePointOutline(RailRoutePointId routePointId, bool enabled, vec4 color)
{
	m_persistentVisuals.setRoutePointOutline(routePointId, enabled, color);
}

void RailSystem::clearOutlines()
{
	m_persistentVisuals.clearOutlines();
}

bool RailSystem::addStationToSelectedLine(RailStationId stationId)
{
	if (!m_stationManager.station(stationId))
	{
		return false;
	}

	RailLineControlPoint controlPoint;
	controlPoint.kind = RailLineControlPointKind::Station;
	controlPoint.stationId = stationId;
	return addControlPointToSelectedLine(controlPoint);
}

bool RailSystem::addRoutePointToSelectedLine(RailRoutePointId routePointId)
{
	if (!m_routePointManager.routePoint(routePointId))
	{
		return false;
	}

	RailLineControlPoint controlPoint;
	controlPoint.kind = RailLineControlPointKind::RoutePoint;
	controlPoint.routePointId = routePointId;
	return addControlPointToSelectedLine(controlPoint);
}

RailEditResult RailSystem::addPickedControlPointToSelectedLine(PlacementMode placementMode)
{
	if (addPickedStationToSelectedLine(placementMode))
	{
		return RailEditResult::Success;
	}
	if (addPickedRoutePointToSelectedLine(placementMode))
	{
		return RailEditResult::Success;
	}
	return RailEditResult::InvalidOperation;
}

bool RailSystem::addPickedStationToSelectedLine(PlacementMode placementMode)
{
	const vec3 terrainHit = BuildingSystem::shared()->hitTerrain(placementMode);
	if (!BuildingSystem::isValidHitPoint(terrainHit))
	{
		return false;
	}

	const RailStationId stationId = m_stationManager.findNearestStation(
		m_network, m_anchorManager, terrainHit, stationPickRadius(m_config));
	if (stationId == InvalidRailStationId)
	{
		return false;
	}

	return addStationToSelectedLine(stationId);
}

bool RailSystem::addPickedRoutePointToSelectedLine(PlacementMode placementMode)
{
	const vec3 terrainHit = BuildingSystem::shared()->hitTerrain(placementMode);
	if (!BuildingSystem::isValidHitPoint(terrainHit))
	{
		return false;
	}

	const RailRoutePointId routePointId = m_routePointManager.findNearestRoutePoint(
		m_network, m_anchorManager, terrainHit, routePointPickRadius(m_config));
	if (routePointId == InvalidRailRoutePointId)
	{
		return false;
	}

	return addRoutePointToSelectedLine(routePointId);
}

void RailSystem::syncTrackAddVisuals(PlacementMode placementMode)
{
	m_buildTool.setMode(RailBuildMode::Add);
	m_editorVisuals.showTrackAddPreview(m_network, m_buildTool, m_config, placementMode);
}

void RailSystem::syncTrackDeleteVisuals()
{
	m_buildTool.setMode(RailBuildMode::Delete);
	m_editorVisuals.showTrackDeleteHints(m_network, m_config);
}

void RailSystem::showStationEditorVisuals(bool deleteMode)
{
	m_editorVisuals.showStationBillboards(m_network, m_anchorManager, m_stationManager, deleteMode,
		[this, deleteMode](RailStationId stationId)
	{
		if (!deleteMode)
		{
			return;
		}
		if (m_stationManager.deleteStation(stationId, m_anchorManager))
		{
			rebuildAllRailLines();
		}
	});
}

void RailSystem::showRoutePointEditorVisuals(bool deleteMode)
{
	m_editorVisuals.showRoutePointBillboards(m_network, m_anchorManager, m_routePointManager, deleteMode,
		[this, deleteMode](RailRoutePointId routePointId)
	{
		if (!deleteMode)
		{
			return;
		}
		if (m_routePointManager.deleteRoutePoint(routePointId, m_anchorManager))
		{
			rebuildAllRailLines();
		}
	});
}

void RailSystem::showLineAddControlBillboards()
{
	const RailLine* selectedLine = m_lineManager.line(m_lineManager.selectedLineId());
	m_editorVisuals.showLineAddControlBillboards(m_network, m_anchorManager, m_stationManager, m_routePointManager,
		selectedLine,
		[this](RailStationId stationId)
	{
		addStationToSelectedLine(stationId);
	},
		[this](RailRoutePointId routePointId)
	{
		addRoutePointToSelectedLine(routePointId);
	});
}

void RailSystem::showLineRemoveControlBillboards()
{
	const RailLineId selectedLineId = m_lineManager.selectedLineId();
	const RailLine* line = m_lineManager.line(selectedLineId);
	m_editorVisuals.showLineRemoveControlBillboards(m_network, m_anchorManager, m_stationManager,
		m_routePointManager, line,
		[this, selectedLineId](int controlPointIndex)
	{
		if (selectedLineId == InvalidRailLineId)
		{
			return;
		}
		if (m_lineManager.removeControlPointAt(selectedLineId, controlPointIndex, m_network,
			m_anchorManager, m_stationManager, m_routePointManager))
		{
			m_previewLineId = selectedLineId;
			m_trainManager.refreshAfterLineRebuild(m_lineManager);
		}
	});
}

void RailSystem::hideEditorVisuals()
{
	m_editorVisuals.hideInteractionVisuals();
}

void RailSystem::hideAllEditorVisuals()
{
	cancelTrackEdit();
	hideEditorVisuals();
	setLineOverviewVisible(false);
	clearLinePreview();
}

void RailSystem::setLineOverviewVisible(bool isVisible)
{
	if (m_lineOverviewVisible == isVisible)
	{
		return;
	}
	m_lineOverviewVisible = isVisible;
	if (!m_lineOverviewVisible)
	{
		m_editorVisuals.clearLineOverview();
	}
}

void RailSystem::setLinePreview(RailLineId lineId)
{
	if (m_previewLineId == lineId)
	{
		return;
	}
	m_previewLineId = lineId;
}

void RailSystem::clearLinePreview()
{
	if (m_previewLineId == InvalidRailLineId)
	{
		return;
	}
	m_previewLineId = InvalidRailLineId;
	m_editorVisuals.clearLinePreview();
	m_editorVisuals.hideSelectedLineControlPointLabels();
}

void RailSystem::rebuildAllRailLines()
{
	m_lineManager.rebuildAll(m_network, m_anchorManager, m_stationManager, m_routePointManager);
	m_trainManager.refreshAfterLineRebuild(m_lineManager);
}

void RailSystem::markVisualDirty()
{
	m_persistentVisuals.markDirty();
}

bool RailSystem::deletePointsAnchoredToSegment(RailSegmentId segmentId)
{
	bool changed = false;

	std::vector<RailStationId> stationsToDelete;
	for (const RailStation& station : m_stationManager.stations())
	{
		if (isAnchorOnSegment(m_stationManager.anchorForStation(station.id), segmentId))
		{
			stationsToDelete.push_back(station.id);
		}
	}
	for (RailStationId stationId : stationsToDelete)
	{
		changed = m_stationManager.deleteStation(stationId, m_anchorManager) || changed;
	}

	std::vector<RailRoutePointId> routePointsToDelete;
	for (const RailRoutePoint& routePoint : m_routePointManager.routePoints())
	{
		if (isAnchorOnSegment(routePoint.anchorId, segmentId))
		{
			routePointsToDelete.push_back(routePoint.id);
		}
	}
	for (RailRoutePointId routePointId : routePointsToDelete)
	{
		changed = m_routePointManager.deleteRoutePoint(routePointId, m_anchorManager) || changed;
	}

	return changed;
}

bool RailSystem::isAnchorOnSegment(RailAnchorId anchorId, RailSegmentId segmentId) const
{
	const RailAnchor* anchor = m_anchorManager.anchor(anchorId);
	return anchor && anchor->location.segmentId == segmentId;
}

bool RailSystem::createAnchorAtHit(PlacementMode placementMode, RailAnchorId& outAnchorId)
{
	outAnchorId = InvalidRailAnchorId;
	const vec3 terrainHit = BuildingSystem::shared()->hitTerrain(placementMode);
	if (!BuildingSystem::isValidHitPoint(terrainHit))
	{
		return false;
	}

	RailTrackLocation location;
	if (!m_anchorManager.pickLocationOnTrack(m_network, terrainHit, m_config.segmentPickRadius, location))
	{
		return false;
	}

	outAnchorId = m_anchorManager.createAnchor(location);
	return outAnchorId != InvalidRailAnchorId;
}

bool RailSystem::addControlPointToSelectedLine(const RailLineControlPoint& controlPoint)
{
	const RailLineId selectedLineId = m_lineManager.selectedLineId();
	if (selectedLineId == InvalidRailLineId)
	{
		return false;
	}
	RailLine* selectedLine = m_lineManager.line(selectedLineId);
	if (selectedLine && !selectedLine->controlPoints.empty()
		&& isSameControlPoint(selectedLine->controlPoints.back(), controlPoint))
	{
		return false;
	}

	const bool changed = m_lineManager.addControlPoint(selectedLineId, controlPoint, m_network,
		m_anchorManager, m_stationManager, m_routePointManager);
	if (changed)
	{
		m_previewLineId = selectedLineId;
		m_trainManager.refreshAfterLineRebuild(m_lineManager);
	}
	return changed;
}

} // namespace tzw
