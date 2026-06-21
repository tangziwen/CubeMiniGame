#include "RailBuildTool.h"

#include "CubeGame/BuildingSystem.h"

#include <cmath>

namespace tzw {

namespace
{
bool isValidHitPoint(const vec3& point)
{
	return point.x > -999990.0f;
}
}

void RailBuildTool::bind(RailNetwork* network, const RailConfig* config)
{
	m_network = network;
	m_config = config;
	clearPending();
}

void RailBuildTool::setMode(RailBuildMode mode)
{
	if (m_mode == mode)
	{
		return;
	}
	m_mode = mode;
	clearPending();
}

RailBuildMode RailBuildTool::mode() const
{
	return m_mode;
}

bool RailBuildTool::hasPendingAnchor() const
{
	return m_pendingAnchor.type != AnchorType::None;
}

RailNodeId RailBuildTool::pendingNodeId() const
{
	if (m_pendingAnchor.type == AnchorType::Node)
	{
		return m_pendingAnchor.nodeId;
	}
	return InvalidRailNodeId;
}

void RailBuildTool::clearPending()
{
	m_pendingAnchor = Anchor();
}

bool RailBuildTool::handlePrimaryClick(PlacementMode placementMode)
{
	if (!m_network || !m_config)
	{
		return false;
	}

	Anchor anchor = pickAnchor(placementMode);
	if (anchor.type == AnchorType::None)
	{
		return false;
	}

	if (m_mode == RailBuildMode::Delete)
	{
		if (anchor.type == AnchorType::Segment)
		{
			clearPending();
			return m_network->deleteSegment(anchor.segmentId);
		}
		return false;
	}

	if (m_mode != RailBuildMode::Add)
	{
		return false;
	}

	if (hasPendingAnchor())
	{
		if (anchor.type == AnchorType::Segment)
		{
			m_pendingAnchor = anchor;
			return false;
		}

		RailNodeId endNodeId = InvalidRailNodeId;
		const bool didCommit = commitAdd(m_pendingAnchor, anchor, &endNodeId);
		if (didCommit && endNodeId != InvalidRailNodeId)
		{
			m_pendingAnchor.type = AnchorType::Node;
			m_pendingAnchor.nodeId = endNodeId;
			m_pendingAnchor.segmentId = InvalidRailSegmentId;
			m_pendingAnchor.distanceOnSegment = 0.0f;
			const RailNode* railNode = m_network->node(endNodeId);
			m_pendingAnchor.position = railNode ? railNode->position : anchor.position;
		}
		return didCommit;
	}

	if (anchor.type == AnchorType::Node)
	{
		m_pendingAnchor = anchor;
		return false;
	}

	RailNodeId nodeId = InvalidRailNodeId;
	if (!resolveAnchorToNode(anchor, nodeId) || nodeId == InvalidRailNodeId)
	{
		m_network->deleteIsolatedOrdinaryNodes();
		return false;
	}

	m_pendingAnchor.type = AnchorType::Node;
	m_pendingAnchor.nodeId = nodeId;
	m_pendingAnchor.segmentId = InvalidRailSegmentId;
	m_pendingAnchor.distanceOnSegment = 0.0f;
	const RailNode* railNode = m_network->node(nodeId);
	m_pendingAnchor.position = railNode ? railNode->position : anchor.position;
	return false;
}

void RailBuildTool::handleSecondaryClick()
{
	if (m_mode == RailBuildMode::Add || m_mode == RailBuildMode::Delete)
	{
		clearPending();
	}
}

RailBuildTool::Anchor RailBuildTool::pickAnchor(PlacementMode placementMode) const
{
	Anchor result;
	if (!m_network || !m_config)
	{
		return result;
	}

	const vec3 terrainHit = BuildingSystem::shared()->hitTerrain(placementMode);
	if (!isValidHitPoint(terrainHit))
	{
		return result;
	}

	if (m_mode != RailBuildMode::Delete)
	{
		RailNodePick nodePick = m_network->findNearestNode(terrainHit, m_config->nodePickRadius);
		if (nodePick.nodeId != InvalidRailNodeId)
		{
			result.type = AnchorType::Node;
			result.nodeId = nodePick.nodeId;
			const RailNode* railNode = m_network->node(nodePick.nodeId);
			result.position = railNode ? railNode->position : terrainHit;
			return result;
		}
	}

	RailSegmentPick segmentPick = m_network->findNearestSegment(terrainHit, m_config->segmentPickRadius);
	if (segmentPick.segmentId != InvalidRailSegmentId)
	{
		result.type = AnchorType::Segment;
		result.segmentId = segmentPick.segmentId;
		result.distanceOnSegment = segmentPick.distanceOnSegment;
		result.position = segmentPick.position;
		return result;
	}

	if (m_mode == RailBuildMode::Delete)
	{
		return result;
	}

	result.type = AnchorType::Terrain;
	result.position = terrainHit;
	return result;
}

bool RailBuildTool::commitAdd(const Anchor& startAnchor, const Anchor& endAnchor, RailNodeId* outEndNode)
{
	if (outEndNode)
	{
		*outEndNode = InvalidRailNodeId;
	}

	if (startAnchor.type == AnchorType::Segment
		&& endAnchor.type == AnchorType::Segment
		&& startAnchor.segmentId == endAnchor.segmentId)
	{
		return false;
	}

	const float anchorDistance = startAnchor.position.distance(endAnchor.position);
	if (anchorDistance < m_config->minSegmentLength || anchorDistance > m_config->maxSegmentLength)
	{
		return false;
	}

	const vec3 anchorDelta = endAnchor.position - startAnchor.position;
	const float horizontalDistance = std::sqrt(anchorDelta.x * anchorDelta.x + anchorDelta.z * anchorDelta.z);
	if (horizontalDistance > 0.0001f && std::fabs(anchorDelta.y) / horizontalDistance > m_config->maxSlope)
	{
		return false;
	}

	RailNodeId startNode = InvalidRailNodeId;
	RailNodeId endNode = InvalidRailNodeId;
	if (!resolveAnchorToNode(startAnchor, startNode) || !resolveAnchorToNode(endAnchor, endNode))
	{
		m_network->deleteIsolatedOrdinaryNodes();
		return false;
	}

	if (startNode == endNode)
	{
		m_network->deleteIsolatedOrdinaryNodes();
		return false;
	}

	const RailSegmentId segmentId = m_network->createSegment(startNode, endNode);
	if (segmentId == InvalidRailSegmentId)
	{
		m_network->deleteIsolatedOrdinaryNodes();
		return false;
	}

	if (outEndNode)
	{
		*outEndNode = endNode;
	}
	return true;
}

bool RailBuildTool::resolveAnchorToNode(const Anchor& anchor, RailNodeId& outNodeId)
{
	outNodeId = InvalidRailNodeId;
	switch (anchor.type)
	{
	case AnchorType::Node:
		outNodeId = anchor.nodeId;
		return outNodeId != InvalidRailNodeId;
	case AnchorType::Terrain:
		outNodeId = m_network->createNode(anchor.position);
		return outNodeId != InvalidRailNodeId;
	case AnchorType::Segment:
		{
			RailSplitResult splitResult;
			if (!m_network->splitSegment(anchor.segmentId, anchor.distanceOnSegment, splitResult))
			{
				return false;
			}
			outNodeId = splitResult.nodeId;
			return outNodeId != InvalidRailNodeId;
		}
	default:
		break;
	}
	return false;
}

} // namespace tzw
