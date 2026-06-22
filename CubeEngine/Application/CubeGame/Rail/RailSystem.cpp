#include "RailSystem.h"

#include "CubeGame/BuildingSystem.h"
#include "EngineSrc/3D/Primitive/CubePrimitive.h"
#include "EngineSrc/3D/Primitive/LinePrimitive.h"
#include "EngineSrc/Base/Node.h"
#include "EngineSrc/Math/vec4.h"

#include <algorithm>

namespace tzw {

namespace
{
float stationPickRadius(const RailConfig& config)
{
	return std::max(config.nodePickRadius * 2.0f, 1.75f);
}

float routePointPickRadius(const RailConfig& config)
{
	return std::max(config.nodePickRadius * 1.5f, 1.25f);
}
}

RailSystem::RailSystem()
{
	m_buildTool.bind(&m_network, &m_config);
}

RailSystem::~RailSystem()
{
	clear();
}

void RailSystem::init()
{
	m_buildTool.bind(&m_network, &m_config);
	m_visualDirty = true;
}

void RailSystem::update(Node* sceneRoot, float deltaSeconds)
{
	if (!sceneRoot)
	{
		return;
	}

	ensureVisualRoot(sceneRoot);
	if (m_visualDirty)
	{
		syncVisuals(sceneRoot);
	}
	syncStationVisuals(true, false, false);
	syncRoutePointVisuals(true, false, false);
	m_trainManager.update(deltaSeconds, m_lineManager, m_stationManager);
	m_trainManager.updateWorldPoses(m_network, m_lineManager);
	m_trainVisuals.sync(sceneRoot, m_network, m_lineManager, m_trainManager);
	if (m_previewLineId != InvalidRailLineId)
	{
		m_linePreviewVisual.show(sceneRoot, m_network, m_lineManager.line(m_previewLineId));
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
	m_trainVisuals.clear();
	m_linePreviewVisual.clear();
	m_previewLineId = InvalidRailLineId;
	clearVisuals();
	m_visualDirty = true;
}

bool RailSystem::handleTrackPrimaryClick(PlacementMode placementMode)
{
	const bool changed = m_buildTool.handlePrimaryClick(placementMode);
	markVisualDirty();
	if (changed)
	{
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
	return handleTrackPrimaryClick(placementMode);
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

bool RailSystem::handleStationAddPrimaryClick(PlacementMode placementMode)
{
	RailAnchorId anchorId = InvalidRailAnchorId;
	if (!createAnchorAtHit(placementMode, anchorId))
	{
		return false;
	}
	m_stationManager.createStation(anchorId);
	rebuildAllRailLines();
	markVisualDirty();
	return true;
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
		markVisualDirty();
	}
	return changed;
}

bool RailSystem::handleRoutePointAddPrimaryClick(PlacementMode placementMode)
{
	RailAnchorId anchorId = InvalidRailAnchorId;
	if (!createAnchorAtHit(placementMode, anchorId))
	{
		return false;
	}
	m_routePointManager.createRoutePoint(anchorId);
	rebuildAllRailLines();
	markVisualDirty();
	return true;
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
		markVisualDirty();
	}
	return changed;
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

bool RailSystem::addPickedControlPointToSelectedLine(PlacementMode placementMode)
{
	if (addPickedStationToSelectedLine(placementMode))
	{
		return true;
	}
	return addPickedRoutePointToSelectedLine(placementMode);
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

void RailSystem::setLinePreview(RailLineId lineId)
{
	if (m_previewLineId == lineId)
	{
		return;
	}
	m_previewLineId = lineId;
	markVisualDirty();
}

void RailSystem::clearLinePreview()
{
	if (m_previewLineId == InvalidRailLineId)
	{
		return;
	}
	m_previewLineId = InvalidRailLineId;
	m_linePreviewVisual.clear();
	markVisualDirty();
}

void RailSystem::rebuildAllRailLines()
{
	m_lineManager.rebuildAll(m_network, m_anchorManager, m_stationManager, m_routePointManager);
	m_trainManager.refreshAfterLineRebuild(m_lineManager);
}

void RailSystem::markVisualDirty()
{
	m_visualDirty = true;
}

void RailSystem::syncVisuals(Node* sceneRoot)
{
	ensureVisualRoot(sceneRoot);
	if (!m_visualRoot || !m_lineVisual)
	{
		return;
	}

	RailTrackVisualData visualData;
	m_trackMesh.build(m_network, m_config, visualData);

	m_lineVisual->clear();
	for (const RailLineVisual& line : visualData.lines)
	{
		m_lineVisual->append(line.start, line.end, line.color);
	}
	m_lineVisual->setIsVisible(!visualData.lines.empty());
	if (!visualData.lines.empty())
	{
		m_lineVisual->initBuffer();
	}

	while (m_sleeperVisuals.size() < visualData.sleepers.size())
	{
		auto sleeper = new CubePrimitive(1.25f, 0.2f, 0.12f, true);
		sleeper->setIsAccpectOcTtree(false);
		sleeper->setIsHitable(false);
		sleeper->setColor(vec4::fromRGB(120, 84, 48));
		m_visualRoot->addChild(sleeper, false);
		m_sleeperVisuals.push_back(sleeper);
	}

	for (size_t i = 0; i < m_sleeperVisuals.size(); ++i)
	{
		CubePrimitive* sleeper = m_sleeperVisuals[i];
		if (i < visualData.sleepers.size())
		{
			sleeper->setIsVisible(true);
			sleeper->setPos(visualData.sleepers[i].position);
			sleeper->setRotateQ(visualData.sleepers[i].rotation);
		}
		else
		{
			sleeper->setIsVisible(false);
		}
	}

	for (CubePrimitive* nodeCube : m_nodeVisuals)
	{
		if (nodeCube)
		{
			nodeCube->setIsVisible(false);
		}
	}
	for (CubePrimitive* stationCube : m_stationVisuals)
	{
		if (stationCube)
		{
			stationCube->setIsVisible(false);
		}
	}
	for (CubePrimitive* routePointCube : m_routePointVisuals)
	{
		if (routePointCube)
		{
			routePointCube->setIsVisible(false);
		}
	}

	if (m_pendingAnchorVisual)
	{
		m_pendingAnchorVisual->setIsVisible(false);
	}

	m_visualDirty = false;
}

void RailSystem::syncTrackAddVisuals(PlacementMode placementMode)
{
	syncNodeVisuals(true, false, false);
	syncTrackPreview(placementMode, true);
}

void RailSystem::syncTrackDeleteVisuals()
{
	syncNodeVisuals(true, false, false);
	syncTrackPreview(PlacementMode::CursorBased, false);
}

void RailSystem::syncStationVisuals(bool showStations, bool deleteMode, bool linePickMode)
{
	if (!m_visualRoot)
	{
		return;
	}

	while (m_stationVisuals.size() < m_stationManager.stations().size())
	{
		auto stationCube = new CubePrimitive(0.65f, 0.22f, 0.65f, true);
		stationCube->setIsAccpectOcTtree(false);
		stationCube->setIsHitable(false);
		m_visualRoot->addChild(stationCube, false);
		m_stationVisuals.push_back(stationCube);
	}

	for (size_t i = 0; i < m_stationVisuals.size(); ++i)
	{
		CubePrimitive* stationCube = m_stationVisuals[i];
		if (i >= m_stationManager.stations().size() || !showStations)
		{
			stationCube->setIsVisible(false);
			continue;
		}

		const RailStation& station = m_stationManager.stations()[i];
		RailAnchorSample sample = m_anchorManager.sampleAnchor(m_network, m_stationManager.anchorForStation(station.id));
		if (!sample.valid)
		{
			stationCube->setIsVisible(false);
			continue;
		}

		stationCube->setIsVisible(true);
		stationCube->setPos(sample.position + vec3(0.0f, 0.28f, 0.0f));
		stationCube->setScale(vec3(1.0f, 1.0f, 1.0f));
		if (deleteMode)
		{
			stationCube->setColor(vec4::fromRGB(240, 80, 80));
		}
		else if (linePickMode)
		{
			stationCube->setColor(vec4::fromRGB(80, 190, 240));
		}
		else
		{
			stationCube->setColor(vec4::fromRGB(240, 210, 70));
		}
	}

}

void RailSystem::syncRoutePointVisuals(bool showRoutePoints, bool deleteMode, bool linePickMode)
{
	if (!m_visualRoot)
	{
		return;
	}

	while (m_routePointVisuals.size() < m_routePointManager.routePoints().size())
	{
		auto routePointCube = new CubePrimitive(0.34f, 0.34f, 0.34f, true);
		routePointCube->setIsAccpectOcTtree(false);
		routePointCube->setIsHitable(false);
		m_visualRoot->addChild(routePointCube, false);
		m_routePointVisuals.push_back(routePointCube);
	}

	for (size_t i = 0; i < m_routePointVisuals.size(); ++i)
	{
		CubePrimitive* routePointCube = m_routePointVisuals[i];
		if (i >= m_routePointManager.routePoints().size() || !showRoutePoints)
		{
			routePointCube->setIsVisible(false);
			continue;
		}

		const RailRoutePoint& routePoint = m_routePointManager.routePoints()[i];
		RailAnchorSample sample = m_anchorManager.sampleAnchor(m_network, routePoint.anchorId);
		if (!sample.valid)
		{
			routePointCube->setIsVisible(false);
			continue;
		}

		routePointCube->setIsVisible(true);
		routePointCube->setPos(sample.position + vec3(0.0f, 0.26f, 0.0f));
		routePointCube->setScale(vec3(1.0f, 1.0f, 1.0f));
		if (deleteMode)
		{
			routePointCube->setColor(vec4::fromRGB(240, 80, 80));
		}
		else if (linePickMode)
		{
			routePointCube->setColor(vec4::fromRGB(80, 220, 120));
		}
		else
		{
			routePointCube->setColor(vec4::fromRGB(180, 120, 240));
		}
	}

}

void RailSystem::hideEditorVisuals()
{
	syncNodeVisuals(false, false, false);
	syncStationVisuals(true, false, false);
	syncRoutePointVisuals(true, false, false);
	syncTrackPreview(PlacementMode::CursorBased, false);
}

void RailSystem::syncNodeVisuals(bool showNodes, bool lineAddMode, bool lineRemoveMode)
{
	(void)lineRemoveMode;
	if (!m_visualRoot)
	{
		return;
	}

	std::vector<RailNodeVisual> nodeVisualData;
	m_nodeMesh.build(m_network, m_config, nodeVisualData);

	while (m_nodeVisuals.size() < nodeVisualData.size())
	{
		auto nodeCube = new CubePrimitive(0.4f, 0.4f, 0.4f, true);
		nodeCube->setIsAccpectOcTtree(false);
		nodeCube->setIsHitable(false);
		m_visualRoot->addChild(nodeCube, false);
		m_nodeVisuals.push_back(nodeCube);
	}

	const RailNodeId pendingId = m_buildTool.pendingNodeId();
	for (size_t i = 0; i < m_nodeVisuals.size(); ++i)
	{
		CubePrimitive* nodeCube = m_nodeVisuals[i];
		if (i >= nodeVisualData.size() || !showNodes)
		{
			nodeCube->setIsVisible(false);
			continue;
		}

		nodeCube->setIsVisible(true);
		nodeCube->setPos(nodeVisualData[i].position);
		nodeCube->setScale(vec3(1.0f, 1.0f, 1.0f));

		const RailNodeId nodeId = nodeVisualData[i].nodeId;
		if (nodeId == pendingId)
		{
			nodeCube->setColor(vec4::fromRGB(80, 220, 80));
		}
		else if (lineAddMode)
		{
			const RailNode* node = m_network.node(nodeId);
			nodeCube->setColor((node && node->isConnectable) ? vec4::fromRGB(80, 220, 80) : vec4::fromRGB(220, 180, 40));
		}
		else
		{
			nodeCube->setColor(vec4::fromRGB(220, 180, 40));
		}
	}

	if (!m_pendingAnchorVisual)
	{
		m_pendingAnchorVisual = new CubePrimitive(0.44f, 0.44f, 0.44f, true);
		m_pendingAnchorVisual->setIsAccpectOcTtree(false);
		m_pendingAnchorVisual->setIsHitable(false);
		m_pendingAnchorVisual->setColor(vec4::fromRGB(80, 220, 80));
		m_visualRoot->addChild(m_pendingAnchorVisual, false);
	}

	vec3 pendingPosition;
	if (showNodes
		&& m_buildTool.pendingNodeId() == InvalidRailNodeId
		&& m_buildTool.pendingAnchorPosition(pendingPosition))
	{
		m_pendingAnchorVisual->setIsVisible(true);
		m_pendingAnchorVisual->setPos(pendingPosition + vec3(0.0f, 0.12f, 0.0f));
	}
	else
	{
		m_pendingAnchorVisual->setIsVisible(false);
	}
}

void RailSystem::syncTrackPreview(PlacementMode placementMode, bool showTrackPreview)
{
	if (!m_visualRoot)
	{
		return;
	}
	if (!m_trackPreviewVisual)
	{
		m_trackPreviewVisual = new LinePrimitive();
		m_trackPreviewVisual->setIsAccpectOcTtree(false);
		m_trackPreviewVisual->setIsHitable(false);
		m_visualRoot->addChild(m_trackPreviewVisual, false);
	}

	m_trackPreviewVisual->clear();
	if (!showTrackPreview)
	{
		m_trackPreviewVisual->setIsVisible(false);
		for (CubePrimitive* sleeper : m_trackPreviewSleeperVisuals)
		{
			if (sleeper)
			{
				sleeper->setIsVisible(false);
			}
		}
		return;
	}

	RailSegment previewSegment;
	if (!m_buildTool.buildPreviewSegment(placementMode, previewSegment))
	{
		m_trackPreviewVisual->setIsVisible(false);
		for (CubePrimitive* sleeper : m_trackPreviewSleeperVisuals)
		{
			if (sleeper)
			{
				sleeper->setIsVisible(false);
			}
		}
		return;
	}

	RailTrackVisualData previewData;
	m_trackMesh.appendSegmentVisual(previewSegment, m_config, previewData);
	for (const RailLineVisual& line : previewData.lines)
	{
		m_trackPreviewVisual->append(line.start + vec3(0.0f, 0.08f, 0.0f), line.end + vec3(0.0f, 0.08f, 0.0f),
			vec3(0.15f, 0.95f, 0.35f));
	}

	const bool hasLines = m_trackPreviewVisual->getLineCount() > 0;
	m_trackPreviewVisual->setIsVisible(hasLines);
	if (hasLines)
	{
		m_trackPreviewVisual->initBuffer();
	}

	while (m_trackPreviewSleeperVisuals.size() < previewData.sleepers.size())
	{
		auto sleeper = new CubePrimitive(1.25f, 0.2f, 0.12f, true);
		sleeper->setIsAccpectOcTtree(false);
		sleeper->setIsHitable(false);
		sleeper->setColor(vec4(0.15f, 0.95f, 0.35f, 1.0f));
		m_visualRoot->addChild(sleeper, false);
		m_trackPreviewSleeperVisuals.push_back(sleeper);
	}

	for (size_t i = 0; i < m_trackPreviewSleeperVisuals.size(); ++i)
	{
		CubePrimitive* sleeper = m_trackPreviewSleeperVisuals[i];
		if (i < previewData.sleepers.size())
		{
			sleeper->setIsVisible(true);
			sleeper->setPos(previewData.sleepers[i].position + vec3(0.0f, 0.08f, 0.0f));
			sleeper->setRotateQ(previewData.sleepers[i].rotation);
		}
		else
		{
			sleeper->setIsVisible(false);
		}
	}
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

	const bool changed = m_lineManager.addControlPoint(selectedLineId, controlPoint, m_network,
		m_anchorManager, m_stationManager, m_routePointManager);
	if (changed)
	{
		m_previewLineId = selectedLineId;
		m_trainManager.refreshAfterLineRebuild(m_lineManager);
		markVisualDirty();
	}
	return changed;
}

void RailSystem::ensureVisualRoot(Node* sceneRoot)
{
	if (!sceneRoot)
	{
		return;
	}

	if (!m_visualRoot)
	{
		m_visualRoot = new Node();
		m_visualRoot->setName("RailVisualRoot");
	}
	if (!m_visualRoot->getParent())
	{
		sceneRoot->addChild(m_visualRoot, false);
	}
	if (!m_lineVisual)
	{
		m_lineVisual = new LinePrimitive();
		m_lineVisual->setIsAccpectOcTtree(false);
		m_lineVisual->setIsHitable(false);
		m_visualRoot->addChild(m_lineVisual, false);
	}
}

void RailSystem::clearVisuals()
{
	m_lineVisual = nullptr;
	m_sleeperVisuals.clear();
	m_nodeVisuals.clear();
	m_stationVisuals.clear();
	m_routePointVisuals.clear();
	m_trackPreviewSleeperVisuals.clear();
	m_pendingAnchorVisual = nullptr;
	m_trackPreviewVisual = nullptr;
	if (m_visualRoot)
	{
		if (m_visualRoot->getParent())
		{
			m_visualRoot->removeFromParent();
		}
		delete m_visualRoot;
		m_visualRoot = nullptr;
	}
}

} // namespace tzw
