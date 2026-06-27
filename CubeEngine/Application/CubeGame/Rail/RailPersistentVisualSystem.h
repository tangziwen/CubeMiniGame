#ifndef TZW_RAIL_PERSISTENT_VISUAL_SYSTEM_H
#define TZW_RAIL_PERSISTENT_VISUAL_SYSTEM_H

#include "RailPoint.h"
#include "RailPointNameBillboard.h"
#include "RailTrackMesh.h"
#include "RailTrainVisual.h"
#include "EngineSrc/Math/vec3.h"
#include "EngineSrc/Scene/ScenePickingSystem.h"

#include <vector>

namespace tzw {

class LinePrimitive;
class Node;
class CubePrimitive;

class RailStationVisual
{
public:
	explicit RailStationVisual(RailStationId stationId);

	RailStationId stationId() const;
	void sync(Node* visualRoot, Node* uiRoot, const RailNetwork& network, const RailAnchorManager& anchorManager,
		const RailStationManager& stationManager, const RailStation& station);
	void hide();
	void clearPickTarget();
	void setOutline(bool enabled, vec4 color);

private:
	void ensureMarker(Node* visualRoot);
	void syncPickTarget(const vec3& center);

	RailStationId m_stationId = InvalidRailStationId;
	CubePrimitive* m_marker = nullptr;
	PickHandle m_pickHandle;
	RailPointNameBillboard m_nameBillboard;
	bool m_outlineEnabled = false;
	vec4 m_outlineColor = vec4(1.0f, 0.85f, 0.15f, 1.0f);
};

class RailRoutePointVisual
{
public:
	explicit RailRoutePointVisual(RailRoutePointId routePointId);

	RailRoutePointId routePointId() const;
	void sync(Node* visualRoot, Node* uiRoot, const RailNetwork& network, const RailAnchorManager& anchorManager,
		const RailRoutePoint& routePoint);
	void hide();
	void clearPickTarget();
	void setOutline(bool enabled, vec4 color);

private:
	void ensureMarker(Node* visualRoot);
	void syncPickTarget(const vec3& center);

	RailRoutePointId m_routePointId = InvalidRailRoutePointId;
	CubePrimitive* m_marker = nullptr;
	PickHandle m_pickHandle;
	RailPointNameBillboard m_nameBillboard;
	bool m_outlineEnabled = false;
	vec4 m_outlineColor = vec4(1.0f, 0.85f, 0.15f, 1.0f);
};

class RailTrackVisualSet
{
public:
	void sync(Node* sceneRoot, const RailNetwork& network, const RailConfig& config, bool forceRebuild);
	void clear();

private:
	void ensureVisualRoot(Node* sceneRoot);

	RailTrackMesh m_trackMesh;
	Node* m_visualRoot = nullptr;
	LinePrimitive* m_lineVisual = nullptr;
	std::vector<CubePrimitive*> m_sleeperVisuals;
};

class RailPointVisualSet
{
public:
	void sync(Node* sceneRoot, const RailNetwork& network, const RailAnchorManager& anchorManager,
		const RailStationManager& stationManager, const RailRoutePointManager& routePointManager);
	void clear();
	void setStationOutline(RailStationId stationId, bool enabled, vec4 color);
	void setRoutePointOutline(RailRoutePointId routePointId, bool enabled, vec4 color);
	void clearOutlines();

private:
	void ensureVisualRoot(Node* sceneRoot);
	void ensureUiRoot();
	RailStationVisual& ensureStationVisual(RailStationId stationId);
	RailRoutePointVisual& ensureRoutePointVisual(RailRoutePointId routePointId);
	void hideUnused(const RailStationManager& stationManager, const RailRoutePointManager& routePointManager);
	bool containsStation(const RailStationManager& stationManager, RailStationId stationId) const;
	bool containsRoutePoint(const RailRoutePointManager& routePointManager, RailRoutePointId routePointId) const;

	Node* m_visualRoot = nullptr;
	Node* m_uiRoot = nullptr;
	std::vector<RailStationVisual> m_stationVisuals;
	std::vector<RailRoutePointVisual> m_routePointVisuals;
};

class RailPersistentVisualSystem
{
public:
	void sync(Node* sceneRoot, const RailNetwork& network, const RailConfig& config,
		const RailAnchorManager& anchorManager, const RailStationManager& stationManager,
		const RailRoutePointManager& routePointManager, const RailLineManager& lineManager,
		const RailTrainManager& trainManager);
	void markDirty();
	void clear();
	void setTrainOutline(RailTrainId trainId, bool enabled, vec4 color);
	void setStationOutline(RailStationId stationId, bool enabled, vec4 color);
	void setRoutePointOutline(RailRoutePointId routePointId, bool enabled, vec4 color);
	void clearOutlines();

private:
	RailTrackVisualSet m_trackVisuals;
	RailPointVisualSet m_pointVisuals;
	RailTrainVisualSet m_trainVisuals;
	bool m_trackDirty = true;
};

} // namespace tzw

#endif // TZW_RAIL_PERSISTENT_VISUAL_SYSTEM_H
