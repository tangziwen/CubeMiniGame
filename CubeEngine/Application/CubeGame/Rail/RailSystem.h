#pragma once

#include "RailBuildTool.h"
#include "RailEditorVisualSystem.h"
#include "RailInspectTarget.h"
#include "RailLine.h"
#include "RailPersistentVisualSystem.h"
#include "RailTrain.h"
#include "EngineSrc/Math/vec4.h"
#include "rapidjson/document.h"

#include <string>

namespace tzw {

class Node;

enum class RailEditResult
{
	Success,
	InvalidOperation,
};

class RailSystem
{
public:
	RailSystem();
	~RailSystem();

	void init();
	void update(Node* sceneRoot, float deltaSeconds);
	void clear();
	void serialize(rapidjson::Value& railData, rapidjson::Document::AllocatorType& allocator) const;
	bool unserialize(const rapidjson::Value& railData);

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
	RailEditResult handleStationAddPrimaryClick(PlacementMode placementMode, const std::string& name = "");
	bool handleStationDeletePrimaryClick(PlacementMode placementMode);
	RailEditResult handleRoutePointAddPrimaryClick(PlacementMode placementMode, const std::string& name = "");
	bool handleRoutePointDeletePrimaryClick(PlacementMode placementMode);
	bool renameStation(RailStationId stationId, const std::string& name);
	bool renameRoutePoint(RailRoutePointId routePointId, const std::string& name);
	bool pickInspectTarget(PlacementMode placementMode, RailInspectTarget& outTarget) const;
	void setTrainOutline(RailTrainId trainId, bool enabled, vec4 color);
	void setStationOutline(RailStationId stationId, bool enabled, vec4 color);
	void setRoutePointOutline(RailRoutePointId routePointId, bool enabled, vec4 color);
	void clearOutlines();
	bool addStationToSelectedLine(RailStationId stationId);
	bool addRoutePointToSelectedLine(RailRoutePointId routePointId);
	RailEditResult addPickedControlPointToSelectedLine(PlacementMode placementMode);
	bool addPickedStationToSelectedLine(PlacementMode placementMode);
	bool addPickedRoutePointToSelectedLine(PlacementMode placementMode);
	void syncTrackAddVisuals(PlacementMode placementMode);
	void syncTrackDeleteVisuals();
	void showStationEditorVisuals(bool deleteMode);
	void showRoutePointEditorVisuals(bool deleteMode);
	void showLineAddControlBillboards();
	void showLineRemoveControlBillboards();
	void hideEditorVisuals();
	void hideAllEditorVisuals();
	void setLinePreview(RailLineId lineId);
	void clearLinePreview();

private:
	void rebuildAllRailLines();
	void markVisualDirty();
	bool handleTrackPrimaryClick(PlacementMode placementMode);
	bool deletePointsAnchoredToSegment(RailSegmentId segmentId);
	bool isAnchorOnSegment(RailAnchorId anchorId, RailSegmentId segmentId) const;
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
