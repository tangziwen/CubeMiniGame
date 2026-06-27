#include "RailPersistentVisualSystem.h"

#include "CubeGame/Rail/RailInspectTarget.h"
#include "EngineSrc/3D/Primitive/CubePrimitive.h"
#include "EngineSrc/2D/RetainedUISystem.h"
#include "EngineSrc/3D/Primitive/LinePrimitive.h"
#include "EngineSrc/Base/Node.h"
#include "EngineSrc/Math/vec4.h"

#include <memory>

namespace tzw {

namespace
{
constexpr int PersistentTrackVisualRootPriority = 10;
constexpr int PersistentPointVisualRootPriority = 20;
constexpr int StationPickPriority = 20;
constexpr int RoutePointPickPriority = 10;
const vec3 StationPickHalfExtents(0.48f, 0.52f, 0.48f);
const vec3 RoutePointPickHalfExtents(0.32f, 0.38f, 0.32f);
const char* PersistentUiLayerName = "default";
const char* PersistentUiRootName = "RailPersistentVisualRoot";
constexpr int PersistentUiLayerPriority = 0;

AABB makeCenteredBounds(const vec3& halfExtents)
{
	AABB bounds;
	bounds.update(-halfExtents);
	bounds.update(halfExtents);
	return bounds;
}

std::unique_ptr<PickShape> makeTranslatedBoxPickShape(const vec3& center, const vec3& halfExtents)
{
	Matrix44 transform;
	transform.setTranslate(center);
	return std::make_unique<BoxPickShape>(makeCenteredBounds(halfExtents), transform);
}

PickPayload railPickPayload(RailInspectTargetKind kind, int id)
{
	PickPayload payload;
	payload.type = static_cast<int>(kind);
	payload.id0 = id;
	return payload;
}
}

RailStationVisual::RailStationVisual(RailStationId stationId)
	: m_stationId(stationId)
{
}

RailStationId RailStationVisual::stationId() const
{
	return m_stationId;
}

void RailStationVisual::sync(Node* visualRoot, Node* uiRoot, const RailNetwork& network,
	const RailAnchorManager& anchorManager, const RailStationManager& stationManager, const RailStation& station)
{
	ensureMarker(visualRoot);
	if (!m_marker)
	{
		return;
	}

	RailAnchorSample sample = anchorManager.sampleAnchor(network, stationManager.anchorForStation(station.id));
	if (!sample.valid)
	{
		hide();
		return;
	}
	m_marker->setIsVisible(true);
	m_marker->setOutlineEnabled(m_outlineEnabled);
	m_marker->setOutlineColor(m_outlineColor);
	m_marker->setPos(sample.position + vec3(0.0f, 0.34f, 0.0f));
	m_nameBillboard.sync(uiRoot, sample.position + vec3(0.0f, 1.05f, 0.0f), station.name,
		vec4::fromRGB(80, 70, 35, 225));
	syncPickTarget(sample.position + vec3(0.0f, 0.34f, 0.0f));
}

void RailStationVisual::hide()
{
	if (m_marker)
	{
		m_marker->setIsVisible(false);
	}
	m_nameBillboard.hide();
	if (m_pickHandle.isValid())
	{
		ScenePickingSystem::shared()->setTargetEnabled(m_pickHandle, false);
	}
}

void RailStationVisual::clearPickTarget()
{
	if (!m_pickHandle.isValid())
	{
		return;
	}
	ScenePickingSystem::shared()->unregisterTarget(m_pickHandle);
	m_pickHandle = PickHandle();
}

void RailStationVisual::setOutline(bool enabled, vec4 color)
{
	m_outlineEnabled = enabled;
	m_outlineColor = color;
	if (m_marker)
	{
		m_marker->setOutlineEnabled(enabled);
		m_marker->setOutlineColor(color);
	}
}

void RailStationVisual::ensureMarker(Node* visualRoot)
{
	if (m_marker || !visualRoot)
	{
		return;
	}
	m_marker = new CubePrimitive(0.65f, 0.22f, 0.65f, true);
	m_marker->setIsAccpectOcTtree(false);
	m_marker->setIsHitable(false);
	m_marker->setColor(vec4::fromRGB(240, 210, 70));
	m_marker->setOutlineEnabled(m_outlineEnabled);
	m_marker->setOutlineColor(m_outlineColor);
	visualRoot->addChild(m_marker, false);
}

void RailStationVisual::syncPickTarget(const vec3& center)
{
	std::unique_ptr<PickShape> shape = makeTranslatedBoxPickShape(center, StationPickHalfExtents);
	if (!m_pickHandle.isValid())
	{
		PickTargetDesc desc;
		desc.category = RailInspectPickCategory;
		desc.priority = StationPickPriority;
		desc.owner = this;
		desc.payload = railPickPayload(RailInspectTargetKind::Station, m_stationId);
		desc.enabled = true;
		desc.shape = std::move(shape);
		m_pickHandle = ScenePickingSystem::shared()->registerTarget(std::move(desc));
		return;
	}

	ScenePickingSystem::shared()->updateTargetShape(m_pickHandle, std::move(shape));
	ScenePickingSystem::shared()->setTargetEnabled(m_pickHandle, true);
}

RailRoutePointVisual::RailRoutePointVisual(RailRoutePointId routePointId)
	: m_routePointId(routePointId)
{
}

RailRoutePointId RailRoutePointVisual::routePointId() const
{
	return m_routePointId;
}

void RailRoutePointVisual::sync(Node* visualRoot, Node* uiRoot, const RailNetwork& network,
	const RailAnchorManager& anchorManager, const RailRoutePoint& routePoint)
{
	ensureMarker(visualRoot);
	if (!m_marker)
	{
		return;
	}

	RailAnchorSample sample = anchorManager.sampleAnchor(network, routePoint.anchorId);
	if (!sample.valid)
	{
		hide();
		return;
	}
	m_marker->setIsVisible(true);
	m_marker->setOutlineEnabled(m_outlineEnabled);
	m_marker->setOutlineColor(m_outlineColor);
	m_marker->setPos(sample.position + vec3(0.0f, 0.28f, 0.0f));
	m_nameBillboard.sync(uiRoot, sample.position + vec3(0.0f, 0.88f, 0.0f), routePoint.name,
		vec4::fromRGB(55, 45, 75, 225));
	syncPickTarget(sample.position + vec3(0.0f, 0.28f, 0.0f));
}

void RailRoutePointVisual::hide()
{
	if (m_marker)
	{
		m_marker->setIsVisible(false);
	}
	m_nameBillboard.hide();
	if (m_pickHandle.isValid())
	{
		ScenePickingSystem::shared()->setTargetEnabled(m_pickHandle, false);
	}
}

void RailRoutePointVisual::clearPickTarget()
{
	if (!m_pickHandle.isValid())
	{
		return;
	}
	ScenePickingSystem::shared()->unregisterTarget(m_pickHandle);
	m_pickHandle = PickHandle();
}

void RailRoutePointVisual::setOutline(bool enabled, vec4 color)
{
	m_outlineEnabled = enabled;
	m_outlineColor = color;
	if (m_marker)
	{
		m_marker->setOutlineEnabled(enabled);
		m_marker->setOutlineColor(color);
	}
}

void RailRoutePointVisual::ensureMarker(Node* visualRoot)
{
	if (m_marker || !visualRoot)
	{
		return;
	}
	m_marker = new CubePrimitive(0.34f, 0.34f, 0.34f, true);
	m_marker->setIsAccpectOcTtree(false);
	m_marker->setIsHitable(false);
	m_marker->setColor(vec4::fromRGB(180, 120, 240));
	m_marker->setOutlineEnabled(m_outlineEnabled);
	m_marker->setOutlineColor(m_outlineColor);
	visualRoot->addChild(m_marker, false);
}

void RailRoutePointVisual::syncPickTarget(const vec3& center)
{
	std::unique_ptr<PickShape> shape = makeTranslatedBoxPickShape(center, RoutePointPickHalfExtents);
	if (!m_pickHandle.isValid())
	{
		PickTargetDesc desc;
		desc.category = RailInspectPickCategory;
		desc.priority = RoutePointPickPriority;
		desc.owner = this;
		desc.payload = railPickPayload(RailInspectTargetKind::RoutePoint, m_routePointId);
		desc.enabled = true;
		desc.shape = std::move(shape);
		m_pickHandle = ScenePickingSystem::shared()->registerTarget(std::move(desc));
		return;
	}

	ScenePickingSystem::shared()->updateTargetShape(m_pickHandle, std::move(shape));
	ScenePickingSystem::shared()->setTargetEnabled(m_pickHandle, true);
}

void RailTrackVisualSet::sync(Node* sceneRoot, const RailNetwork& network, const RailConfig& config, bool forceRebuild)
{
	if (!sceneRoot)
	{
		return;
	}
	ensureVisualRoot(sceneRoot);
	if (!forceRebuild || !m_visualRoot || !m_lineVisual)
	{
		return;
	}

	RailTrackVisualData visualData;
	m_trackMesh.build(network, config, visualData);

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
}

void RailTrackVisualSet::clear()
{
	m_lineVisual = nullptr;
	m_sleeperVisuals.clear();
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

void RailTrackVisualSet::ensureVisualRoot(Node* sceneRoot)
{
	if (!m_visualRoot)
	{
		m_visualRoot = new Node();
		m_visualRoot->setName("RailPersistentTrackVisualRoot");
		m_visualRoot->setLocalPriority(PersistentTrackVisualRootPriority);
	}
	if (!m_visualRoot->getParent())
	{
		sceneRoot->addChild(m_visualRoot);
	}
	if (!m_lineVisual)
	{
		m_lineVisual = new LinePrimitive();
		m_lineVisual->setIsAccpectOcTtree(false);
		m_lineVisual->setIsHitable(false);
		m_visualRoot->addChild(m_lineVisual, false);
	}
}

void RailPointVisualSet::sync(Node* sceneRoot, const RailNetwork& network, const RailAnchorManager& anchorManager,
	const RailStationManager& stationManager, const RailRoutePointManager& routePointManager)
{
	if (!sceneRoot)
	{
		return;
	}
	ensureVisualRoot(sceneRoot);
	ensureUiRoot();
	hideUnused(stationManager, routePointManager);

	for (const RailStation& station : stationManager.stations())
	{
		RailStationVisual& visual = ensureStationVisual(station.id);
		visual.sync(m_visualRoot, m_uiRoot, network, anchorManager, stationManager, station);
	}

	for (const RailRoutePoint& routePoint : routePointManager.routePoints())
	{
		RailRoutePointVisual& visual = ensureRoutePointVisual(routePoint.id);
		visual.sync(m_visualRoot, m_uiRoot, network, anchorManager, routePoint);
	}
}

void RailPointVisualSet::clear()
{
	for (RailStationVisual& visual : m_stationVisuals)
	{
		visual.clearPickTarget();
	}
	for (RailRoutePointVisual& visual : m_routePointVisuals)
	{
		visual.clearPickTarget();
	}
	m_stationVisuals.clear();
	m_routePointVisuals.clear();
	if (m_visualRoot)
	{
		if (m_visualRoot->getParent())
		{
			m_visualRoot->removeFromParent();
		}
		delete m_visualRoot;
		m_visualRoot = nullptr;
	}
	if (m_uiRoot)
	{
		if (m_uiRoot->getParent())
		{
			m_uiRoot->removeFromParent();
		}
		delete m_uiRoot;
		m_uiRoot = nullptr;
	}
}

void RailPointVisualSet::setStationOutline(RailStationId stationId, bool enabled, vec4 color)
{
	if (stationId == InvalidRailStationId)
	{
		return;
	}
	if (!enabled)
	{
		for (RailStationVisual& visual : m_stationVisuals)
		{
			if (visual.stationId() == stationId)
			{
				visual.setOutline(false, color);
				return;
			}
		}
		return;
	}
	ensureStationVisual(stationId).setOutline(enabled, color);
}

void RailPointVisualSet::setRoutePointOutline(RailRoutePointId routePointId, bool enabled, vec4 color)
{
	if (routePointId == InvalidRailRoutePointId)
	{
		return;
	}
	if (!enabled)
	{
		for (RailRoutePointVisual& visual : m_routePointVisuals)
		{
			if (visual.routePointId() == routePointId)
			{
				visual.setOutline(false, color);
				return;
			}
		}
		return;
	}
	ensureRoutePointVisual(routePointId).setOutline(enabled, color);
}

void RailPointVisualSet::clearOutlines()
{
	for (RailStationVisual& visual : m_stationVisuals)
	{
		visual.setOutline(false, vec4(1.0f, 0.85f, 0.15f, 1.0f));
	}
	for (RailRoutePointVisual& visual : m_routePointVisuals)
	{
		visual.setOutline(false, vec4(1.0f, 0.85f, 0.15f, 1.0f));
	}
}

void RailPointVisualSet::ensureVisualRoot(Node* sceneRoot)
{
	if (!m_visualRoot)
	{
		m_visualRoot = new Node();
		m_visualRoot->setName("RailPersistentPointVisualRoot");
		m_visualRoot->setLocalPriority(PersistentPointVisualRootPriority);
	}
	if (!m_visualRoot->getParent())
	{
		sceneRoot->addChild(m_visualRoot);
	}
}

void RailPointVisualSet::ensureUiRoot()
{
	if (!m_uiRoot)
	{
		m_uiRoot = new Node();
		m_uiRoot->setName(PersistentUiRootName);
	}
	if (!m_uiRoot->getParent())
	{
		RetainedUISystem::shared()->addToLayer(PersistentUiLayerName, PersistentUiLayerPriority, m_uiRoot);
	}
}

RailStationVisual& RailPointVisualSet::ensureStationVisual(RailStationId stationId)
{
	for (RailStationVisual& visual : m_stationVisuals)
	{
		if (visual.stationId() == stationId)
		{
			return visual;
		}
	}
	m_stationVisuals.push_back(RailStationVisual(stationId));
	return m_stationVisuals.back();
}

RailRoutePointVisual& RailPointVisualSet::ensureRoutePointVisual(RailRoutePointId routePointId)
{
	for (RailRoutePointVisual& visual : m_routePointVisuals)
	{
		if (visual.routePointId() == routePointId)
		{
			return visual;
		}
	}
	m_routePointVisuals.push_back(RailRoutePointVisual(routePointId));
	return m_routePointVisuals.back();
}

void RailPointVisualSet::hideUnused(const RailStationManager& stationManager,
	const RailRoutePointManager& routePointManager)
{
	for (RailStationVisual& visual : m_stationVisuals)
	{
		if (!containsStation(stationManager, visual.stationId()))
		{
			visual.hide();
			visual.clearPickTarget();
		}
	}
	for (RailRoutePointVisual& visual : m_routePointVisuals)
	{
		if (!containsRoutePoint(routePointManager, visual.routePointId()))
		{
			visual.hide();
			visual.clearPickTarget();
		}
	}
}

bool RailPointVisualSet::containsStation(const RailStationManager& stationManager, RailStationId stationId) const
{
	for (const RailStation& station : stationManager.stations())
	{
		if (station.id == stationId)
		{
			return true;
		}
	}
	return false;
}

bool RailPointVisualSet::containsRoutePoint(const RailRoutePointManager& routePointManager,
	RailRoutePointId routePointId) const
{
	for (const RailRoutePoint& routePoint : routePointManager.routePoints())
	{
		if (routePoint.id == routePointId)
		{
			return true;
		}
	}
	return false;
}

void RailPersistentVisualSystem::sync(Node* sceneRoot, const RailNetwork& network, const RailConfig& config,
	const RailAnchorManager& anchorManager, const RailStationManager& stationManager,
	const RailRoutePointManager& routePointManager, const RailLineManager& lineManager,
	const RailTrainManager& trainManager)
{
	if (!sceneRoot)
	{
		return;
	}
	m_trackVisuals.sync(sceneRoot, network, config, m_trackDirty);
	m_trackDirty = false;
	m_pointVisuals.sync(sceneRoot, network, anchorManager, stationManager, routePointManager);
	m_trainVisuals.sync(sceneRoot, network, lineManager, trainManager);
}

void RailPersistentVisualSystem::markDirty()
{
	m_trackDirty = true;
}

void RailPersistentVisualSystem::clear()
{
	m_trackVisuals.clear();
	m_pointVisuals.clear();
	m_trainVisuals.clear();
	m_trackDirty = true;
}

void RailPersistentVisualSystem::setTrainOutline(RailTrainId trainId, bool enabled, vec4 color)
{
	m_trainVisuals.setTrainOutline(trainId, enabled, color);
}

void RailPersistentVisualSystem::setStationOutline(RailStationId stationId, bool enabled, vec4 color)
{
	m_pointVisuals.setStationOutline(stationId, enabled, color);
}

void RailPersistentVisualSystem::setRoutePointOutline(RailRoutePointId routePointId, bool enabled, vec4 color)
{
	m_pointVisuals.setRoutePointOutline(routePointId, enabled, color);
}

void RailPersistentVisualSystem::clearOutlines()
{
	m_pointVisuals.clearOutlines();
	m_trainVisuals.clearOutlines();
}

} // namespace tzw
