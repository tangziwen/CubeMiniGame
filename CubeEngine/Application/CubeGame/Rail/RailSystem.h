#pragma once

#include "RailBuildTool.h"
#include "RailTrackMesh.h"

#include <vector>

namespace tzw {

class CubePrimitive;
class LinePrimitive;
class Node;

class RailSystem
{
public:
	RailSystem();
	~RailSystem();

	void init();
	void update(Node* sceneRoot, float deltaSeconds);
	void clear();

	void setBuildMode(RailBuildMode mode);
	RailBuildMode buildMode() const;
	bool handlePrimaryClick(PlacementMode placementMode);

	RailNetwork& network();
	const RailNetwork& network() const;

private:
	void markVisualDirty();
	void syncVisuals(Node* sceneRoot);
	void ensureVisualRoot(Node* sceneRoot);
	void clearVisuals();

	RailConfig m_config;
	RailNetwork m_network;
	RailBuildTool m_buildTool;
	RailTrackMesh m_trackMesh;
	Node* m_visualRoot = nullptr;
	LinePrimitive* m_lineVisual = nullptr;
	std::vector<CubePrimitive*> m_sleeperVisuals;
	bool m_visualDirty = true;
};

} // namespace tzw
