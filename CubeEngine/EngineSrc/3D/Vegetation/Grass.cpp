#include "Grass.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include "EngineSrc/Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "EngineSrc/3D/Vegetation/Grass.h"

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
	setIsAccpectOCTtree(false);
	setCamera(g_GetCurrScene()->defaultCamera());
	initMesh();
}

unsigned int Grass::getTypeID()
{
	return 2333;
}

void Grass::submitDrawCmd()
{
	RenderCommand command(m_mesh, m_material, RenderCommand::RenderType::Common);
	setUpTransFormation(command.m_transInfo);
	Renderer::shared()->addRenderCommand(command);
}

void Grass::initMesh()
{
	m_mesh = new Mesh();
	float halfWidth = 0.5;
	float halfHeight = 0.5;
	VertexData vertices[] = {
		//#1
		VertexData(vec3(-1.0f *halfWidth, -1.0f * halfHeight, 0.0f), vec2(0.0f, 0.0f)), 
		VertexData(vec3( 1.0f *halfWidth, -1.0f * halfHeight,  0.0f), vec2(1.0f, 0.0f)),
		VertexData(vec3(1.0f *halfWidth,  1.0f * halfHeight,  0.0f), vec2(1.0f, 1.0f)), 
		VertexData(vec3( -1.0f *halfWidth,  1.0f * halfHeight,  0.0f), vec2(0.0f, 1.0f)),
		//#2
		VertexData(vec3(-0.707f *halfWidth, -1.0f * halfHeight, -0.707f *halfWidth), vec2(0.0f, 0.0f)),
		VertexData(vec3(0.707f *halfWidth, 1.0f * halfHeight,  0.707f * halfWidth), vec2(1.0f, 1.0f)),
		VertexData(vec3(-0.707f *halfWidth,  1.0f * halfHeight,  -0.707f * halfWidth), vec2(0.0f, 1.0f)),
		VertexData(vec3(0.707f *halfWidth,  -1.0f * halfHeight,  0.707f * halfWidth), vec2(1.0f, 0.0f)),
		//#3
		VertexData(vec3(-0.707f *halfWidth, -1.0f * halfHeight, 0.707f *halfWidth), vec2(0.0f, 0.0f)),
		VertexData(vec3(0.707f *halfWidth, 1.0f * halfHeight,  -0.707f * halfWidth), vec2(1.0f, 1.0f)),
		VertexData(vec3(-0.707f *halfWidth,  1.0f * halfHeight,  0.707f * halfWidth), vec2(0.0f, 1.0f)),
		VertexData(vec3(0.707f *halfWidth,  -1.0f * halfHeight,  -0.707f * halfWidth), vec2(1.0f, 0.0f)),
		//#4
		VertexData(vec3(0.0f, -1.0f * halfHeight, -1.0f *halfWidth), vec2(0.0f, 0.0f)),
		VertexData(vec3(0.0f, -1.0f * halfHeight,  1.0f *halfWidth), vec2(1.0f, 0.0f)),
		VertexData(vec3(0.0f,  1.0f * halfHeight,  1.0f *halfWidth), vec2(1.0f, 1.0f)),
		VertexData(vec3(0.0f,  1.0f * halfHeight,  -1.0f *halfWidth), vec2(0.0f, 1.0f)),

	};

	unsigned short indices[] = {
		0, 1, 2, 0, 2, 3,     4, 5, 6, 4, 7, 5,   8, 9, 10, 8, 11, 9,   12, 13, 14, 12, 14, 15
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
