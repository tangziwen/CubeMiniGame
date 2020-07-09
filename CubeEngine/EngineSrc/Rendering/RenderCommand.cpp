#include "RenderCommand.h"
#include "../Mesh/VertexData.h"
namespace tzw {

RenderCommand::RenderCommand(Mesh *mesh, Material *material, RenderType type, PrimitiveType primitiveType, RenderBatchType batchType)
    :m_mesh(mesh),m_material(material),m_type(type),
	m_primitiveType(primitiveType),m_Zorder(0),
	m_renderState(material->getRenderStage()),
m_batchType(batchType)
{

}

void RenderCommand::render()
{



}
RenderCommand::RenderType RenderCommand::type() const
{
    return m_type;
}

void RenderCommand::setType(const RenderType &type)
{
    m_type = type;
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
} // namespace tzw

