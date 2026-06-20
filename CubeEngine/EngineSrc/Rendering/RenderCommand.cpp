#include "RenderCommand.h"
#include "../Technique/Material.h"

#include "../Mesh/InstancedMesh.h"
#include "../Mesh/VertexData.h"
namespace tzw {

RenderCommand::RenderCommand(Mesh *mesh, Material *material, void * obj,RenderFlag::RenderStage renderStage, PrimitiveType primitiveType, RenderBatchType batchType)
    :m_mesh(mesh),m_material(material),
	m_primitiveType(primitiveType),m_Zorder(0),
    m_batchType(batchType)
{
    m_obj = obj;
	m_renderStageMask = static_cast<uint32_t>(renderStage);
}

void RenderCommand::render()
{



}

RenderCommand::RenderBatchType RenderCommand::batchType() const
{
	return m_batchType;
}

void RenderCommand::setBatchType(const RenderBatchType& newBatchType)
{
	m_batchType = newBatchType;
}

unsigned int RenderCommand::Zorder() const
{
    return m_Zorder;
}

void RenderCommand::setZorder(unsigned int Zorder)
{
    m_Zorder = Zorder;
}
RenderCommand::PrimitiveType RenderCommand::primitiveType() const
{
    return m_primitiveType;
}

void RenderCommand::setPrimitiveType(const PrimitiveType &primitiveType)
{
    m_primitiveType = primitiveType;
}

DepthPolicy RenderCommand::depthTestPolicy() const
{
    return m_depthTestPolicy;
}

void RenderCommand::setDepthTestPolicy(const DepthPolicy &depthTestPolicy)
{
    m_depthTestPolicy = depthTestPolicy;
}

uint32_t RenderCommand::getRenderStageMask() const
{
	return m_renderStageMask;
}

void RenderCommand::setRenderStageMask(uint32_t renderStageMask)
{
	m_renderStageMask = renderStageMask;
}

void RenderCommand::addRenderStage(RenderFlag::RenderStage renderStage)
{
	m_renderStageMask |= static_cast<uint32_t>(renderStage);
}

void RenderCommand::removeRenderStage(RenderFlag::RenderStage renderStage)
{
	m_renderStageMask &= ~static_cast<uint32_t>(renderStage);
}

bool RenderCommand::hasRenderStage(RenderFlag::RenderStage renderStage) const
{
	return (m_renderStageMask & static_cast<uint32_t>(renderStage)) != 0;
}

InstancedMesh* RenderCommand::getInstancedMesh() const
{
	return m_instancedMesh;
}

void RenderCommand::setInstancedMesh(InstancedMesh* const instancedMesh)
{
	m_instancedMesh = instancedMesh;
}
Material* RenderCommand::getMat()
{
    return m_material;
}
void RenderCommand::setMat(Material* newMat)
{
    m_material = newMat;
}
Mesh* RenderCommand::getMesh()
{
    return m_mesh;
}
void* RenderCommand::getDrawableObj()
{
    return m_obj;
}
} // namespace tzw

