#include "TerrainDrawableNode.h"
#include "../../Mesh/Mesh.h"
#include "../../Technique/Material.h"
#include "../../Rendering/RenderCommand.h"
#include "../../Rendering/RenderQueues.h"
#include "../../Base/Camera.h"
#include "../../Scene/Scene.h"
#include "../../Scene/SceneMgr.h"

namespace tzw {

TerrainDrawableNode::TerrainDrawableNode()
    : m_mesh(nullptr)
    , m_material(nullptr)
    , m_revision(0)
    , m_isBound(false)
{
}

void TerrainDrawableNode::bind(const TerrainNodeKey& key, const TerrainRegion& region,
    Mesh* mesh, Material* material, int revision)
{
    m_key = key;
    m_region = region;
    m_mesh = mesh;
    m_material = material;
    m_revision = revision;
    m_isBound = true;

    if (m_mesh && !m_mesh->isEmpty())
    {
        setLocalAABB(m_mesh->getAabb());
    }
    else
    {
        AABB aabb;
        aabb.update(region.worldMin(vec3(0.0f, 0.0f, 0.0f), 1.0f));
        aabb.update(region.worldMax(vec3(0.0f, 0.0f, 0.0f), 1.0f));
        setLocalAABB(aabb);
    }

    setPos(vec3(0.0f, 0.0f, 0.0f));
    reCache();
}

void TerrainDrawableNode::unbind()
{
    m_isBound = false;
    m_mesh = nullptr;
    m_material = nullptr;
    m_revision = 0;
    m_key = TerrainNodeKey();
    m_region = TerrainRegion();
}

bool TerrainDrawableNode::isBoundTo(const TerrainNodeKey& key, int revision) const
{
    return m_isBound && m_key == key && m_revision == revision;
}

void TerrainDrawableNode::setDebugWireframeEnabled(bool enabled)
{
    m_debugWireframeEnabled = enabled;
}

bool TerrainDrawableNode::isDebugWireframeEnabled() const
{
    return m_debugWireframeEnabled;
}

Mesh* TerrainDrawableNode::getMesh()
{
    return m_mesh;
}

Mesh* TerrainDrawableNode::getMesh(int index)
{
    if (index == 0)
        return m_mesh;
    return nullptr;
}

int TerrainDrawableNode::getMeshCount()
{
    return m_mesh ? 1 : 0;
}

bool TerrainDrawableNode::getIsAccpectOcTtree() const
{
    return m_isBound && m_mesh && !m_mesh->isEmpty();
}

void TerrainDrawableNode::submitDrawCmd(RenderFlag::RenderStage stage, RenderQueue* queues, int requirementArg)
{
    if (!m_isBound)
        return;
    if (stage != RenderFlag::RenderStage::COMMON)
        return;
    if (!m_mesh || m_mesh->isEmpty())
        return;
    if (!m_mesh->getIndexBuf() || m_mesh->getIndexBuf()->bufferId() == nullptr)
        return;
    if (!m_material)
        return;

    RenderCommand command(m_mesh, m_material, this, stage);
    setUpTransFormation(command.m_transInfo);
    command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
    if (m_debugWireframeEnabled)
    {
        command.addRenderStage(RenderFlag::RenderStage::DEBUG_LAYER);
    }
    queues->addRenderCommand(command, requirementArg);
}

void TerrainDrawableNode::setUpTransFormation(TransformationInfo& info)
{
    auto currCam = g_GetCurrScene()->defaultCamera();
    info.m_projectMatrix = currCam->projection();
    info.m_viewMatrix = currCam->getViewMatrix();
    Matrix44 mat;
    mat.setToIdentity();
    info.m_worldMatrix = mat;
}

} // namespace tzw
