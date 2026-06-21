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
	bool addPickedNodeToSelectedLine(PlacementMode placementMode);
	bool removePickedNodeFromSelectedLine(PlacementMode placementMode);
	void syncTrackAddVisuals(PlacementMode placementMode);
	void syncTrackDeleteVisuals();
	void syncLineAddNodeVisuals();
	void syncLineRemoveNodeVisuals();
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
	void ensureVisualRoot(Node* sceneRoot);
	void clearVisuals();

	RailConfig m_config;
	RailNetwork m_network;
	RailBuildTool m_buildTool;
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
	std::vector<CubePrimitive*> m_trackPreviewSleeperVisuals;
	CubePrimitive* m_pendingAnchorVisual = nullptr;
	LinePrimitive* m_trackPreviewVisual = nullptr;
	RailLineId m_previewLineId = InvalidRailLineId;
	bool m_visualDirty = true;
};

} // namespace tzw
