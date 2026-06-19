#pragma once

#include "TerrainOctreeTypes.h"
#include <memory>

namespace tzw {

class TerrainOctree;
class TerrainMeshCache;
class TerrainSampler;
class TerrainMesher;
class TerrainDrawableSet;
class TerrainEditSystem;
class Node;
class vec3;

class TerrainRuntime
{
public:
	TerrainRuntime();
	~TerrainRuntime();

	void init(const TerrainOctreeConfig& config);
	void update(const vec3& viewerPosition, Node* sceneRoot);
	void clear();

	TerrainOctree* octree() const;
	TerrainMeshCache* meshCache() const;
	TerrainSampler* sampler() const;
	TerrainMesher* mesher() const;
	TerrainDrawableSet* drawableSet() const;
	TerrainEditSystem* editSystem() const;
	bool isActive() const;
	void setDebugLodVertexColorEnabled(bool enabled);
	bool isDebugLodVertexColorEnabled() const;
	void setDebugLodCheckerboardEnabled(bool enabled);
	bool isDebugLodCheckerboardEnabled() const;
	void setDebugWireframeEnabled(bool enabled);
	bool isDebugWireframeEnabled() const;
	void setLodViewerPositionFreezeEnabled(bool enabled);
	void freezeLodViewerPosition(const vec3& viewerPosition);
	void unfreezeLodViewerPosition();
	bool isLodViewerPositionFrozen() const;
	const vec3& frozenLodViewerPosition() const;

private:
	std::unique_ptr<TerrainOctree> m_octree;
	std::unique_ptr<TerrainMeshCache> m_meshCache;
	std::unique_ptr<TerrainSampler> m_sampler;
	std::unique_ptr<TerrainMesher> m_mesher;
	std::unique_ptr<TerrainDrawableSet> m_drawableSet;
	std::unique_ptr<TerrainEditSystem> m_editSystem;
	int m_frameIndex = 0;
	bool m_debugLodVertexColorEnabled = true;
	bool m_debugLodCheckerboardEnabled = true;
	bool m_debugWireframeEnabled = false;
	bool m_lodViewerPositionFrozen = false;
	bool m_hasFrozenLodViewerPosition = false;
	vec3 m_frozenLodViewerPosition;
};

} // namespace tzw
