#include "volumeMesh.h"
#include "../../Rendering/Renderer.h"
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
	setIsAccpectOCTtree(false);
}

void SimpleMesh::submitDrawCmd()
{
	RenderCommand command(m_mesh,m_material,RenderCommand::RenderType::Common);
    setUpTransFormation(command.m_transInfo);
	command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
	Renderer::shared()->addRenderCommand(command);
}

bool SimpleMesh::getIsAccpectOCTtree() const
{
	return false;
}

} // namespace tzw
