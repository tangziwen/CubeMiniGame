#include "RailSystem.h"

#include "CubeGame/BuildingSystem.h"

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
	if (m_previewLineId != InvalidRailLineId)
	{
		m_editorVisuals.showLinePreview(sceneRoot, m_network, m_lineManager.line(m_previewLineId));
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
	m_editorVisuals.showLineAddControlBillboards(m_network, m_anchorManager, m_stationManager, m_routePointManager,
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
