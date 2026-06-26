#include "RailPersistentVisualSystem.h"

#include "EngineSrc/3D/Primitive/CubePrimitive.h"
#include "EngineSrc/3D/Primitive/LinePrimitive.h"
#include "EngineSrc/Base/Node.h"
#include "EngineSrc/Math/vec4.h"

namespace tzw {

namespace
{
constexpr int PersistentTrackVisualRootPriority = 10;
constexpr int PersistentPointVisualRootPriority = 20;
}

RailStationVisual::RailStationVisual(RailStationId stationId)
	: m_stationId(stationId)
{
}

RailStationId RailStationVisual::stationId() const
{
	return m_stationId;
}

void RailStationVisual::sync(Node* visualRoot, const RailNetwork& network,
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
	m_marker->setPos(sample.position + vec3(0.0f, 0.34f, 0.0f));
	m_nameBillboard.sync(visualRoot, sample.position + vec3(0.0f, 1.05f, 0.0f), station.name,
		vec4::fromRGB(80, 70, 35, 225));
}

void RailStationVisual::hide()
{
	if (m_marker)
	{
		m_marker->setIsVisible(false);
	}
	m_nameBillboard.hide();
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
	visualRoot->addChild(m_marker, false);
}

RailRoutePointVisual::RailRoutePointVisual(RailRoutePointId routePointId)
	: m_routePointId(routePointId)
{
}

RailRoutePointId RailRoutePointVisual::routePointId() const
{
	return m_routePointId;
}

void RailRoutePointVisual::sync(Node* visualRoot, const RailNetwork& network,
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
	m_marker->setPos(sample.position + vec3(0.0f, 0.28f, 0.0f));
	m_nameBillboard.sync(visualRoot, sample.position + vec3(0.0f, 0.88f, 0.0f), routePoint.name,
		vec4::fromRGB(55, 45, 75, 225));
}

void RailRoutePointVisual::hide()
{
	if (m_marker)
	{
		m_marker->setIsVisible(false);
	}
	m_nameBillboard.hide();
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
	visualRoot->addChild(m_marker, false);
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
		m_visualRoot->setLocalPiority(PersistentTrackVisualRootPriority);
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

void RailPointVisualSet::sync(Node* sceneRoot, const RailNetwork& network, const RailAnchorManager& anchorManager,
	const RailStationManager& stationManager, const RailRoutePointManager& routePointManager)
{
	if (!sceneRoot)
	{
		return;
	}
	ensureVisualRoot(sceneRoot);
	hideUnused(stationManager, routePointManager);

	for (const RailStation& station : stationManager.stations())
	{
		RailStationVisual& visual = ensureStationVisual(station.id);
		visual.sync(m_visualRoot, network, anchorManager, stationManager, station);
	}

	for (const RailRoutePoint& routePoint : routePointManager.routePoints())
	{
		RailRoutePointVisual& visual = ensureRoutePointVisual(routePoint.id);
		visual.sync(m_visualRoot, network, anchorManager, routePoint);
	}
}

void RailPointVisualSet::clear()
{
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
}

void RailPointVisualSet::ensureVisualRoot(Node* sceneRoot)
{
	if (!m_visualRoot)
	{
		m_visualRoot = new Node();
		m_visualRoot->setName("RailPersistentPointVisualRoot");
		m_visualRoot->setLocalPiority(PersistentPointVisualRootPriority);
	}
	if (!m_visualRoot->getParent())
	{
		sceneRoot->addChild(m_visualRoot, false);
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
		}
	}
	for (RailRoutePointVisual& visual : m_routePointVisuals)
	{
		if (!containsRoutePoint(routePointManager, visual.routePointId()))
		{
			visual.hide();
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

} // namespace tzw
