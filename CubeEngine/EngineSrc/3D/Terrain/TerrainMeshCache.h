#pragma once

#include "TerrainMeshRequest.h"
#include "TerrainOctreeNode.h"
#include "TerrainRenderSet.h"
#include "../../Mesh/Mesh.h"

#include <memory>
#include <unordered_map>

namespace tzw {

struct TerrainMeshCacheEntry
{
	TerrainMeshState state = TerrainMeshState::Empty;
	TerrainMeshRequest request;
	std::unique_ptr<Mesh> mesh;
	float lastTouchedTimeSeconds = -1.0f;
	int revision = 0;
};

class TerrainMeshCache
{
public:
	TerrainMeshCacheEntry* find(const TerrainNodeKey& key);
	const TerrainMeshCacheEntry* find(const TerrainNodeKey& key) const;
	TerrainMeshCacheEntry& ensure(const TerrainNodeKey& key);

	TerrainMeshRequest makeRequest(const TerrainOctreeNode& node, const TerrainOctreeConfig& config);
	TerrainMeshRequest makeRequest(const TerrainOctreeNode& node, const TerrainOctreeConfig& config,
		const TerrainMeshSeamSet& seams);
	bool hasFreshReadyMesh(const TerrainNodeKey& key, const TerrainMeshSeamSet& seams) const;
	bool hasMatchingRequest(const TerrainNodeKey& key, const TerrainMeshSeamSet& seams) const;
	void markRequested(const TerrainMeshRequest& request);
	bool storeReady(const TerrainMeshRequest& request, std::unique_ptr<Mesh> mesh);
	bool markFailed(const TerrainMeshRequest& request);
	void invalidate(const TerrainNodeKey& key);
	void invalidateInBounds(const TerrainEditBounds& bounds, const TerrainOctreeConfig& config);
	void touchRenderSet(const TerrainRenderSet& renderSet, float currentTimeSeconds);
	int finishReadyMeshesForRender(const TerrainRenderSet& renderSet);
	void evictUnused(const TerrainRenderSet& renderSet, const TerrainOctreeConfig& config,
		const vec3& viewerPosition, float currentTimeSeconds,
		float unloadDistance, float keepAliveSeconds);
	void clear();
	size_t size() const;

private:
	std::unordered_map<TerrainNodeKey, TerrainMeshCacheEntry> m_entries;
};

} // namespace tzw
