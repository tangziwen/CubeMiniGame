#include "EngineSrc/Technique/MaterialPool.h"
#include "EngineSrc/Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "EngineSrc/3D/Vegetation/Tree.h"

#include "3D/Model/Model.h"

namespace tzw {
Tree::Tree(std::string filePath)
{
	 
	m_isFinish = false;
	std::string materialName = "Tree";
	materialName +=filePath;
	auto mat = MaterialPool::shared()->getMaterialByName(materialName);
	if (!mat)
	{
	
		mat = Material::createFromTemplate("Tree");
		auto tex = TextureMgr::shared()->getByPath("Models/tree/bark.jpg");
		 
		tex->genMipMap();
		 
		mat->setTex("diffuseMap", tex);
		MaterialPool::shared()->addMaterial(materialName, mat);
	}
	setMaterial(mat);
	 
	setIsAccpectOcTtree(false);
	 
	setCamera(g_GetCurrScene()->defaultCamera());
	 
	initMesh();
	 
}


void Tree::setUpTransFormation(TransformationInfo &info)
{
	auto currCam = g_GetCurrScene()->defaultCamera();
	info.m_projectMatrix = currCam->projection();
	info.m_viewMatrix = currCam->getViewMatrix();
	Matrix44 mat;
	mat.setToIdentity();
	info.m_worldMatrix = mat;
}

unsigned int Tree::getTypeId()
{
	return 2333;
}

void Tree::pushCommand()
{
	if (! m_isFinish) return;
	RenderCommand command(m_mesh, getMaterial(), RenderCommand::RenderType::Instanced);
	command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
	setUpTransFormation(command.m_transInfo);
	Renderer::shared()->addRenderCommand(command);
	setUpTransFormation(command.m_transInfo);
}

void Tree::finish()
{

	// m_mesh->finish();
	m_mesh->reSubmitInstanced();
	m_isFinish = true;
}

void Tree::submitDrawCmd(RenderCommand::RenderType passType)
{
	RenderCommand command(m_mesh, m_material, RenderCommand::RenderType::Common);
	setUpTransFormation(command.m_transInfo);
	Renderer::shared()->addRenderCommand(command);
}

void Tree::initMesh()
{
	auto treeModel = Model::create("Models/tree/tree.tzw", true);
	treeModel->getMat(0)->setTex("DiffuseMap",
								TextureMgr::shared()->getByPath("Models/tree/bark.jpg", true));
	treeModel->getMat(1)->setTex("DiffuseMap",
								TextureMgr::shared()->getByPath("Models/tree/twig.png", true));
	m_mesh = treeModel->getMesh(0);
}

} // namespace tzw
