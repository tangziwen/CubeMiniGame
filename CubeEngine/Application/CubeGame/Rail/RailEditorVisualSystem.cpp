#include "RailEditorVisualSystem.h"

#include "EngineSrc/3D/Primitive/CubePrimitive.h"
#include "EngineSrc/3D/Primitive/LinePrimitive.h"
#include "EngineSrc/Base/Node.h"
#include "EngineSrc/Math/vec4.h"

namespace tzw {

namespace
{
constexpr int EditorVisualRootPriority = 100;
constexpr unsigned int EditorDeleteBillboardPriority = 220;
constexpr unsigned int EditorLineAddBillboardPriority = 230;
constexpr unsigned int EditorLineRemoveBillboardPriority = 240;

WorldBillboardSprite* createEditorBillboard(Node* visualRoot, const vec2& size, unsigned int priority)
{
	auto sprite = WorldBillboardSprite::createWithColor(vec4::fromRGB(255, 255, 255), size);
	sprite->setScreenOffset(vec2(0.0f, 0.0f));
	sprite->setGlobalPiority(priority);
	visualRoot->addChild(sprite, false);
	return sprite;
}

bool stationExists(const RailStationManager& stationManager, RailStationId stationId)
{
	return stationManager.station(stationId) != nullptr;
}

bool routePointExists(const RailRoutePointManager& routePointManager, RailRoutePointId routePointId)
{
	return routePointManager.routePoint(routePointId) != nullptr;
}
}

RailStationDeleteBillboard::RailStationDeleteBillboard(RailStationId stationId)
	: m_stationId(stationId)
{
}

RailStationId RailStationDeleteBillboard::stationId() const
{
	return m_stationId;
}

void RailStationDeleteBillboard::beginSync()
{
	m_isUsedThisFrame = false;
}

void RailStationDeleteBillboard::sync(Node* visualRoot, const RailNetwork& network,
	const RailAnchorManager& anchorManager, const RailStationManager& stationManager,
	const RailStation& station, const std::function<void(RailStationId)>& onClicked)
{
	ensureSprite(visualRoot);
	const RailAnchorId anchorId = stationManager.anchorForStation(station.id);
	RailAnchorSample sample = anchorManager.sampleAnchor(network, anchorId);
	if (!sample.valid)
	{
		hide();
		return;
	}

	m_isUsedThisFrame = true;
	m_sprite->setWorldAnchor(sample.position + vec3(0.0f, 0.95f, 0.0f));
	m_sprite->setColor(vec4::fromRGB(240, 80, 80));
	m_sprite->setOnBtnClicked([stationId = station.id, onClicked](Sprite*)
	{
		if (onClicked)
		{
			onClicked(stationId);
		}
	});
	m_sprite->setOwnerVisible(true);
}

void RailStationDeleteBillboard::hide()
{
	m_isUsedThisFrame = false;
	if (m_sprite)
	{
		m_sprite->setOnBtnClicked(nullptr);
		m_sprite->setOwnerVisible(false);
	}
}

bool RailStationDeleteBillboard::isUsedThisFrame() const
{
	return m_isUsedThisFrame;
}

void RailStationDeleteBillboard::ensureSprite(Node* visualRoot)
{
	if (!m_sprite && visualRoot)
	{
		m_sprite = createEditorBillboard(visualRoot, vec2(22.0f, 22.0f), EditorDeleteBillboardPriority);
	}
}

RailRoutePointDeleteBillboard::RailRoutePointDeleteBillboard(RailRoutePointId routePointId)
	: m_routePointId(routePointId)
{
}

RailRoutePointId RailRoutePointDeleteBillboard::routePointId() const
{
	return m_routePointId;
}

void RailRoutePointDeleteBillboard::beginSync()
{
	m_isUsedThisFrame = false;
}

void RailRoutePointDeleteBillboard::sync(Node* visualRoot, const RailNetwork& network,
	const RailAnchorManager& anchorManager, const RailRoutePoint& routePoint,
	const std::function<void(RailRoutePointId)>& onClicked)
{
	ensureSprite(visualRoot);
	RailAnchorSample sample = anchorManager.sampleAnchor(network, routePoint.anchorId);
	if (!sample.valid)
	{
		hide();
		return;
	}

	m_isUsedThisFrame = true;
	m_sprite->setWorldAnchor(sample.position + vec3(0.0f, 0.82f, 0.0f));
	m_sprite->setColor(vec4::fromRGB(240, 80, 80));
	m_sprite->setOnBtnClicked([routePointId = routePoint.id, onClicked](Sprite*)
	{
		if (onClicked)
		{
			onClicked(routePointId);
		}
	});
	m_sprite->setOwnerVisible(true);
}

void RailRoutePointDeleteBillboard::hide()
{
	m_isUsedThisFrame = false;
	if (m_sprite)
	{
		m_sprite->setOnBtnClicked(nullptr);
		m_sprite->setOwnerVisible(false);
	}
}

bool RailRoutePointDeleteBillboard::isUsedThisFrame() const
{
	return m_isUsedThisFrame;
}

void RailRoutePointDeleteBillboard::ensureSprite(Node* visualRoot)
{
	if (!m_sprite && visualRoot)
	{
		m_sprite = createEditorBillboard(visualRoot, vec2(20.0f, 20.0f), EditorDeleteBillboardPriority);
	}
}

RailLineAddStationBillboard::RailLineAddStationBillboard(RailStationId stationId)
	: m_stationId(stationId)
{
}

RailStationId RailLineAddStationBillboard::stationId() const
{
	return m_stationId;
}

void RailLineAddStationBillboard::beginSync()
{
	m_isUsedThisFrame = false;
}

void RailLineAddStationBillboard::sync(Node* visualRoot, const RailNetwork& network,
	const RailAnchorManager& anchorManager, const RailStationManager& stationManager,
	const RailStation& station, const std::function<void(RailStationId)>& onClicked)
{
	ensureSprite(visualRoot);
	const RailAnchorId anchorId = stationManager.anchorForStation(station.id);
	RailAnchorSample sample = anchorManager.sampleAnchor(network, anchorId);
	if (!sample.valid)
	{
		hide();
		return;
	}

	m_isUsedThisFrame = true;
	m_sprite->setWorldAnchor(sample.position + vec3(0.0f, 1.05f, 0.0f));
	m_sprite->setColor(vec4::fromRGB(80, 190, 240));
	m_sprite->setOnBtnClicked([stationId = station.id, onClicked](Sprite*)
	{
		if (onClicked)
		{
			onClicked(stationId);
		}
	});
	m_sprite->setOwnerVisible(true);
}

void RailLineAddStationBillboard::hide()
{
	m_isUsedThisFrame = false;
	if (m_sprite)
	{
		m_sprite->setOnBtnClicked(nullptr);
		m_sprite->setOwnerVisible(false);
	}
}

bool RailLineAddStationBillboard::isUsedThisFrame() const
{
	return m_isUsedThisFrame;
}

void RailLineAddStationBillboard::ensureSprite(Node* visualRoot)
{
	if (!m_sprite && visualRoot)
	{
		m_sprite = createEditorBillboard(visualRoot, vec2(24.0f, 24.0f), EditorLineAddBillboardPriority);
	}
}

RailLineAddRoutePointBillboard::RailLineAddRoutePointBillboard(RailRoutePointId routePointId)
	: m_routePointId(routePointId)
{
}

RailRoutePointId RailLineAddRoutePointBillboard::routePointId() const
{
	return m_routePointId;
}

void RailLineAddRoutePointBillboard::beginSync()
{
	m_isUsedThisFrame = false;
}

void RailLineAddRoutePointBillboard::sync(Node* visualRoot, const RailNetwork& network,
	const RailAnchorManager& anchorManager, const RailRoutePoint& routePoint,
	const std::function<void(RailRoutePointId)>& onClicked)
{
	ensureSprite(visualRoot);
	RailAnchorSample sample = anchorManager.sampleAnchor(network, routePoint.anchorId);
	if (!sample.valid)
	{
		hide();
		return;
	}

	m_isUsedThisFrame = true;
	m_sprite->setWorldAnchor(sample.position + vec3(0.0f, 0.92f, 0.0f));
	m_sprite->setColor(vec4::fromRGB(80, 220, 120));
	m_sprite->setOnBtnClicked([routePointId = routePoint.id, onClicked](Sprite*)
	{
		if (onClicked)
		{
			onClicked(routePointId);
		}
	});
	m_sprite->setOwnerVisible(true);
}

void RailLineAddRoutePointBillboard::hide()
{
	m_isUsedThisFrame = false;
	if (m_sprite)
	{
		m_sprite->setOnBtnClicked(nullptr);
		m_sprite->setOwnerVisible(false);
	}
}

bool RailLineAddRoutePointBillboard::isUsedThisFrame() const
{
	return m_isUsedThisFrame;
}

void RailLineAddRoutePointBillboard::ensureSprite(Node* visualRoot)
{
	if (!m_sprite && visualRoot)
	{
		m_sprite = createEditorBillboard(visualRoot, vec2(22.0f, 22.0f), EditorLineAddBillboardPriority);
	}
}

void RailLineRemoveControlPointBillboard::sync(Node* visualRoot, int controlPointIndex,
	const RailLineControlPoint& controlPoint, const RailNetwork& network,
	const RailAnchorManager& anchorManager, const RailStationManager& stationManager,
	const RailRoutePointManager& routePointManager, const std::function<void(int)>& onClicked)
{
	ensureSprite(visualRoot);
	vec3 position;
	if (!resolvePosition(controlPoint, network, anchorManager, stationManager, routePointManager, position))
	{
		hide();
		return;
	}

	m_isUsedThisFrame = true;
	m_sprite->setWorldAnchor(position + vec3(0.0f, 1.12f, 0.0f));
	m_sprite->setColor(vec4::fromRGB(240, 120, 60));
	m_sprite->setOnBtnClicked([controlPointIndex, onClicked](Sprite*)
	{
		if (onClicked)
		{
			onClicked(controlPointIndex);
		}
	});
	m_sprite->setOwnerVisible(true);
}

void RailLineRemoveControlPointBillboard::hide()
{
	m_isUsedThisFrame = false;
	if (m_sprite)
	{
		m_sprite->setOnBtnClicked(nullptr);
		m_sprite->setOwnerVisible(false);
	}
}

void RailLineRemoveControlPointBillboard::beginSync()
{
	m_isUsedThisFrame = false;
}

bool RailLineRemoveControlPointBillboard::isUsedThisFrame() const
{
	return m_isUsedThisFrame;
}

void RailLineRemoveControlPointBillboard::ensureSprite(Node* visualRoot)
{
	if (!m_sprite && visualRoot)
	{
		m_sprite = createEditorBillboard(visualRoot, vec2(24.0f, 24.0f), EditorLineRemoveBillboardPriority);
	}
}

bool RailLineRemoveControlPointBillboard::resolvePosition(const RailLineControlPoint& controlPoint,
	const RailNetwork& network, const RailAnchorManager& anchorManager,
	const RailStationManager& stationManager, const RailRoutePointManager& routePointManager,
	vec3& outPosition) const
{
	RailAnchorId anchorId = InvalidRailAnchorId;
	if (controlPoint.kind == RailLineControlPointKind::Station)
	{
		anchorId = stationManager.anchorForStation(controlPoint.stationId);
	}
	else
	{
		const RailRoutePoint* routePoint = routePointManager.routePoint(controlPoint.routePointId);
		if (routePoint)
		{
			anchorId = routePoint->anchorId;
		}
	}

	RailAnchorSample sample = anchorManager.sampleAnchor(network, anchorId);
	if (!sample.valid)
	{
		return false;
	}

	outPosition = sample.position;
	return true;
}

void RailEditorVisualSystem::prepare(Node* sceneRoot)
{
	ensureVisualRoot(sceneRoot);
}

void RailEditorVisualSystem::clear()
{
	m_linePreviewVisual.clear();
	m_nodeVisuals.clear();
	m_trackPreviewSleeperVisuals.clear();
	m_pendingAnchorVisual = nullptr;
	m_trackPreviewVisual = nullptr;
	m_stationDeleteBillboards.clear();
	m_routePointDeleteBillboards.clear();
	m_lineAddStationBillboards.clear();
	m_lineAddRoutePointBillboards.clear();
	m_lineRemoveControlPointBillboards.clear();
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

void RailEditorVisualSystem::showTrackAddPreview(const RailNetwork& network, const RailBuildTool& buildTool,
	const RailConfig& config, PlacementMode placementMode)
{
	syncNodeVisuals(network, buildTool, config, true, false);
	syncTrackPreview(buildTool, config, placementMode, true);
	hideBillboards();
}

void RailEditorVisualSystem::showTrackDeleteHints(const RailNetwork& network, const RailConfig& config)
{
	RailBuildTool emptyBuildTool;
	syncNodeVisuals(network, emptyBuildTool, config, true, false);
	hideTrackPreview();
	hideBillboards();
}

void RailEditorVisualSystem::showStationBillboards(const RailNetwork& network,
	const RailAnchorManager& anchorManager, const RailStationManager& stationManager, bool deleteMode,
	const std::function<void(RailStationId)>& onClicked)
{
	if (!m_visualRoot)
	{
		return;
	}
	hideNodeVisuals();
	hideTrackPreview();
	hideRoutePointDeleteBillboards();
	hideLineAddBillboards();
	hideLineRemoveControlPointBillboards();
	beginStationDeleteBillboardSync();

	if (deleteMode)
	{
		for (const RailStation& station : stationManager.stations())
		{
			ensureStationDeleteBillboard(station.id).sync(m_visualRoot, network, anchorManager,
				stationManager, station, onClicked);
		}
	}

	hideUnusedStationDeleteBillboards(stationManager);
}

void RailEditorVisualSystem::showRoutePointBillboards(const RailNetwork& network,
	const RailAnchorManager& anchorManager, const RailRoutePointManager& routePointManager, bool deleteMode,
	const std::function<void(RailRoutePointId)>& onClicked)
{
	if (!m_visualRoot)
	{
		return;
	}
	hideNodeVisuals();
	hideTrackPreview();
	hideStationDeleteBillboards();
	hideLineAddBillboards();
	hideLineRemoveControlPointBillboards();
	beginRoutePointDeleteBillboardSync();

	if (deleteMode)
	{
		for (const RailRoutePoint& routePoint : routePointManager.routePoints())
		{
			ensureRoutePointDeleteBillboard(routePoint.id).sync(m_visualRoot, network, anchorManager,
				routePoint, onClicked);
		}
	}

	hideUnusedRoutePointDeleteBillboards(routePointManager);
}

void RailEditorVisualSystem::showLineAddControlBillboards(const RailNetwork& network,
	const RailAnchorManager& anchorManager, const RailStationManager& stationManager,
	const RailRoutePointManager& routePointManager,
	const std::function<void(RailStationId)>& onStationClicked,
	const std::function<void(RailRoutePointId)>& onRoutePointClicked)
{
	if (!m_visualRoot)
	{
		return;
	}
	hideNodeVisuals();
	hideTrackPreview();
	hideStationDeleteBillboards();
	hideRoutePointDeleteBillboards();
	hideLineRemoveControlPointBillboards();
	beginLineAddBillboardSync();

	for (const RailStation& station : stationManager.stations())
	{
		ensureLineAddStationBillboard(station.id).sync(m_visualRoot, network, anchorManager,
			stationManager, station, onStationClicked);
	}
	for (const RailRoutePoint& routePoint : routePointManager.routePoints())
	{
		ensureLineAddRoutePointBillboard(routePoint.id).sync(m_visualRoot, network, anchorManager,
			routePoint, onRoutePointClicked);
	}
	hideUnusedLineAddBillboards(stationManager, routePointManager);
}

void RailEditorVisualSystem::showLineRemoveControlBillboards(const RailNetwork& network,
	const RailAnchorManager& anchorManager, const RailStationManager& stationManager,
	const RailRoutePointManager& routePointManager, const RailLine* line,
	const std::function<void(int)>& onControlPointClicked)
{
	if (!m_visualRoot)
	{
		return;
	}
	hideNodeVisuals();
	hideTrackPreview();
	hideStationDeleteBillboards();
	hideRoutePointDeleteBillboards();
	hideLineAddBillboards();
	beginLineRemoveBillboardSync();

	const int controlPointCount = line ? static_cast<int>(line->controlPoints.size()) : 0;
	while (static_cast<int>(m_lineRemoveControlPointBillboards.size()) < controlPointCount)
	{
		m_lineRemoveControlPointBillboards.emplace_back();
	}

	for (int i = 0; i < static_cast<int>(m_lineRemoveControlPointBillboards.size()); ++i)
	{
		if (i < controlPointCount)
		{
			m_lineRemoveControlPointBillboards[i].sync(m_visualRoot, i, line->controlPoints[i],
				network, anchorManager, stationManager, routePointManager, onControlPointClicked);
		}
		else
		{
			m_lineRemoveControlPointBillboards[i].hide();
		}
	}
	hideUnusedLineRemoveControlPointBillboards();
}

void RailEditorVisualSystem::hideInteractionVisuals()
{
	hideNodeVisuals();
	hideTrackPreview();
	hideBillboards();
}

void RailEditorVisualSystem::showLinePreview(Node* sceneRoot, const RailNetwork& network, const RailLine* line)
{
	m_linePreviewVisual.show(sceneRoot, network, line);
}

void RailEditorVisualSystem::clearLinePreview()
{
	m_linePreviewVisual.clear();
}

void RailEditorVisualSystem::hideAll()
{
	hideInteractionVisuals();
	clearLinePreview();
}

void RailEditorVisualSystem::ensureVisualRoot(Node* sceneRoot)
{
	if (!sceneRoot)
	{
		return;
	}
	if (!m_visualRoot)
	{
		m_visualRoot = new Node();
		m_visualRoot->setName("RailEditorVisualRoot");
		m_visualRoot->setLocalPiority(EditorVisualRootPriority);
	}
	if (!m_visualRoot->getParent())
	{
		sceneRoot->addChild(m_visualRoot, false);
	}
}

void RailEditorVisualSystem::syncNodeVisuals(const RailNetwork& network, const RailBuildTool& buildTool,
	const RailConfig& config, bool showNodes, bool lineAddMode)
{
	if (!m_visualRoot)
	{
		return;
	}

	std::vector<RailNodeVisual> nodeVisualData;
	m_nodeMesh.build(network, config, nodeVisualData);

	while (m_nodeVisuals.size() < nodeVisualData.size())
	{
		auto nodeCube = new CubePrimitive(0.4f, 0.4f, 0.4f, true);
		nodeCube->setIsAccpectOcTtree(false);
		nodeCube->setIsHitable(false);
		m_visualRoot->addChild(nodeCube, false);
		m_nodeVisuals.push_back(nodeCube);
	}

	const RailNodeId pendingId = buildTool.pendingNodeId();
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
		else if (lineAddMode)
		{
			const RailNode* node = network.node(nodeId);
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
		&& buildTool.pendingNodeId() == InvalidRailNodeId
		&& buildTool.pendingAnchorPosition(pendingPosition))
	{
		m_pendingAnchorVisual->setIsVisible(true);
		m_pendingAnchorVisual->setPos(pendingPosition + vec3(0.0f, 0.12f, 0.0f));
	}
	else
	{
		m_pendingAnchorVisual->setIsVisible(false);
	}
}

void RailEditorVisualSystem::syncTrackPreview(const RailBuildTool& buildTool, const RailConfig& config,
	PlacementMode placementMode, bool showTrackPreview)
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
		hideTrackPreview();
		return;
	}

	RailSegment previewSegment;
	if (!buildTool.buildPreviewSegment(placementMode, previewSegment))
	{
		hideTrackPreview();
		return;
	}

	RailTrackVisualData previewData;
	m_trackMesh.appendSegmentVisual(previewSegment, config, previewData);
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

void RailEditorVisualSystem::hideTrackPreview()
{
	if (m_trackPreviewVisual)
	{
		m_trackPreviewVisual->clear();
		m_trackPreviewVisual->setIsVisible(false);
	}
	for (CubePrimitive* sleeper : m_trackPreviewSleeperVisuals)
	{
		if (sleeper)
		{
			sleeper->setIsVisible(false);
		}
	}
}

void RailEditorVisualSystem::hideNodeVisuals()
{
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
}

RailStationDeleteBillboard& RailEditorVisualSystem::ensureStationDeleteBillboard(RailStationId stationId)
{
	for (RailStationDeleteBillboard& billboard : m_stationDeleteBillboards)
	{
		if (billboard.stationId() == stationId)
		{
			return billboard;
		}
	}

	m_stationDeleteBillboards.emplace_back(stationId);
	return m_stationDeleteBillboards.back();
}

RailRoutePointDeleteBillboard& RailEditorVisualSystem::ensureRoutePointDeleteBillboard(RailRoutePointId routePointId)
{
	for (RailRoutePointDeleteBillboard& billboard : m_routePointDeleteBillboards)
	{
		if (billboard.routePointId() == routePointId)
		{
			return billboard;
		}
	}

	m_routePointDeleteBillboards.emplace_back(routePointId);
	return m_routePointDeleteBillboards.back();
}

RailLineAddStationBillboard& RailEditorVisualSystem::ensureLineAddStationBillboard(RailStationId stationId)
{
	for (RailLineAddStationBillboard& billboard : m_lineAddStationBillboards)
	{
		if (billboard.stationId() == stationId)
		{
			return billboard;
		}
	}

	m_lineAddStationBillboards.emplace_back(stationId);
	return m_lineAddStationBillboards.back();
}

RailLineAddRoutePointBillboard& RailEditorVisualSystem::ensureLineAddRoutePointBillboard(RailRoutePointId routePointId)
{
	for (RailLineAddRoutePointBillboard& billboard : m_lineAddRoutePointBillboards)
	{
		if (billboard.routePointId() == routePointId)
		{
			return billboard;
		}
	}

	m_lineAddRoutePointBillboards.emplace_back(routePointId);
	return m_lineAddRoutePointBillboards.back();
}

void RailEditorVisualSystem::beginStationDeleteBillboardSync()
{
	for (RailStationDeleteBillboard& billboard : m_stationDeleteBillboards)
	{
		billboard.beginSync();
	}
}

void RailEditorVisualSystem::beginRoutePointDeleteBillboardSync()
{
	for (RailRoutePointDeleteBillboard& billboard : m_routePointDeleteBillboards)
	{
		billboard.beginSync();
	}
}

void RailEditorVisualSystem::beginLineAddBillboardSync()
{
	for (RailLineAddStationBillboard& billboard : m_lineAddStationBillboards)
	{
		billboard.beginSync();
	}
	for (RailLineAddRoutePointBillboard& billboard : m_lineAddRoutePointBillboards)
	{
		billboard.beginSync();
	}
}

void RailEditorVisualSystem::beginLineRemoveBillboardSync()
{
	for (RailLineRemoveControlPointBillboard& billboard : m_lineRemoveControlPointBillboards)
	{
		billboard.beginSync();
	}
}

void RailEditorVisualSystem::hideBillboards()
{
	hideStationDeleteBillboards();
	hideRoutePointDeleteBillboards();
	hideLineAddBillboards();
	hideLineRemoveControlPointBillboards();
}

void RailEditorVisualSystem::hideStationDeleteBillboards()
{
	for (RailStationDeleteBillboard& billboard : m_stationDeleteBillboards)
	{
		billboard.hide();
	}
}

void RailEditorVisualSystem::hideRoutePointDeleteBillboards()
{
	for (RailRoutePointDeleteBillboard& billboard : m_routePointDeleteBillboards)
	{
		billboard.hide();
	}
}

void RailEditorVisualSystem::hideLineAddBillboards()
{
	for (RailLineAddStationBillboard& billboard : m_lineAddStationBillboards)
	{
		billboard.hide();
	}
	for (RailLineAddRoutePointBillboard& billboard : m_lineAddRoutePointBillboards)
	{
		billboard.hide();
	}
}

void RailEditorVisualSystem::hideLineRemoveControlPointBillboards()
{
	for (RailLineRemoveControlPointBillboard& billboard : m_lineRemoveControlPointBillboards)
	{
		billboard.hide();
	}
}

void RailEditorVisualSystem::hideUnusedStationDeleteBillboards(const RailStationManager& stationManager)
{
	for (RailStationDeleteBillboard& billboard : m_stationDeleteBillboards)
	{
		if (!billboard.isUsedThisFrame() || !stationExists(stationManager, billboard.stationId()))
		{
			billboard.hide();
		}
	}
}

void RailEditorVisualSystem::hideUnusedRoutePointDeleteBillboards(const RailRoutePointManager& routePointManager)
{
	for (RailRoutePointDeleteBillboard& billboard : m_routePointDeleteBillboards)
	{
		if (!billboard.isUsedThisFrame() || !routePointExists(routePointManager, billboard.routePointId()))
		{
			billboard.hide();
		}
	}
}

void RailEditorVisualSystem::hideUnusedLineAddBillboards(const RailStationManager& stationManager,
	const RailRoutePointManager& routePointManager)
{
	for (RailLineAddStationBillboard& billboard : m_lineAddStationBillboards)
	{
		if (!billboard.isUsedThisFrame() || !stationExists(stationManager, billboard.stationId()))
		{
			billboard.hide();
		}
	}
	for (RailLineAddRoutePointBillboard& billboard : m_lineAddRoutePointBillboards)
	{
		if (!billboard.isUsedThisFrame() || !routePointExists(routePointManager, billboard.routePointId()))
		{
			billboard.hide();
		}
	}
}

void RailEditorVisualSystem::hideUnusedLineRemoveControlPointBillboards()
{
	for (RailLineRemoveControlPointBillboard& billboard : m_lineRemoveControlPointBillboards)
	{
		if (!billboard.isUsedThisFrame())
		{
			billboard.hide();
		}
	}
}

} // namespace tzw
