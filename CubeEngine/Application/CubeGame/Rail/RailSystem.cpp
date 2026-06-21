#include "RailSystem.h"

#include "CubeGame/BuildingSystem.h"
#include "EngineSrc/3D/Primitive/CubePrimitive.h"
#include "EngineSrc/3D/Primitive/LinePrimitive.h"
#include "EngineSrc/Base/Node.h"
#include "EngineSrc/Math/vec4.h"

#include <unordered_set>

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

void RailSystem::update(Node* sceneRoot, float deltaSeconds)
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
	m_trainManager.update(deltaSeconds, m_lineManager);
	m_trainManager.updateWorldPoses(m_network, m_lineManager);
	m_trainVisuals.sync(sceneRoot, m_network, m_lineManager, m_trainManager);
	if (m_previewLineId != InvalidRailLineId)
	{
		m_linePreviewVisual.show(sceneRoot, m_network, m_lineManager.line(m_previewLineId));
	}
}

void RailSystem::clear()
{
	m_buildTool.clearPending();
	m_network.clear();
	m_lineManager.clear();
	m_trainManager.clear();
	m_trainVisuals.clear();
	m_linePreviewVisual.clear();
	m_previewLineId = InvalidRailLineId;
	clearVisuals();
	m_visualDirty = true;
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

bool RailSystem::addPickedNodeToSelectedLine(PlacementMode placementMode)
{
	if (m_lineManager.selectedLineId() == InvalidRailLineId)
	{
		return false;
	}

	const vec3 terrainHit = BuildingSystem::shared()->hitTerrain(placementMode);
	if (!BuildingSystem::isValidHitPoint(terrainHit))
	{
		return false;
	}

	RailNodePick nodePick = m_network.findNearestNode(terrainHit, m_config.nodePickRadius);
	if (nodePick.nodeId == InvalidRailNodeId)
	{
		return false;
	}

	const bool changed = m_lineManager.addControlNode(m_lineManager.selectedLineId(), nodePick.nodeId, m_network);
	if (changed)
	{
		m_previewLineId = m_lineManager.selectedLineId();
		m_trainManager.refreshAfterLineRebuild(m_lineManager);
		markVisualDirty();
	}
	return changed;
}

bool RailSystem::removePickedNodeFromSelectedLine(PlacementMode placementMode)
{
	const RailLineId selectedLineId = m_lineManager.selectedLineId();
	RailLine* selectedLine = m_lineManager.line(selectedLineId);
	if (!selectedLine)
	{
		return false;
	}

	const vec3 terrainHit = BuildingSystem::shared()->hitTerrain(placementMode);
	if (!BuildingSystem::isValidHitPoint(terrainHit))
	{
		return false;
	}

	RailNodePick nodePick = m_network.findNearestNode(terrainHit, m_config.nodePickRadius);
	if (nodePick.nodeId == InvalidRailNodeId)
	{
		return false;
	}

	for (int i = 0; i < static_cast<int>(selectedLine->controlNodes.size()); ++i)
	{
		if (selectedLine->controlNodes[i] != nodePick.nodeId)
		{
			continue;
		}

		const bool changed = m_lineManager.removeControlNodeAt(selectedLineId, i, m_network);
		if (changed)
		{
			m_previewLineId = selectedLineId;
			m_trainManager.refreshAfterLineRebuild(m_lineManager);
			markVisualDirty();
		}
		return changed;
	}
	return false;
}

void RailSystem::setLinePreview(RailLineId lineId)
{
	if (m_previewLineId == lineId)
	{
		return;
	}
	m_previewLineId = lineId;
	markVisualDirty();
}

void RailSystem::clearLinePreview()
{
	if (m_previewLineId == InvalidRailLineId)
	{
		return;
	}
	m_previewLineId = InvalidRailLineId;
	m_linePreviewVisual.clear();
	markVisualDirty();
}

void RailSystem::rebuildAllRailLines()
{
	m_lineManager.rebuildAll(m_network);
	m_trainManager.refreshAfterLineRebuild(m_lineManager);
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

	for (CubePrimitive* nodeCube : m_nodeVisuals)
	{
		if (nodeCube)
		{
			nodeCube->setIsVisible(false);
		}
	}

	if (m_pendingAnchorVisual)
	{
		m_pendingAnchorVisual->setIsVisible(false);
	}

	m_visualDirty = false;
}

void RailSystem::syncTrackAddVisuals(PlacementMode placementMode)
{
	syncNodeVisuals(true, false, false);
	syncTrackPreview(placementMode, true);
}

void RailSystem::syncTrackDeleteVisuals()
{
	syncNodeVisuals(true, false, false);
	syncTrackPreview(PlacementMode::CursorBased, false);
}

void RailSystem::syncLineAddNodeVisuals()
{
	syncNodeVisuals(true, true, false);
	syncTrackPreview(PlacementMode::CursorBased, false);
}

void RailSystem::syncLineRemoveNodeVisuals()
{
	syncNodeVisuals(true, false, true);
	syncTrackPreview(PlacementMode::CursorBased, false);
}

void RailSystem::hideEditorVisuals()
{
	syncNodeVisuals(false, false, false);
	syncTrackPreview(PlacementMode::CursorBased, false);
}

void RailSystem::syncNodeVisuals(bool showNodes, bool lineAddMode, bool lineRemoveMode)
{
	if (!m_visualRoot)
	{
		return;
	}

	std::vector<RailNodeVisual> nodeVisualData;
	m_nodeMesh.build(m_network, m_config, nodeVisualData);

	while (m_nodeVisuals.size() < nodeVisualData.size())
	{
		auto nodeCube = new CubePrimitive(0.4f, 0.4f, 0.4f, true);
		nodeCube->setIsAccpectOcTtree(false);
		nodeCube->setIsHitable(false);
		m_visualRoot->addChild(nodeCube, false);
		m_nodeVisuals.push_back(nodeCube);
	}

	const RailNodeId pendingId = m_buildTool.pendingNodeId();
	const RailLineId selectedLineId = m_lineManager.selectedLineId();
	std::unordered_set<RailNodeId> selectedLineNodes;
	if (selectedLineId != InvalidRailLineId)
	{
		const RailLine* selectedLine = m_lineManager.line(selectedLineId);
		if (selectedLine)
		{
			selectedLineNodes.insert(selectedLine->controlNodes.begin(), selectedLine->controlNodes.end());
		}
	}

	for (size_t i = 0; i < m_nodeVisuals.size(); ++i)
	{
		CubePrimitive* nodeCube = m_nodeVisuals[i];
		if (i >= nodeVisualData.size() || !showNodes)
		{
			nodeCube->setIsVisible(false);
			continue;
		}

		nodeCube->setIsVisible(true);
		nodeCube->setPos(nodeVisualData[i].position);
		nodeCube->setScale(vec3(1.0f, 1.0f, 1.0f));

		const RailNodeId nodeId = nodeVisualData[i].nodeId;
		if (nodeId == pendingId)
		{
			nodeCube->setColor(vec4::fromRGB(80, 220, 80));
		}
		else if (selectedLineNodes.find(nodeId) != selectedLineNodes.end())
		{
			nodeCube->setColor(lineRemoveMode ? vec4::fromRGB(240, 80, 80) : vec4::fromRGB(80, 180, 220));
		}
		else if (lineAddMode)
		{
			const RailNode* node = m_network.node(nodeId);
			nodeCube->setColor((node && node->isConnectable) ? vec4::fromRGB(80, 220, 80) : vec4::fromRGB(220, 180, 40));
		}
		else
		{
			nodeCube->setColor(vec4::fromRGB(220, 180, 40));
		}
	}

	if (!m_pendingAnchorVisual)
	{
		m_pendingAnchorVisual = new CubePrimitive(0.44f, 0.44f, 0.44f, true);
		m_pendingAnchorVisual->setIsAccpectOcTtree(false);
		m_pendingAnchorVisual->setIsHitable(false);
		m_pendingAnchorVisual->setColor(vec4::fromRGB(80, 220, 80));
		m_visualRoot->addChild(m_pendingAnchorVisual, false);
	}

	vec3 pendingPosition;
	if (showNodes
		&& m_buildTool.pendingNodeId() == InvalidRailNodeId
		&& m_buildTool.pendingAnchorPosition(pendingPosition))
	{
		m_pendingAnchorVisual->setIsVisible(true);
		m_pendingAnchorVisual->setPos(pendingPosition + vec3(0.0f, 0.12f, 0.0f));
	}
	else
	{
		m_pendingAnchorVisual->setIsVisible(false);
	}
}

void RailSystem::syncTrackPreview(PlacementMode placementMode, bool showTrackPreview)
{
	if (!m_visualRoot)
	{
		return;
	}
	if (!m_trackPreviewVisual)
	{
		m_trackPreviewVisual = new LinePrimitive();
		m_trackPreviewVisual->setIsAccpectOcTtree(false);
		m_trackPreviewVisual->setIsHitable(false);
		m_visualRoot->addChild(m_trackPreviewVisual, false);
	}

	m_trackPreviewVisual->clear();
	if (!showTrackPreview)
	{
		m_trackPreviewVisual->setIsVisible(false);
		for (CubePrimitive* sleeper : m_trackPreviewSleeperVisuals)
		{
			if (sleeper)
			{
				sleeper->setIsVisible(false);
			}
		}
		return;
	}

	RailSegment previewSegment;
	if (!m_buildTool.buildPreviewSegment(placementMode, previewSegment))
	{
		m_trackPreviewVisual->setIsVisible(false);
		for (CubePrimitive* sleeper : m_trackPreviewSleeperVisuals)
		{
			if (sleeper)
			{
				sleeper->setIsVisible(false);
			}
		}
		return;
	}

	RailTrackVisualData previewData;
	m_trackMesh.appendSegmentVisual(previewSegment, m_config, previewData);
	for (const RailLineVisual& line : previewData.lines)
	{
		m_trackPreviewVisual->append(line.start + vec3(0.0f, 0.08f, 0.0f), line.end + vec3(0.0f, 0.08f, 0.0f),
			vec3(0.15f, 0.95f, 0.35f));
	}

	const bool hasLines = m_trackPreviewVisual->getLineCount() > 0;
	m_trackPreviewVisual->setIsVisible(hasLines);
	if (hasLines)
	{
		m_trackPreviewVisual->initBuffer();
	}

	while (m_trackPreviewSleeperVisuals.size() < previewData.sleepers.size())
	{
		auto sleeper = new CubePrimitive(1.25f, 0.2f, 0.12f, true);
		sleeper->setIsAccpectOcTtree(false);
		sleeper->setIsHitable(false);
		sleeper->setColor(vec4(0.15f, 0.95f, 0.35f, 1.0f));
		m_visualRoot->addChild(sleeper, false);
		m_trackPreviewSleeperVisuals.push_back(sleeper);
	}

	for (size_t i = 0; i < m_trackPreviewSleeperVisuals.size(); ++i)
	{
		CubePrimitive* sleeper = m_trackPreviewSleeperVisuals[i];
		if (i < previewData.sleepers.size())
		{
			sleeper->setIsVisible(true);
			sleeper->setPos(previewData.sleepers[i].position + vec3(0.0f, 0.08f, 0.0f));
			sleeper->setRotateQ(previewData.sleepers[i].rotation);
		}
		else
		{
			sleeper->setIsVisible(false);
		}
	}
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
	m_nodeVisuals.clear();
	m_trackPreviewSleeperVisuals.clear();
	m_pendingAnchorVisual = nullptr;
	m_trackPreviewVisual = nullptr;
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
