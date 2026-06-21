#include "RailBuildTool.h"

#include "CubeGame/BuildingSystem.h"

#include <cmath>

namespace tzw {

namespace
{
vec3 directionForPreviewAnchor(const RailNetwork* network, const RailBuildAnchor& anchor, const vec3& chordDirection)
{
	if (!network)
	{
		return chordDirection;
	}

	if (anchor.type == RailBuildAnchorType::Node)
	{
		const RailNode* node = network->node(anchor.nodeId);
		if (node)
		{
			const vec3 preferred = safeNormalized(node->preferredTangent, chordDirection);
			if (vec3::DotProduct(preferred, chordDirection) > 0.15f)
			{
				return preferred;
			}
		}
	}
	else if (anchor.type == RailBuildAnchorType::Segment)
	{
		const RailSegment* segment = network->segment(anchor.segmentId);
		if (segment)
		{
			const vec3 tangent = safeNormalized(segment->tangentByDistance(anchor.distanceOnSegment), chordDirection);
			if (vec3::DotProduct(tangent, chordDirection) > 0.15f)
			{
				return tangent;
			}
		}
	}

	return chordDirection;
}

vec3 previewTangentVector(const vec3& direction, float length)
{
	return safeNormalized(direction, vec3(1.0f, 0.0f, 0.0f)) * std::max(length * 0.7f, 0.05f);
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
	return m_pendingAnchor.type != RailBuildAnchorType::None;
}

RailNodeId RailBuildTool::pendingNodeId() const
{
	if (m_pendingAnchor.type == RailBuildAnchorType::Node)
	{
		return m_pendingAnchor.nodeId;
	}
	return InvalidRailNodeId;
}

bool RailBuildTool::pendingAnchorPosition(vec3& outPosition) const
{
	if (!hasPendingAnchor())
	{
		return false;
	}
	outPosition = m_pendingAnchor.position;
	return true;
}

bool RailBuildTool::buildPreviewSegment(PlacementMode placementMode, RailSegment& outSegment) const
{
	outSegment = RailSegment();
	if (!m_network || !m_config || m_mode != RailBuildMode::Add || !hasPendingAnchor())
	{
		return false;
	}

	RailBuildAnchor endAnchor = pickAnchor(placementMode);
	if (endAnchor.type == RailBuildAnchorType::None)
	{
		return false;
	}

	if (m_pendingAnchor.type == RailBuildAnchorType::Segment
		&& endAnchor.type == RailBuildAnchorType::Segment
		&& m_pendingAnchor.segmentId == endAnchor.segmentId)
	{
		return false;
	}

	const float anchorDistance = m_pendingAnchor.position.distance(endAnchor.position);
	if (anchorDistance < m_config->minSegmentLength || anchorDistance > m_config->maxSegmentLength)
	{
		return false;
	}

	const vec3 anchorDelta = endAnchor.position - m_pendingAnchor.position;
	const float horizontalDistance = std::sqrt(anchorDelta.x * anchorDelta.x + anchorDelta.z * anchorDelta.z);
	if (horizontalDistance > 0.0001f && std::fabs(anchorDelta.y) / horizontalDistance > m_config->maxSlope)
	{
		return false;
	}

	if (!canResolveAnchorToNode(m_pendingAnchor) || !canResolveAnchorToNode(endAnchor))
	{
		return false;
	}

	const vec3 chordDirection = anchorDelta / anchorDistance;
	const vec3 startDirection = directionForPreviewAnchor(m_network, m_pendingAnchor, chordDirection);
	const vec3 endDirection = directionForPreviewAnchor(m_network, endAnchor, chordDirection);
	outSegment.startTangent = previewTangentVector(startDirection, anchorDistance);
	outSegment.endTangent = previewTangentVector(endDirection, anchorDistance);
	outSegment.spline.configure(m_pendingAnchor.position, endAnchor.position, outSegment.startTangent, outSegment.endTangent);
	outSegment.cachedLength = outSegment.spline.length();
	return outSegment.cachedLength > 0.0001f;
}

void RailBuildTool::clearPending()
{
	m_pendingAnchor = RailBuildAnchor();
}

bool RailBuildTool::handlePrimaryClick(PlacementMode placementMode)
{
	if (!m_network || !m_config)
	{
		return false;
	}

	RailBuildAnchor anchor = pickAnchor(placementMode);
	if (anchor.type == RailBuildAnchorType::None)
	{
		return false;
	}

	if (m_mode == RailBuildMode::Delete)
	{
		if (anchor.type == RailBuildAnchorType::Segment)
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
		RailNodeId endNodeId = InvalidRailNodeId;
		const bool didCommit = commitAdd(m_pendingAnchor, anchor, &endNodeId);
		if (didCommit && endNodeId != InvalidRailNodeId)
		{
			m_pendingAnchor.type = RailBuildAnchorType::Node;
			m_pendingAnchor.nodeId = endNodeId;
			m_pendingAnchor.segmentId = InvalidRailSegmentId;
			m_pendingAnchor.distanceOnSegment = 0.0f;
			const RailNode* railNode = m_network->node(endNodeId);
			m_pendingAnchor.position = railNode ? railNode->position : anchor.position;
		}
		return didCommit;
	}

	if (!canResolveAnchorToNode(anchor))
	{
		return false;
	}

	m_pendingAnchor = anchor;
	return false;
}

void RailBuildTool::handleSecondaryClick()
{
	if (m_mode == RailBuildMode::Add || m_mode == RailBuildMode::Delete)
	{
		clearPending();
	}
}

RailBuildAnchor RailBuildTool::pickAnchor(PlacementMode placementMode) const
{
	RailBuildAnchor result;
	if (!m_network || !m_config)
	{
		return result;
	}

	const vec3 terrainHit = BuildingSystem::shared()->hitTerrain(placementMode);
	if (!BuildingSystem::isValidHitPoint(terrainHit))
	{
		return result;
	}

	if (m_mode != RailBuildMode::Delete)
	{
		RailNodePick nodePick = m_network->findNearestNode(terrainHit, m_config->nodePickRadius);
		if (nodePick.nodeId != InvalidRailNodeId)
		{
			result.type = RailBuildAnchorType::Node;
			result.nodeId = nodePick.nodeId;
			const RailNode* railNode = m_network->node(nodePick.nodeId);
			result.position = railNode ? railNode->position : terrainHit;
			return result;
		}
	}

	RailSegmentPick segmentPick = m_network->findNearestSegment(terrainHit, m_config->segmentPickRadius);
	if (segmentPick.segmentId != InvalidRailSegmentId)
	{
		result.type = RailBuildAnchorType::Segment;
		result.segmentId = segmentPick.segmentId;
		result.distanceOnSegment = segmentPick.distanceOnSegment;
		result.position = segmentPick.position;
		return result;
	}

	if (m_mode == RailBuildMode::Delete)
	{
		return result;
	}

	result.type = RailBuildAnchorType::Terrain;
	result.position = terrainHit;
	return result;
}

bool RailBuildTool::commitAdd(const RailBuildAnchor& startAnchor, const RailBuildAnchor& endAnchor, RailNodeId* outEndNode)
{
	if (outEndNode)
	{
		*outEndNode = InvalidRailNodeId;
	}

	if (startAnchor.type == RailBuildAnchorType::Segment
		&& endAnchor.type == RailBuildAnchorType::Segment
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

	if (!canResolveAnchorToNode(startAnchor) || !canResolveAnchorToNode(endAnchor))
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

bool RailBuildTool::canResolveAnchorToNode(const RailBuildAnchor& anchor) const
{
	if (!m_network)
	{
		return false;
	}

	switch (anchor.type)
	{
	case RailBuildAnchorType::Node:
		return m_network->node(anchor.nodeId) != nullptr;
	case RailBuildAnchorType::Terrain:
		return BuildingSystem::isValidHitPoint(anchor.position);
	case RailBuildAnchorType::Segment:
		{
			const RailSegment* segment = m_network->segment(anchor.segmentId);
			if (!segment || segment->cachedLength <= 0.0001f)
			{
				return false;
			}
			return anchor.distanceOnSegment > 0.05f
				&& anchor.distanceOnSegment < segment->cachedLength - 0.05f;
		}
	default:
		break;
	}
	return false;
}

bool RailBuildTool::resolveAnchorToNode(const RailBuildAnchor& anchor, RailNodeId& outNodeId)
{
	outNodeId = InvalidRailNodeId;
	switch (anchor.type)
	{
	case RailBuildAnchorType::Node:
		outNodeId = anchor.nodeId;
		return outNodeId != InvalidRailNodeId;
	case RailBuildAnchorType::Terrain:
		outNodeId = m_network->createNode(anchor.position);
		return outNodeId != InvalidRailNodeId;
	case RailBuildAnchorType::Segment:
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
