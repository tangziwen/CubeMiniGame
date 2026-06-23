#pragma once

#include "RailBuildTool.h"
#include "RailEditorVisualSystem.h"
#include "RailLine.h"
#include "RailPersistentVisualSystem.h"
#include "RailTrain.h"

namespace tzw {

class Node;

class RailSystem
{
public:
	RailSystem();
	~RailSystem();

	void init();
	void update(Node* sceneRoot, float deltaSeconds);
	void clear();

	bool handleTrackAddPrimaryClick(PlacementMode placementMode);
	bool handleTrackDeletePrimaryClick(PlacementMode placementMode);
	void cancelTrackEdit();

	RailNetwork& network();
	const RailNetwork& network() const;
	RailLineManager& lineManager();
	const RailLineManager& lineManager() const;
	RailTrainManager& trainManager();
	const RailTrainManager& trainManager() const;
	RailAnchorManager& anchorManager();
	const RailAnchorManager& anchorManager() const;
	RailStationManager& stationManager();
	const RailStationManager& stationManager() const;
	RailRoutePointManager& routePointManager();
	const RailRoutePointManager& routePointManager() const;
	bool handleStationAddPrimaryClick(PlacementMode placementMode);
	bool handleStationDeletePrimaryClick(PlacementMode placementMode);
	bool handleRoutePointAddPrimaryClick(PlacementMode placementMode);
	bool handleRoutePointDeletePrimaryClick(PlacementMode placementMode);
	bool addStationToSelectedLine(RailStationId stationId);
	bool addRoutePointToSelectedLine(RailRoutePointId routePointId);
	bool addPickedControlPointToSelectedLine(PlacementMode placementMode);
	bool addPickedStationToSelectedLine(PlacementMode placementMode);
	bool addPickedRoutePointToSelectedLine(PlacementMode placementMode);
	void syncTrackAddVisuals(PlacementMode placementMode);
	void syncTrackDeleteVisuals();
	void showStationEditorVisuals(bool deleteMode);
	void showRoutePointEditorVisuals(bool deleteMode);
	void showLineAddControlBillboards();
	void showLineRemoveControlBillboards();
	void hideEditorVisuals();
	void setLinePreview(RailLineId lineId);
	void clearLinePreview();

private:
	void rebuildAllRailLines();
	void markVisualDirty();
	bool handleTrackPrimaryClick(PlacementMode placementMode);
	bool createAnchorAtHit(PlacementMode placementMode, RailAnchorId& outAnchorId);
	bool addControlPointToSelectedLine(const RailLineControlPoint& controlPoint);

	RailConfig m_config;
	RailNetwork m_network;
	RailBuildTool m_buildTool;
	RailAnchorManager m_anchorManager;
	RailStationManager m_stationManager;
	RailRoutePointManager m_routePointManager;
	RailLineManager m_lineManager;
	RailTrainManager m_trainManager;
	RailPersistentVisualSystem m_persistentVisuals;
	RailEditorVisualSystem m_editorVisuals;
	RailLineId m_previewLineId = InvalidRailLineId;
};

} // namespace tzw
