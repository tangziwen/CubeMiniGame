#pragma once

#include "RailBuildTool.h"
#include "RailLine.h"
#include "RailLinePreviewVisual.h"
#include "RailNodeMesh.h"
#include "RailTrain.h"
#include "RailTrainVisual.h"
#include "RailTrackMesh.h"

#include <vector>

namespace tzw {

class CubePrimitive;
class LinePrimitive;
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
	void syncStationVisuals(bool showStations, bool deleteMode, bool linePickMode);
	void syncRoutePointVisuals(bool showRoutePoints, bool deleteMode, bool linePickMode);
	void hideEditorVisuals();
	void setLinePreview(RailLineId lineId);
	void clearLinePreview();

private:
	void rebuildAllRailLines();
	void markVisualDirty();
	bool handleTrackPrimaryClick(PlacementMode placementMode);
	void syncVisuals(Node* sceneRoot);
	void syncNodeVisuals(bool showNodes, bool lineAddMode, bool lineRemoveMode);
	void syncTrackPreview(PlacementMode placementMode, bool showTrackPreview);
	bool createAnchorAtHit(PlacementMode placementMode, RailAnchorId& outAnchorId);
	bool addControlPointToSelectedLine(const RailLineControlPoint& controlPoint);
	void ensureVisualRoot(Node* sceneRoot);
	void clearVisuals();

	RailConfig m_config;
	RailNetwork m_network;
	RailBuildTool m_buildTool;
	RailAnchorManager m_anchorManager;
	RailStationManager m_stationManager;
	RailRoutePointManager m_routePointManager;
	RailLineManager m_lineManager;
	RailTrainManager m_trainManager;
	RailTrainVisualSet m_trainVisuals;
	RailLinePreviewVisual m_linePreviewVisual;
	RailTrackMesh m_trackMesh;
	RailNodeMesh m_nodeMesh;
	Node* m_visualRoot = nullptr;
	LinePrimitive* m_lineVisual = nullptr;
	std::vector<CubePrimitive*> m_sleeperVisuals;
	std::vector<CubePrimitive*> m_nodeVisuals;
	std::vector<CubePrimitive*> m_stationVisuals;
	std::vector<CubePrimitive*> m_routePointVisuals;
	std::vector<CubePrimitive*> m_trackPreviewSleeperVisuals;
	CubePrimitive* m_pendingAnchorVisual = nullptr;
	LinePrimitive* m_trackPreviewVisual = nullptr;
	RailLineId m_previewLineId = InvalidRailLineId;
	bool m_visualDirty = true;
};

} // namespace tzw
