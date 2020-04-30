#include "Grass.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include "EngineSrc/Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "EngineSrc/3D/Vegetation/Grass.h"
namespace tzw {
Grass::Grass(std::string filePath)
{
	 
	m_isFinish = false;
	std::string materialName = "grass";
	materialName +=filePath;
	auto mat = MaterialPool::shared()->getMaterialByName(materialName);
	if (!mat)
	{

		mat = Material::createFromTemplate("Grass");
		 
		auto tex = TextureMgr::shared()->getByPath(filePath);
		 
		tex->genMipMap();
		 
		mat->setTex("diffuseMap", tex);
		MaterialPool::shared()->addMaterial(materialName, mat);
	}
	setMaterial(mat);
	 
	setIsAccpectOcTtree(false);
	 
	setCamera(g_GetCurrScene()->defaultCamera());
	 
	initMesh();
	 
}


void Grass::setUpTransFormation(TransformationInfo &info)
{
	auto currCam = g_GetCurrScene()->defaultCamera();
	info.m_projectMatrix = currCam->projection();
	info.m_viewMatrix = currCam->getViewMatrix();
	Matrix44 mat;
	mat.setToIdentity();
	info.m_worldMatrix = mat;
}

unsigned int Grass::getTypeId()
{
	return 2333;
}

void Grass::pushCommand()
{
	if (! m_isFinish) return;
	RenderCommand command(m_mesh, getMaterial(), RenderCommand::RenderType::Instanced);
	command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
	setUpTransFormation(command.m_transInfo);
	Renderer::shared()->addRenderCommand(command);
	setUpTransFormation(command.m_transInfo);
}

void Grass::finish()
{

	m_mesh->finish();
	m_isFinish = true;
}

void Grass::submitDrawCmd(RenderCommand::RenderType passType)
{
	RenderCommand command(m_mesh, m_material, RenderCommand::RenderType::Common);
	setUpTransFormation(command.m_transInfo);
	Renderer::shared()->addRenderCommand(command);
}

void Grass::initMesh()
{
	m_mesh = new Mesh();
	float halfWidth = 0.8;
	float halfHeight = 0.8;
	VertexData vertices[] = {
		//#1
		VertexData(vec3(-1.0f *halfWidth, -1.0f * halfHeight, -0.5f), vec2(0.0f, 1.0f)), 
		VertexData(vec3( 1.0f *halfWidth, -1.0f * halfHeight,  -0.5f), vec2(1.0f, 1.0f)),
		VertexData(vec3(1.0f *halfWidth,  1.0f * halfHeight,  -0.5f), vec2(1.0f, 0.0f)), 
		VertexData(vec3( -1.0f *halfWidth,  1.0f * halfHeight,  -0.5f), vec2(0.0f, 0.0f)),

		//#2
		VertexData(vec3(-0.707f *halfWidth - 0.353, -1.0f * halfHeight, -0.707f *halfWidth + 0.353), vec2(0.0f, 1.0f)),
		VertexData(vec3(0.707f *halfWidth - 0.353, 1.0f * halfHeight,  0.707f * halfWidth + 0.353), vec2(1.0f, 0.0f)),
		VertexData(vec3(-0.707f *halfWidth - 0.353,  1.0f * halfHeight,  -0.707f * halfWidth + 0.353), vec2(0.0f, 0.0f)),
		VertexData(vec3(0.707f *halfWidth - 0.353,  -1.0f * halfHeight,  0.707f * halfWidth + 0.353), vec2(1.0f, 1.0f)),

		//#3
		VertexData(vec3(-0.707f *halfWidth + 0.353, -1.0f * halfHeight, 0.707f *halfWidth + 0.353), vec2(0.0f, 1.0f)),
		VertexData(vec3(0.707f *halfWidth + 0.353, 1.0f * halfHeight,  -0.707f * halfWidth + 0.353), vec2(1.0f, 0.0f)),
		VertexData(vec3(-0.707f *halfWidth + 0.353,  1.0f * halfHeight,  0.707f * halfWidth + 0.353), vec2(0.0f, 0.0f)),
		VertexData(vec3(0.707f *halfWidth + 0.353,  -1.0f * halfHeight,  -0.707f * halfWidth + 0.353), vec2(1.0f, 1.0f)),

	};

	unsigned short indices[] = {
		0, 1, 2, 0, 2, 3,     4, 5, 6, 4, 7, 5,   8, 9, 10, 8, 11, 9,
	};
	m_mesh->addVertices(vertices,sizeof(vertices)/sizeof(VertexData));
	m_mesh->addIndices(indices,sizeof(indices)/sizeof(unsigned short));
	m_mesh->finish(false);//don't store vbo
	reCache();

	m_localAABB.merge(m_mesh->getAabb());
	reCache();
	reCacheAABB();
}

} // namespace tzw
