#include "EngineSrc/Technique/MaterialPool.h"
#include "EngineSrc/Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "EngineSrc/3D/Vegetation/Tree.h"

#include "3D/Model/Model.h"

namespace tzw {
Tree::Tree()
{
	 
	m_isFinish = false;
	std::string materialName = "Tree";

	m_barkMat = Material::createFromTemplate("ModelInstance");
	auto tex = TextureMgr::shared()->getByPath("Models/tree/bark.jpg");
	tex->genMipMap();
	m_barkMat->setTex("DiffuseMap", tex);

	
	m_leafMat = Material::createFromTemplate("Tree");
	auto leafTex = TextureMgr::shared()->getByPath("Models/tree/twig.png", true);
	m_leafMat->setTex("DiffuseMap", leafTex);

	setIsAccpectOcTtree(false);
	 
	setCamera(g_GetCurrScene()->defaultCamera());
	 
	initMesh();
	 
}

void Tree::addTreeGroup(TreeGroup* treeGroup)
{
	m_tree.insert(treeGroup);
}

void Tree::clearTreeGroup()
{
	m_tree.clear();
	m_mesh->clearInstances();
	m_leafMesh->clearInstances();
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
	//regroup
	for(auto iter : m_tree)
	{
		// std::copy(iter->m_instance.begin(), iter->m_instance.end(), m_mesh->m_instanceOffset.begin());
		auto &tg = iter;
		for(auto inst : tg->m_instance)
		{
			m_mesh->pushInstance(inst);
			m_leafMesh->pushInstance(inst);
		}
	}
	if (! m_isFinish)
	{
		finish();
	}else
	{
		auto targetAmount = m_mesh->m_instanceOffset.size();
		if (targetAmount > 0)
		{
			// if(targetAmount > m_mesh->getInstanceBuf()->getAmount())
			// {
			// 	m_mesh->submitInstanced(targetAmount);
			// }else
			// {
			// 	m_mesh->reSubmitInstanced();
			// }
			m_mesh->submitInstanced();
			m_leafMesh->submitInstanced();
			// m_mesh->getInstanceBuf()->use();
			// m_mesh->getInstanceBuf()->allocate(&m_mesh->m_instanceOffset[0], m_mesh->m_instanceOffset.size() * sizeof(InstanceData), RenderFlag::BufferStorageType::DYNAMIC_DRAW);
		}
		// m_mesh->reSubmitInstanced();
	}
	if(m_mesh->m_instanceOffset.size() > 0)
	{
		RenderCommand command(m_mesh, m_barkMat, RenderCommand::RenderType::Instanced);
		command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
		setUpTransFormation(command.m_transInfo);
		Renderer::shared()->addRenderCommand(command);
		setUpTransFormation(command.m_transInfo);

		RenderCommand commandLeaf(m_leafMesh,  m_leafMat, RenderCommand::RenderType::Instanced);
		commandLeaf.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
		setUpTransFormation(commandLeaf.m_transInfo);
		Renderer::shared()->addRenderCommand(commandLeaf);
		setUpTransFormation(commandLeaf.m_transInfo);
	}

}

void Tree::finish()
{

	m_mesh->finish();
	m_leafMesh->finish();
	// tlog("instance size %d", m_mesh->getInstanceSize());
	// m_mesh->reSubmitInstanced();
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
	auto treeModel = Model::create("Models/tree/tree.tzw", false);

	
	m_barkMat = treeModel->getMat(0);
	m_barkMat->setIsEnableInstanced(true);

	m_barkMat->setTex("DiffuseMap",TextureMgr::shared()->getByPath("Models/tree/bark.jpg", true));
	m_barkMat->reload();
	
	
	m_leafMat = treeModel->getMat(1);
	m_leafMat->setIsEnableInstanced(true);
	m_barkMat->setIsCullFace(false);
	m_leafMat->reload();
	m_leafMat->setTex("DiffuseMap",TextureMgr::shared()->getByPath("Models/tree/twig.png", true));

	m_mesh = treeModel->getMesh(0);
	m_leafMesh = treeModel->getMesh(1);
}

} // namespace tzw
