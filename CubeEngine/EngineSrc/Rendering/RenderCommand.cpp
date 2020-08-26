#include "RenderCommand.h"
#include "../Technique/Material.h"

#include "../Mesh/InstancedMesh.h"
#include "../Mesh/VertexData.h"
namespace tzw {

RenderCommand::RenderCommand(Mesh *mesh, Material *material, void * obj,RenderFlag::RenderStage renderStage, PrimitiveType primitiveType, RenderBatchType batchType)
    :m_mesh(mesh),m_material(material),
	m_primitiveType(primitiveType),m_Zorder(0),
	m_renderState(renderStage),
m_batchType(batchType)
{
    m_obj = obj;
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

RenderFlag::RenderStage RenderCommand::getRenderState() const
{
	return m_renderState;
}

void RenderCommand::setRenderState(const RenderFlag::RenderStage renderState)
{
	m_renderState = renderState;
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

