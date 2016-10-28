#include "Grass.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include "EngineSrc/Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"

namespace tzw {
Grass::Grass(std::string filePath)
{
	auto mat = MaterialPool::shared()->getMaterialByName("grassMaterial");
	if (!mat)
	{
		mat = Material::createFromEffect("Grass");
		mat->setTex("diffuseMap", TextureMgr::shared()->getByPath(filePath));
	}
	setMaterial(mat);
//	setIsAccpectOCTtree(false);
	setCamera(g_GetCurrScene()->defaultCamera());
	initMesh();
}

void Grass::submitDrawCmd()
{
	RenderCommand command(m_mesh,m_material,RenderCommand::RenderType::Common);
	setUpTransFormation(command.m_transInfo);
	Renderer::shared()->addRenderCommand(command);
}

void Grass::initMesh()
{
	m_mesh = new Mesh();
	float halfWidth = 0.5;
	float halfHeight = 0.5;
	VertexData vertices[] = {
		// Vertex data for face 0
		{vec3(-1.0f *halfWidth, -1.0f * halfHeight, 0.0f), vec2(0.0f, 0.0f)},  // v0
		{vec3( 1.0f *halfWidth, -1.0f * halfHeight,  0.0f), vec2(1.0f, 0.0f)}, // v1
		{vec3(1.0f *halfWidth,  1.0f * halfHeight,  0.0f), vec2(1.0f, 1.0f)},  // v2
		{vec3( -1.0f *halfWidth,  1.0f * halfHeight,  0.0f), vec2(0.0f, 1.0f)}, // v3
	};

	unsigned short indices[] = {
		0,  1,  2,  0,  2,  3,
	};
	m_mesh->addVertices(vertices,sizeof(vertices)/sizeof(VertexData));
	m_mesh->addIndices(indices,sizeof(indices)/sizeof(unsigned short));
	m_mesh->finish();
	reCache();

	m_localAABB.merge(m_mesh->getAabb());
	reCache();
	reCacheAABB();
}

} // namespace tzw
