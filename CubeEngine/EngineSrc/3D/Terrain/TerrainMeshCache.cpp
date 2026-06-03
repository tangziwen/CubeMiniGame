#include "TerrainMeshCache.h"

namespace tzw {

TerrainMeshCacheEntry* TerrainMeshCache::find(const TerrainNodeKey& key)
{
	auto result = m_entries.find(key);
	return result == m_entries.end() ? nullptr : &result->second;
}

const TerrainMeshCacheEntry* TerrainMeshCache::find(const TerrainNodeKey& key) const
{
	auto result = m_entries.find(key);
	return result == m_entries.end() ? nullptr : &result->second;
}

TerrainMeshCacheEntry& TerrainMeshCache::ensure(const TerrainNodeKey& key)
{
	return m_entries[key];
}

TerrainMeshRequest TerrainMeshCache::makeRequest(const TerrainOctreeNode& node,
	const TerrainOctreeConfig& config)
{
	TerrainMeshCacheEntry& entry = ensure(node.key());
	return makeTerrainMeshRequest(node, config, entry.revision + 1);
}

void TerrainMeshCache::markRequested(const TerrainMeshRequest& request)
{
	TerrainMeshCacheEntry& entry = ensure(request.key);
	entry.state = TerrainMeshState::Requested;
	entry.request = request;
	entry.revision = request.revision;
}

bool TerrainMeshCache::storeReady(const TerrainMeshRequest& request, std::unique_ptr<Mesh> mesh)
{
	TerrainMeshCacheEntry* entry = find(request.key);
	if (!entry || entry->revision != request.revision)
	{
		return false;
	}

	entry->state = TerrainMeshState::Ready;
	entry->request = request;
	entry->mesh = std::move(mesh);
	return true;
}

bool TerrainMeshCache::markFailed(const TerrainMeshRequest& request)
{
	TerrainMeshCacheEntry* entry = find(request.key);
	if (!entry || entry->revision != request.revision)
	{
		return false;
	}

	entry->state = TerrainMeshState::Failed;
	entry->request = request;
	entry->mesh.reset();
	return true;
}

void TerrainMeshCache::invalidate(const TerrainNodeKey& key)
{
	TerrainMeshCacheEntry& entry = ensure(key);
	entry.state = TerrainMeshState::Empty;
	entry.mesh.reset();
	++entry.revision;
	entry.request.revision = entry.revision;
}

void TerrainMeshCache::touchRenderSet(const TerrainRenderSet& renderSet, int frameIndex)
{
	for (TerrainOctreeNode* node : renderSet.nodes())
	{
		if (!node)
		{
			continue;
		}
		ensure(node->key()).lastTouchedFrame = frameIndex;
	}
}

void TerrainMeshCache::evictUnused(int currentFrame, int keepAliveFrames)
{
	for (auto iter = m_entries.begin(); iter != m_entries.end();)
	{
		const TerrainMeshCacheEntry& entry = iter->second;
		const bool canEvict = entry.state != TerrainMeshState::Requested
			&& entry.lastTouchedFrame >= 0
			&& currentFrame - entry.lastTouchedFrame > keepAliveFrames;
		if (canEvict)
		{
			iter = m_entries.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void TerrainMeshCache::clear()
{
	m_entries.clear();
}

size_t TerrainMeshCache::size() const
{
	return m_entries.size();
}

} // namespace tzw
