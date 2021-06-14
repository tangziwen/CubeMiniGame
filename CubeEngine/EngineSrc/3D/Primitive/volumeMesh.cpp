#include "volumeMesh.h"
#include "../../Scene/SceneMgr.h"

namespace tzw {
SimpleMesh::SimpleMesh(VertexData * vertices, uint32_t verticesSize, const uint32_t * indices, uint32_t indicesSize)
{
	m_mesh = new Mesh();
	m_mesh->addVertices(vertices, verticesSize);
	for(int i = 0; i < indicesSize; i++)
	{
		m_mesh->addIndex(indices[i]);
	}
	m_mesh->finish();
	m_material = Material::createFromTemplate("Color");
	setCamera(g_GetCurrScene()->defaultCamera());
	reCache();
	setIsAccpectOcTtree(false);
}

void SimpleMesh::submitDrawCmd(RenderFlag::RenderStage stageType, RenderQueue * queues, int requirementArg)
{
	RenderCommand command(m_mesh,m_material,this, stageType);
    setUpTransFormation(command.m_transInfo);
	command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
	queues->addRenderCommand(command, requirementArg);
}

bool SimpleMesh::getIsAccpectOcTtree() const
{
	return false;
}

} // namespace tzw
