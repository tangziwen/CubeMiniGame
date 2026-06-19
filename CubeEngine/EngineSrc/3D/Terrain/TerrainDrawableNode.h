#pragma once

#include "../../Interface/Drawable3D.h"
#include "TerrainOctreeTypes.h"

namespace tzw {

class TerrainDrawableNode : public Drawable3D
{
public:
    TerrainDrawableNode();

    void bind(const TerrainNodeKey& key, const TerrainRegion& region,
        Mesh* mesh, Material* material, int revision);
    void unbind();
    bool isBoundTo(const TerrainNodeKey& key, int revision) const;

    Mesh* getMesh() override;
    Mesh* getMesh(int index) override;
    int getMeshCount() override;
    bool getIsAccpectOcTtree() const override;
    void submitDrawCmd(RenderFlag::RenderStage stage, RenderQueue* queues, int requirementArg) override;
    void setUpTransFormation(TransformationInfo& info) override;
    void setDebugWireframeEnabled(bool enabled);
    bool isDebugWireframeEnabled() const;

private:
    TerrainNodeKey m_key;
    TerrainRegion m_region;
    Mesh* m_mesh;
    Material* m_material;
    int m_revision;
    bool m_isBound;
    bool m_debugWireframeEnabled = false;
};

} // namespace tzw
