#include "RenderCommand.h"
#include "../Technique/MaterialInstance.h"

#include "../Mesh/InstancedMesh.h"
#include "../Mesh/VertexData.h"
namespace tzw {

RenderCommand::RenderCommand(Mesh *mesh, MaterialInstance *material, void * obj, DrawPassTypeMask drawPassMask, PrimitiveType primitiveType, RenderBatchType batchType)
    :m_mesh(mesh),m_material(material),
	m_primitiveType(primitiveType),m_Zorder(0),
    m_batchType(batchType),
	m_outlineColor(1.0f, 0.85f, 0.15f, 1.0f)
{
    m_obj = obj;
	m_drawPassMask = drawPassMask;
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

DrawPassTypeMask RenderCommand::getDrawPassMask() const
{
	return m_drawPassMask;
}

void RenderCommand::setDrawPassMask(DrawPassTypeMask drawPassMask)
{
	m_drawPassMask = drawPassMask;
}

void RenderCommand::addDrawPass(DrawPassTypeMask drawPassMask)
{
	m_drawPassMask |= drawPassMask;
}

void RenderCommand::removeDrawPass(DrawPassTypeMask drawPassMask)
{
	m_drawPassMask &= ~drawPassMask;
}

bool RenderCommand::hasDrawPass(DrawPassTypeMask drawPassMask) const
{
	return (m_drawPassMask & drawPassMask) != 0;
}

InstancedMesh* RenderCommand::getInstancedMesh() const
{
	return m_instancedMesh;
}

void RenderCommand::setInstancedMesh(InstancedMesh* const instancedMesh)
{
	m_instancedMesh = instancedMesh;
}
MaterialInstance* RenderCommand::getMat()
{
    return m_material;
}
void RenderCommand::setMat(MaterialInstance* newMat)
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

void RenderCommand::setOutlineColor(const vec4& outlineColor)
{
	m_outlineColor = outlineColor;
}

const vec4& RenderCommand::outlineColor() const
{
	return m_outlineColor;
}
} // namespace tzw

