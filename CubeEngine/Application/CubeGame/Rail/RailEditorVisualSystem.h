#ifndef TZW_RAIL_EDITOR_VISUAL_SYSTEM_H
#define TZW_RAIL_EDITOR_VISUAL_SYSTEM_H

#include "RailBuildTool.h"
#include "RailLine.h"
#include "RailLinePreviewVisual.h"
#include "RailNodeMesh.h"
#include "RailPoint.h"
#include "RailTrackMesh.h"
#include "2D/WorldBillboardSprite.h"

#include <functional>
#include <vector>

namespace tzw {

class CubePrimitive;
class LinePrimitive;
class Node;

class RailStationDeleteBillboard
{
public:
	explicit RailStationDeleteBillboard(RailStationId stationId);

	RailStationId stationId() const;
	void beginSync();
	void sync(Node* visualRoot, const RailNetwork& network, const RailAnchorManager& anchorManager,
		const RailStationManager& stationManager, const RailStation& station,
		const std::function<void(RailStationId)>& onClicked);
	void hide();
	bool isUsedThisFrame() const;

private:
	void ensureSprite(Node* visualRoot);

	RailStationId m_stationId = InvalidRailStationId;
	WorldBillboardSprite* m_sprite = nullptr;
	bool m_isUsedThisFrame = false;
};

class RailRoutePointDeleteBillboard
{
public:
	explicit RailRoutePointDeleteBillboard(RailRoutePointId routePointId);

	RailRoutePointId routePointId() const;
	void beginSync();
	void sync(Node* visualRoot, const RailNetwork& network, const RailAnchorManager& anchorManager,
		const RailRoutePoint& routePoint, const std::function<void(RailRoutePointId)>& onClicked);
	void hide();
	bool isUsedThisFrame() const;

private:
	void ensureSprite(Node* visualRoot);

	RailRoutePointId m_routePointId = InvalidRailRoutePointId;
	WorldBillboardSprite* m_sprite = nullptr;
	bool m_isUsedThisFrame = false;
};

class RailLineAddStationBillboard
{
public:
	explicit RailLineAddStationBillboard(RailStationId stationId);

	RailStationId stationId() const;
	void beginSync();
	void sync(Node* visualRoot, const RailNetwork& network, const RailAnchorManager& anchorManager,
		const RailStationManager& stationManager, const RailStation& station,
		const std::function<void(RailStationId)>& onClicked);
	void hide();
	bool isUsedThisFrame() const;

private:
	void ensureSprite(Node* visualRoot);

	RailStationId m_stationId = InvalidRailStationId;
	WorldBillboardSprite* m_sprite = nullptr;
	bool m_isUsedThisFrame = false;
};

class RailLineAddRoutePointBillboard
{
public:
	explicit RailLineAddRoutePointBillboard(RailRoutePointId routePointId);

	RailRoutePointId routePointId() const;
	void beginSync();
	void sync(Node* visualRoot, const RailNetwork& network, const RailAnchorManager& anchorManager,
		const RailRoutePoint& routePoint, const std::function<void(RailRoutePointId)>& onClicked);
	void hide();
	bool isUsedThisFrame() const;

private:
	void ensureSprite(Node* visualRoot);

	RailRoutePointId m_routePointId = InvalidRailRoutePointId;
	WorldBillboardSprite* m_sprite = nullptr;
	bool m_isUsedThisFrame = false;
};

class RailLineRemoveControlPointBillboard
{
public:
	void beginSync();
	void sync(Node* visualRoot, int controlPointIndex, const RailLineControlPoint& controlPoint,
		const RailNetwork& network, const RailAnchorManager& anchorManager,
		const RailStationManager& stationManager, const RailRoutePointManager& routePointManager,
		const std::function<void(int)>& onClicked);
	void hide();
	bool isUsedThisFrame() const;

private:
	void ensureSprite(Node* visualRoot);
	bool resolvePosition(const RailLineControlPoint& controlPoint, const RailNetwork& network,
		const RailAnchorManager& anchorManager, const RailStationManager& stationManager,
		const RailRoutePointManager& routePointManager, vec3& outPosition) const;

	WorldBillboardSprite* m_sprite = nullptr;
	bool m_isUsedThisFrame = false;
};

class RailEditorVisualSystem
{
public:
	void prepare(Node* sceneRoot);
	void clear();

	void showTrackAddPreview(const RailNetwork& network, const RailBuildTool& buildTool,
		const RailConfig& config, PlacementMode placementMode);
	void showTrackDeleteHints(const RailNetwork& network, const RailConfig& config);
	void showStationBillboards(const RailNetwork& network, const RailAnchorManager& anchorManager,
		const RailStationManager& stationManager, bool deleteMode,
		const std::function<void(RailStationId)>& onClicked);
	void showRoutePointBillboards(const RailNetwork& network, const RailAnchorManager& anchorManager,
		const RailRoutePointManager& routePointManager, bool deleteMode,
		const std::function<void(RailRoutePointId)>& onClicked);
	void showLineAddControlBillboards(const RailNetwork& network, const RailAnchorManager& anchorManager,
		const RailStationManager& stationManager, const RailRoutePointManager& routePointManager,
		const std::function<void(RailStationId)>& onStationClicked,
		const std::function<void(RailRoutePointId)>& onRoutePointClicked);
	void showLineRemoveControlBillboards(const RailNetwork& network, const RailAnchorManager& anchorManager,
		const RailStationManager& stationManager, const RailRoutePointManager& routePointManager,
		const RailLine* line, const std::function<void(int)>& onControlPointClicked);
	void hideInteractionVisuals();
	void showLinePreview(Node* sceneRoot, const RailNetwork& network, const RailLine* line);
	void clearLinePreview();
	void hideAll();

private:
	void ensureVisualRoot(Node* sceneRoot);
	void syncNodeVisuals(const RailNetwork& network, const RailBuildTool& buildTool,
		const RailConfig& config, bool showNodes, bool lineAddMode);
	void syncTrackPreview(const RailBuildTool& buildTool, const RailConfig& config,
		PlacementMode placementMode, bool showTrackPreview);
	void hideTrackPreview();
	void hideNodeVisuals();
	RailStationDeleteBillboard& ensureStationDeleteBillboard(RailStationId stationId);
	RailRoutePointDeleteBillboard& ensureRoutePointDeleteBillboard(RailRoutePointId routePointId);
	RailLineAddStationBillboard& ensureLineAddStationBillboard(RailStationId stationId);
	RailLineAddRoutePointBillboard& ensureLineAddRoutePointBillboard(RailRoutePointId routePointId);
	void beginStationDeleteBillboardSync();
	void beginRoutePointDeleteBillboardSync();
	void beginLineAddBillboardSync();
	void beginLineRemoveBillboardSync();
	void hideBillboards();
	void hideStationDeleteBillboards();
	void hideRoutePointDeleteBillboards();
	void hideLineAddBillboards();
	void hideLineRemoveControlPointBillboards();
	void hideUnusedStationDeleteBillboards(const RailStationManager& stationManager);
	void hideUnusedRoutePointDeleteBillboards(const RailRoutePointManager& routePointManager);
	void hideUnusedLineAddBillboards(const RailStationManager& stationManager,
		const RailRoutePointManager& routePointManager);
	void hideUnusedLineRemoveControlPointBillboards();

	RailNodeMesh m_nodeMesh;
	RailTrackMesh m_trackMesh;
	RailLinePreviewVisual m_linePreviewVisual;
	Node* m_visualRoot = nullptr;
	std::vector<CubePrimitive*> m_nodeVisuals;
	std::vector<CubePrimitive*> m_trackPreviewSleeperVisuals;
	CubePrimitive* m_pendingAnchorVisual = nullptr;
	LinePrimitive* m_trackPreviewVisual = nullptr;
	std::vector<RailStationDeleteBillboard> m_stationDeleteBillboards;
	std::vector<RailRoutePointDeleteBillboard> m_routePointDeleteBillboards;
	std::vector<RailLineAddStationBillboard> m_lineAddStationBillboards;
	std::vector<RailLineAddRoutePointBillboard> m_lineAddRoutePointBillboards;
	std::vector<RailLineRemoveControlPointBillboard> m_lineRemoveControlPointBillboards;
};

} // namespace tzw

#endif // TZW_RAIL_EDITOR_VISUAL_SYSTEM_H
