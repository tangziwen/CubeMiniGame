#pragma once

#include "RailNetwork.h"

namespace tzw {

enum class PlacementMode;

enum class RailBuildMode
{
	None,
	Add,
	Delete,
};

class RailBuildTool
{
public:
	void bind(RailNetwork* network, const RailConfig* config);
	void setMode(RailBuildMode mode);
	RailBuildMode mode() const;
	bool hasPendingAnchor() const;
	void clearPending();
	bool handlePrimaryClick(PlacementMode placementMode);
	void handleSecondaryClick();
	RailNodeId pendingNodeId() const;

private:
	enum class AnchorType
	{
		None,
		Node,
		Segment,
		Terrain,
	};

	struct Anchor
	{
		AnchorType type = AnchorType::None;
		RailNodeId nodeId = InvalidRailNodeId;
		RailSegmentId segmentId = InvalidRailSegmentId;
		float distanceOnSegment = 0.0f;
		vec3 position;
	};

	Anchor pickAnchor(PlacementMode placementMode) const;
	bool commitAdd(const Anchor& startAnchor, const Anchor& endAnchor, RailNodeId* outEndNode = nullptr);
	bool resolveAnchorToNode(const Anchor& anchor, RailNodeId& outNodeId);

	RailNetwork* m_network = nullptr;
	const RailConfig* m_config = nullptr;
	RailBuildMode m_mode = RailBuildMode::None;
	Anchor m_pendingAnchor;
};

} // namespace tzw
