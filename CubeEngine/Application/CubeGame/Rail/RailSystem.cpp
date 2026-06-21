#include "RailSystem.h"

#include "EngineSrc/3D/Primitive/CubePrimitive.h"
#include "EngineSrc/3D/Primitive/LinePrimitive.h"
#include "EngineSrc/Base/Node.h"
#include "EngineSrc/Math/vec4.h"

namespace tzw {

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

void RailSystem::update(Node* sceneRoot, float)
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
}

void RailSystem::clear()
{
	m_buildTool.clearPending();
	m_network.clear();
	clearVisuals();
	m_visualDirty = true;
}

void RailSystem::setBuildMode(RailBuildMode mode)
{
	m_buildTool.setMode(mode);
}

RailBuildMode RailSystem::buildMode() const
{
	return m_buildTool.mode();
}

bool RailSystem::handlePrimaryClick(PlacementMode placementMode)
{
	const bool changed = m_buildTool.handlePrimaryClick(placementMode);
	if (changed)
	{
		markVisualDirty();
	}
	return changed;
}

RailNetwork& RailSystem::network()
{
	return m_network;
}

const RailNetwork& RailSystem::network() const
{
	return m_network;
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
		auto sleeper = new CubePrimitive(0.9f, 0.16f, 0.08f, true);
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

	m_visualDirty = false;
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
