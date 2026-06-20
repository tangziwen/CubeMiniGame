#pragma once

#include "TerrainDrawableNode.h"
#include "TerrainRenderSet.h"
#include "TerrainMeshCache.h"

#include <unordered_map>
#include <memory>

namespace tzw {

class TerrainDrawableSet
{
public:
    void setMaterial(MaterialInstance* material);
    void sync(const TerrainRenderSet& renderSet, const TerrainMeshCache& meshCache);
    void setDebugWireframeEnabled(bool enabled);
    bool isDebugWireframeEnabled() const;
    TerrainDrawableNode* find(const TerrainNodeKey& key);
    void releaseMissingNodes(const TerrainRenderSet& renderSet);
    void clear();

    const std::unordered_map<TerrainNodeKey, std::unique_ptr<TerrainDrawableNode>>& drawables() const;

private:
    std::unordered_map<TerrainNodeKey, std::unique_ptr<TerrainDrawableNode>> m_drawables;
    MaterialInstance* m_material = nullptr;
    bool m_debugWireframeEnabled = false;
};

} // namespace tzw
