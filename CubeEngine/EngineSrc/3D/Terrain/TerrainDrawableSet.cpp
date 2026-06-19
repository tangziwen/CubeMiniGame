#include "TerrainDrawableSet.h"

#include <unordered_set>

namespace tzw {

void TerrainDrawableSet::setMaterial(Material* material)
{
    m_material = material;
}

void TerrainDrawableSet::setDebugWireframeEnabled(bool enabled)
{
    m_debugWireframeEnabled = enabled;
    for (auto& pair : m_drawables)
    {
        if (pair.second)
        {
            pair.second->setDebugWireframeEnabled(enabled);
        }
    }
}

bool TerrainDrawableSet::isDebugWireframeEnabled() const
{
    return m_debugWireframeEnabled;
}

void TerrainDrawableSet::sync(const TerrainRenderSet& renderSet, const TerrainMeshCache& meshCache)
{
    for (TerrainOctreeNode* node : renderSet.nodes())
    {
        if (!node)
        {
            continue;
        }

        const TerrainNodeKey& key = node->key();
        const TerrainMeshCacheEntry* entry = meshCache.find(key);
        if (!entry || entry->state != TerrainMeshState::Ready)
        {
            continue;
        }

        Mesh* mesh = entry->mesh.get();
        if (!mesh || mesh->isEmpty())
        {
            continue;
        }

        auto iter = m_drawables.find(key);
        if (iter == m_drawables.end())
        {
            auto drawable = std::make_unique<TerrainDrawableNode>();
            drawable->bind(key, node->region(), mesh, m_material, entry->revision);
            drawable->setDebugWireframeEnabled(m_debugWireframeEnabled);
            m_drawables.emplace(key, std::move(drawable));
        }
        else
        {
            TerrainDrawableNode* drawable = iter->second.get();
            if (!drawable->isBoundTo(key, entry->revision))
            {
                drawable->bind(key, node->region(), mesh, m_material, entry->revision);
            }
            drawable->setDebugWireframeEnabled(m_debugWireframeEnabled);
        }
    }

    for (auto iter = m_drawables.begin(); iter != m_drawables.end();)
    {
        const TerrainNodeKey& key = iter->first;
        TerrainDrawableNode* drawable = iter->second.get();

		const bool isInRenderSet = renderSet.contains(key);
		const TerrainMeshCacheEntry* entry = meshCache.find(key);
		bool keep = false;
		if (isInRenderSet && entry && entry->state == TerrainMeshState::Ready && entry->mesh && !entry->mesh->isEmpty())
		{
			if (drawable->isBoundTo(key, entry->revision))
			{
				keep = true;
			}
        }

        if (!keep)
        {
            iter->second->removeFromParent();
            iter = m_drawables.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

TerrainDrawableNode* TerrainDrawableSet::find(const TerrainNodeKey& key)
{
    auto iter = m_drawables.find(key);
    if (iter != m_drawables.end())
    {
        return iter->second.get();
    }
    return nullptr;
}

void TerrainDrawableSet::releaseMissingNodes(const TerrainRenderSet& renderSet)
{
    std::unordered_set<TerrainNodeKey> activeKeys;
    for (TerrainOctreeNode* node : renderSet.nodes())
    {
        if (node)
        {
            activeKeys.insert(node->key());
        }
    }

    for (auto iter = m_drawables.begin(); iter != m_drawables.end();)
    {
        if (activeKeys.find(iter->first) == activeKeys.end())
        {
            iter->second->removeFromParent();
            iter = m_drawables.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void TerrainDrawableSet::clear()
{
    for (auto& pair : m_drawables)
    {
        pair.second->removeFromParent();
    }
    m_drawables.clear();
}

const std::unordered_map<TerrainNodeKey, std::unique_ptr<TerrainDrawableNode>>& TerrainDrawableSet::drawables() const
{
    return m_drawables;
}

} // namespace tzw
