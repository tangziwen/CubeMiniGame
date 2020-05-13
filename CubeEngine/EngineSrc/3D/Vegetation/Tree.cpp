#include "EngineSrc/Technique/MaterialPool.h"
#include "EngineSrc/Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "EngineSrc/3D/Vegetation/Tree.h"

#include "3D/Model/Model.h"

namespace tzw {
VegetationBatInfo::VegetationBatInfo()
{
}

VegetationBatInfo::VegetationBatInfo(VegetationType theType, std::string filePath)
{
	m_type = theType;
	file_path = filePath;
}

VegetationBatch::VegetationBatch(const VegetationBatInfo * info)
{
	m_type = info->m_type;
	auto filePath = info->file_path;
	m_info = (*info);
	switch (m_type)
	{
	case VegetationType::QUAD_TRI:
		{
			auto theMesh = new Mesh();
			float halfWidth = 0.8;
			float halfHeight = 0.8;
			VertexData vertices[] = {
				//#1
				VertexData(vec3(-1.0f *halfWidth, -1.0f * halfHeight, -0.5f), vec2(0.0f, 0.0f)), 
				VertexData(vec3( 1.0f *halfWidth, -1.0f * halfHeight,  -0.5f), vec2(1.0f, 0.0f)),
				VertexData(vec3(1.0f *halfWidth,  1.0f * halfHeight,  -0.5f), vec2(1.0f, 1.0f)), 
				VertexData(vec3( -1.0f *halfWidth,  1.0f * halfHeight,  -0.5f), vec2(0.0f, 1.0f)),

				//#2
				VertexData(vec3(-0.707f *halfWidth - 0.353, -1.0f * halfHeight, -0.707f *halfWidth + 0.353), vec2(0.0f, 0.0f)),
				VertexData(vec3(0.707f *halfWidth - 0.353, 1.0f * halfHeight,  0.707f * halfWidth + 0.353), vec2(1.0f, 1.0f)),
				VertexData(vec3(-0.707f *halfWidth - 0.353,  1.0f * halfHeight,  -0.707f * halfWidth + 0.353), vec2(0.0f, 1.0f)),
				VertexData(vec3(0.707f *halfWidth - 0.353,  -1.0f * halfHeight,  0.707f * halfWidth + 0.353), vec2(1.0f, 0.0f)),

				//#3
				VertexData(vec3(-0.707f *halfWidth + 0.353, -1.0f * halfHeight, 0.707f *halfWidth + 0.353), vec2(0.0f, 0.0f)),
				VertexData(vec3(0.707f *halfWidth + 0.353, 1.0f * halfHeight,  -0.707f * halfWidth + 0.353), vec2(1.0f, 1.0f)),
				VertexData(vec3(-0.707f *halfWidth + 0.353,  1.0f * halfHeight,  0.707f * halfWidth + 0.353), vec2(0.0f, 1.0f)),
				VertexData(vec3(0.707f *halfWidth + 0.353,  -1.0f * halfHeight,  -0.707f * halfWidth + 0.353), vec2(1.0f, 0.0f)),

			};

			unsigned short indices[] = {
				0, 1, 2, 0, 2, 3,     4, 5, 6, 4, 7, 5,   8, 9, 10, 8, 11, 9,
			};
			theMesh->addVertices(vertices,sizeof(vertices)/sizeof(VertexData));
			theMesh->addIndices(indices,sizeof(indices)/sizeof(unsigned short));
			theMesh->finish();
			m_quadMesh = theMesh;
			m_quadMat = Material::createFromTemplate("Grass");
			auto tex = TextureMgr::shared()->getByPath(filePath);
			 
			tex->genMipMap();
			 
			m_quadMat->setTex("DiffuseMap", tex);
		}
		break;
	case VegetationType::ModelType:
		{
			auto treeModel = Model::create(filePath, false);
			m_model = treeModel;
			for(auto i = 0; i < treeModel->getMatCount(); i ++)
			{
				auto mat = treeModel->getMat(i);
				mat->setIsEnableInstanced(true);
				mat->reload();
			}
		}
		break;
	default: ;
	}
}

void VegetationBatch::insertInstanceData(InstanceData inst)
{
	switch (m_type)
	{
	case VegetationType::QUAD_TRI:
		{
			m_quadMesh->pushInstance(inst);
		}
		break;
	case VegetationType::ModelType:
		{
			for(auto i = 0; i < m_model->getMeshCount(); i++)
			{
				m_model->getMesh(i)->pushInstance(inst);
			}
		}
		break;
	default: ;
	}
	m_totalCount += 1;
}

void VegetationBatch::clear()
{
	switch (m_type)
	{
	case VegetationType::QUAD_TRI:
		{
			m_quadMesh->clearInstances();
		}
		break;
	case VegetationType::ModelType:
		{
			for(auto i = 0; i < m_model->getMeshCount(); i++)
			{
				m_model->getMesh(i)->clearInstances();
			}
		}
		break;
	default: ;
	}
	m_totalCount = 0;
}

void VegetationBatch::setUpTransFormation(TransformationInfo& info)
{
	auto currCam = g_GetCurrScene()->defaultCamera();
	info.m_projectMatrix = currCam->projection();
	info.m_viewMatrix = currCam->getViewMatrix();
	Matrix44 mat;
	mat.setToIdentity();
	info.m_worldMatrix = mat;
}

void VegetationBatch::commitRenderCmd()
{
	switch (m_type)
	{
	case VegetationType::QUAD_TRI:
		{
			auto theMesh = m_quadMesh;
			auto mat = m_quadMat;
			theMesh->submitInstanced();
			RenderCommand command(theMesh, mat, RenderCommand::RenderType::Common, RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
			command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
			setUpTransFormation(command.m_transInfo);
			Renderer::shared()->addRenderCommand(command);
			setUpTransFormation(command.m_transInfo);
		}
		break;
	case VegetationType::ModelType:
		{
			for(auto i = 0; i < m_model->getMeshCount(); i++)
			{
				
				auto theMesh = m_model->getMesh(i);
				auto mat = m_model->getMat(theMesh->getMatIndex());
				theMesh->submitInstanced();
				RenderCommand command(theMesh, mat, RenderCommand::RenderType::Common,RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
				command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
				setUpTransFormation(command.m_transInfo);
				Renderer::shared()->addRenderCommand(command);
				setUpTransFormation(command.m_transInfo);
			}
		}
		break;
	default: ;
	}
}

void VegetationBatch::commitShadowRenderCmd()
{
	switch (m_type)
	{
	case VegetationType::QUAD_TRI:
		{
		}
		break;
	case VegetationType::ModelType:
		{
			for(auto i = 0; i < m_model->getMeshCount(); i++)
			{
				
				auto theMesh = m_model->getMesh(i);
				auto mat = m_model->getMat(theMesh->getMatIndex());
				theMesh->submitInstanced();
				RenderCommand command(theMesh, mat, RenderCommand::RenderType::Shadow,RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
				command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
				setUpTransFormation(command.m_transInfo);
				Renderer::shared()->addRenderCommand(command);
				setUpTransFormation(command.m_transInfo);
			}
		}
		break;
	default: ;
	}
}

VegetationInfo::VegetationInfo()
{
}

void VegetationInfo::init(const VegetationBatInfo * lod0, const VegetationBatInfo * lod1, const VegetationBatInfo * lod2)
{
	m_lodBatch[0] = new VegetationBatch(lod0);
	m_lodBatch[1] = new VegetationBatch(lod1);
	m_lodBatch[2] = new VegetationBatch(lod2);
}

void VegetationInfo::clear()
{
	for(int i = 0; i < 3; i++)
	{

		m_lodBatch[i]->clear();
	}
}

void VegetationInfo::commitRenderCmd()
{
	for(int i = 0; i < 3; i++)
	{
		if(m_lodBatch[i]->m_totalCount)
		{
			m_lodBatch[i]->commitRenderCmd();
		}
	}
}

void VegetationInfo::insert(InstanceData inst)
{
	auto cam = g_GetCurrScene()->defaultCamera();
	auto dist = cam->getWorldPos().distance(inst.posAndScale.toVec3());
	if(dist < 15)
	{
		m_lodBatch[0]->insertInstanceData(inst);
	}
	else if(dist < 35)
	{
		m_lodBatch[1]->insertInstanceData(inst);
	}
	else
	{
		m_lodBatch[2]->insertInstanceData(inst);
	}
}

bool VegetationInfo::anyHas()
{
	for(int i = 0; i < 3; i++)
	{
		if(m_lodBatch[i]->m_totalCount) return true;
	}
	return false;
}

void VegetationInfo::submitShadowDraw()
{
	if(m_lodBatch[0]->m_totalCount) m_lodBatch[0]->commitShadowRenderCmd();
	if(m_lodBatch[1]->m_totalCount) m_lodBatch[1]->commitShadowRenderCmd();
}

TreeGroup::TreeGroup(int treeClass)
{
	m_treeClass = treeClass;
}

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

int Tree::regVegetation(const VegetationBatInfo * lod0, const VegetationBatInfo * lod1, const VegetationBatInfo * lod2)
{
	auto info = new VegetationInfo();
	// info->m_type = type;
	info->init(lod0, lod1, lod2);
	m_infoList.push_back(info);
	return m_infoList.size() -1;
}

void Tree::addTreeGroup(TreeGroup* treeGroup)
{
	m_tree[treeGroup->m_treeClass].insert(treeGroup);
}

void Tree::clearTreeGroup()
{
	for(auto p : m_tree)
	{
		p.second.clear();
	}
	for(auto info :m_infoList)
	{
		info->clear();	
	}
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

void Tree::submitShadowDraw()
{
	for(auto info :m_infoList)
	{
		if(info->anyHas())
		{
			info->submitShadowDraw();

		}
	}
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
		auto info = m_infoList[iter.first];
		auto &tg = iter;
		auto theSet = tg.second;
		for(auto group : theSet)
		{
			for(auto inst :group->m_instance)
			{
				info->insert(inst);
			}
			// info->m_totalCount += group->m_instance.size();
		}
	}
	if (! m_isFinish)
	{
		finish();
	}else
	{
		for(auto info :m_infoList)
		{
			if(info->anyHas())
			{
				info->commitRenderCmd();

			}
		}
		// auto targetAmount = m_mesh->m_instanceOffset.size();
		// if (targetAmount > 0)
		// {
		// 	// if(targetAmount > m_mesh->getInstanceBuf()->getAmount())
		// 	// {
		// 	// 	m_mesh->submitInstanced(targetAmount);
		// 	// }else
		// 	// {
		// 	// 	m_mesh->reSubmitInstanced();
		// 	// }
		// 	m_mesh->submitInstanced();
		// 	m_leafMesh->submitInstanced();
		// 	// m_mesh->getInstanceBuf()->use();
		// 	// m_mesh->getInstanceBuf()->allocate(&m_mesh->m_instanceOffset[0], m_mesh->m_instanceOffset.size() * sizeof(InstanceData), RenderFlag::BufferStorageType::DYNAMIC_DRAW);
		// }
		// // m_mesh->reSubmitInstanced();
	}
	// if(m_mesh->m_instanceOffset.size() > 0)
	// {
	// 	RenderCommand command(m_mesh, m_barkMat, RenderCommand::RenderType::Instanced);
	// 	command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
	// 	setUpTransFormation(command.m_transInfo);
	// 	Renderer::shared()->addRenderCommand(command);
	// 	setUpTransFormation(command.m_transInfo);
	//
	// 	RenderCommand commandLeaf(m_leafMesh,  m_leafMat, RenderCommand::RenderType::Instanced);
	// 	commandLeaf.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
	// 	setUpTransFormation(commandLeaf.m_transInfo);
	// 	Renderer::shared()->addRenderCommand(commandLeaf);
	// 	setUpTransFormation(commandLeaf.m_transInfo);
	// }

}

void Tree::finish()
{

	// m_mesh->finish();
	// m_leafMesh->finish();
	// // tlog("instance size %d", m_mesh->getInstanceSize());
	// // m_mesh->reSubmitInstanced();
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
	// auto treeModel = Model::create("Models/tree/tree.tzw", false);
	//
	//
	// m_barkMat = treeModel->getMat(0);
	// m_barkMat->setIsEnableInstanced(true);
	//
	// m_barkMat->setTex("DiffuseMap",TextureMgr::shared()->getByPath("Models/tree/bark.jpg", true));
	// m_barkMat->reload();
	//
	//
	// m_leafMat = treeModel->getMat(1);
	// m_leafMat->setIsEnableInstanced(true);
	// m_barkMat->setIsCullFace(false);
	// m_leafMat->reload();
	// m_leafMat->setTex("DiffuseMap",TextureMgr::shared()->getByPath("Models/tree/twig.png", true));
	//
	// m_mesh = treeModel->getMesh(0);
	// m_leafMesh = treeModel->getMesh(1);
}

} // namespace tzw
