#pragma once

#include "RailNetwork.h"
#include "RailPoint.h"

namespace tzw {

enum class PlacementMode;

enum class RailBuildMode
{
	None,
	Add,
	Delete,
};

enum class RailBuildAnchorType
{
	None,
	Node,
	Segment,
	Terrain,
};

struct RailBuildAnchor
{
	RailBuildAnchorType type = RailBuildAnchorType::None;
	RailNodeId nodeId = InvalidRailNodeId;
	RailSegmentId segmentId = InvalidRailSegmentId;
	float distanceOnSegment = 0.0f;
	vec3 position;
};

class RailBuildTool
{
public:
	void bind(RailNetwork* network, RailAnchorManager* anchorManager, const RailConfig* config);
	void setMode(RailBuildMode mode);
	RailBuildMode mode() const;
	bool hasPendingAnchor() const;
	void clearPending();
	bool handlePrimaryClick(PlacementMode placementMode);
	void handleSecondaryClick();
	bool pickSegment(PlacementMode placementMode, RailSegmentId& outSegmentId) const;
	RailNodeId pendingNodeId() const;
	bool pendingAnchorPosition(vec3& outPosition) const;
	bool buildPreviewSegment(PlacementMode placementMode, RailSegment& outSegment) const;

private:
	RailBuildAnchor pickAnchor(PlacementMode placementMode) const;
	bool commitAdd(const RailBuildAnchor& startAnchor, const RailBuildAnchor& endAnchor, RailNodeId* outEndNode = nullptr);
	bool canResolveAnchorToNode(const RailBuildAnchor& anchor) const;
	bool resolveAnchorToNode(const RailBuildAnchor& anchor, RailNodeId& outNodeId);

	RailNetwork* m_network = nullptr;
	RailAnchorManager* m_anchorManager = nullptr;
	const RailConfig* m_config = nullptr;
	RailBuildMode m_mode = RailBuildMode::None;
	RailBuildAnchor m_pendingAnchor;
};

} // namespace tzw
