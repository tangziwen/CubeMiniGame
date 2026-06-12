#include "TerrainMeshCache.h"
#include "CubeGame/GameConfig.h"

#include <algorithm>

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

void TerrainMeshCache::invalidateInBounds(const TerrainEditBounds& bounds, const TerrainOctreeConfig& config)
{
	if (!config.isValid())
	{
		return;
	}
	for (auto& pair : m_entries)
	{
		const TerrainNodeKey& key = pair.first;
		TerrainMeshCacheEntry& entry = pair.second;
		if (entry.state == TerrainMeshState::Empty)
		{
			continue;
		}

		const TerrainRegion region = config.regionForKey(key);
		const int stride = region.sampleStride(config.meshCellCount);
		const int s = std::max(stride, 1);
		const TerrainInt3 cellMax = region.cellMaxExclusive();
		const TerrainInt3 paddedMin(
			region.voxelMin.x - MIN_PADDING * s,
			region.voxelMin.y - MIN_PADDING * s,
			region.voxelMin.z - MIN_PADDING * s);
		const TerrainInt3 paddedMax(
			cellMax.x + MAX_PADDING * s,
			cellMax.y + MAX_PADDING * s,
			cellMax.z + MAX_PADDING * s);

		if (bounds.voxelMaxExclusive.x <= paddedMin.x || bounds.voxelMin.x >= paddedMax.x
			|| bounds.voxelMaxExclusive.y <= paddedMin.y || bounds.voxelMin.y >= paddedMax.y
			|| bounds.voxelMaxExclusive.z <= paddedMin.z || bounds.voxelMin.z >= paddedMax.z)
		{
			continue;
		}

		entry.state = TerrainMeshState::Empty;
		entry.mesh.reset();
		++entry.revision;
		entry.request.revision = entry.revision;
	}
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

int TerrainMeshCache::finishReadyMeshesForRender(const TerrainRenderSet& renderSet)
{
	int finishedCount = 0;
	for (TerrainOctreeNode* node : renderSet.nodes())
	{
		if (!node)
		{
			continue;
		}

		TerrainMeshCacheEntry* entry = find(node->key());
		if (!entry || entry->state != TerrainMeshState::Ready || !entry->mesh || entry->mesh->isEmpty())
		{
			continue;
		}

		RenderBuffer* indexBuffer = entry->mesh->getIndexBuf();
		RenderBuffer* arrayBuffer = entry->mesh->getArrayBuf();
		if (indexBuffer && indexBuffer->bufferId() && arrayBuffer && arrayBuffer->bufferId())
		{
			continue;
		}

		entry->mesh->finish();
		++finishedCount;
	}
	return finishedCount;
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
