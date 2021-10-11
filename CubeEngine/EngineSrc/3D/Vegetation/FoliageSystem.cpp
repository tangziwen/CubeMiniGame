#include "EngineSrc/Technique/MaterialPool.h"
#include "../../Scene/SceneMgr.h"
#include "EngineSrc/3D/Vegetation/FoliageSystem.h"

#include "3D/Model/Model.h"
#include "Mesh/InstancedMesh.h"

namespace tzw {
VegetationBatInfo::VegetationBatInfo()
{
}

VegetationBatInfo::VegetationBatInfo(VegetationType theType, std::string filePath, vec2 size)
{
	m_type = theType;
	file_path = filePath;
	m_size = size;
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
			float halfWidth = m_info.m_size.x / 2.f;
			float halfHeight = m_info.m_size.y / 2.f;
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
			tex->setWarp(RenderFlag::WarpAddress::ClampToEdge);
			m_quadMat->setTex("DiffuseMap", tex);
			auto instancedMesh = new InstancedMesh();
			instancedMesh->setMesh(theMesh);
			m_instancedMeshList.push_back(instancedMesh);
		}
		break;
		case VegetationType::QUAD:
		{
			auto theMesh = new Mesh();
			float halfWidth = m_info.m_size.x / 2.f;
			float halfHeight = m_info.m_size.y / 2.f;
			VertexData vertices[] = {
				//#1
				VertexData(vec3(-1.0f *halfWidth, 0, -0.5f), vec2(0.0f, 0.0f)), 
				VertexData(vec3( 1.0f *halfWidth, 0,  -0.5f), vec2(1.0f, 0.0f)),
				VertexData(vec3(1.0f *halfWidth,  2.0f * halfHeight,  -0.5f), vec2(1.0f, 1.0f)), 
				VertexData(vec3( -1.0f *halfWidth,  2.0f * halfHeight,  -0.5f), vec2(0.0f, 1.0f)),
			};

			unsigned short indices[] = {
				0, 1, 2, 0, 2, 3,
			};
			theMesh->addVertices(vertices,sizeof(vertices)/sizeof(VertexData));
			theMesh->addIndices(indices,sizeof(indices)/sizeof(unsigned short));
			theMesh->finish();
			m_quadMesh = theMesh;
			m_quadMat = Material::createFromTemplate("BillBoardInstance");
			auto tex = TextureMgr::shared()->getByPath(filePath);
			 
			tex->genMipMap();
			tex->setWarp(RenderFlag::WarpAddress::ClampToEdge);
			m_quadMat->setTex("DiffuseMap", tex);
			auto instancedMesh = new InstancedMesh();
			instancedMesh->setMesh(theMesh);
			m_instancedMeshList.push_back(instancedMesh);
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
			for(auto i = 0; i < m_model->getMeshCount(); i++)
			{
				
				auto theMesh = m_model->getMesh(i);
				auto instancedMesh = new InstancedMesh();
				instancedMesh->setMesh(theMesh);
				m_instancedMeshList.push_back(instancedMesh);
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
			m_instancedMeshList[0]->pushInstance(inst);
		}
		break;
	case VegetationType::QUAD:
		{
			m_instancedMeshList[0]->pushInstance(inst);
		}
		break;
	case VegetationType::ModelType:
		{
			for(auto i = 0; i < m_model->getMeshCount(); i++)
			{
				m_instancedMeshList[i]->pushInstance(inst);
				//m_model->getMesh(i)->pushInstance(inst);
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
			m_instancedMeshList[0]->clearInstances();
		}
		break;
	case VegetationType::QUAD:
		{
			m_instancedMeshList[0]->clearInstances();
		}
		break;
	case VegetationType::ModelType:
		{
			for(auto i = 0; i < m_model->getMeshCount(); i++)
			{
				m_instancedMeshList[i]->clearInstances();
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

void VegetationBatch::commitRenderCmd(RenderFlag::RenderStage stageType, RenderQueue * queues, int requirementArg)
{
	switch (m_type)
	{
	case VegetationType::QUAD_TRI:
		{
			auto theMesh = m_quadMesh;
			auto mat = m_quadMat;
			m_instancedMeshList[0]->submitInstanced();
			RenderCommand command(theMesh, mat, this, stageType, RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
			command.setInstancedMesh(m_instancedMeshList[0]);
			command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
			setUpTransFormation(command.m_transInfo);
			queues->addRenderCommand(command, requirementArg);
			setUpTransFormation(command.m_transInfo);
		}
		break;

	case VegetationType::QUAD:
		{
			auto theMesh = m_quadMesh;
			auto mat = m_quadMat;
			m_instancedMeshList[0]->submitInstanced();
			RenderCommand command(theMesh, mat, this, stageType, RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
			command.setInstancedMesh(m_instancedMeshList[0]);
			command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
			setUpTransFormation(command.m_transInfo);
			queues->addRenderCommand(command, requirementArg);
			setUpTransFormation(command.m_transInfo);
		}
		break;
	
	case VegetationType::ModelType:
		{
			for(auto i = 0; i < m_model->getMeshCount(); i++)
			{
				
				auto theMesh = m_model->getMesh(i);
				auto mat = m_model->getMat(theMesh->getMatIndex());
				m_instancedMeshList[i]->submitInstanced();
				RenderCommand command(theMesh, mat, this, stageType,RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
				command.setInstancedMesh(m_instancedMeshList[i]);
				command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
				setUpTransFormation(command.m_transInfo);
				queues->addRenderCommand(command, requirementArg);
				setUpTransFormation(command.m_transInfo);
			}
		}
		break;
	default: ;
	}
}

void VegetationBatch::commitShadowRenderCmd(RenderQueue * queues, int level)
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
				m_instancedMeshList[i]->submitInstanced();
				RenderCommand command(theMesh, mat, this, RenderFlag::RenderStage::SHADOW, RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
				command.setInstancedMesh(m_instancedMeshList[i]);
				command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
				setUpTransFormation(command.m_transInfo);
				queues->addRenderCommand(command,level);
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
	if(lod1)
	{
		m_lodBatch[1] = new VegetationBatch(lod1);
	} else
	{
		m_lodBatch[1] = nullptr;
	}
	
	if(lod2)
	{
		m_lodBatch[2] = new VegetationBatch(lod2);
	} else
	{
		m_lodBatch[2] = nullptr;
	}
	
}

void VegetationInfo::clear()
{
	for(int i = 0; i < 3; i++)
	{
		if(m_lodBatch[i])
		{
			m_lodBatch[i]->clear();
		}
	}
}

void VegetationInfo::commitRenderCmd(RenderFlag::RenderStage stageType, RenderQueue * queues, int requirementArg)
{
	for(int i = 0; i < 3; i++)
	{
		if(m_lodBatch[i] && m_lodBatch[i]->m_totalCount)
		{
			m_lodBatch[i]->commitRenderCmd(stageType, queues, requirementArg);
		}
	}
}

void VegetationInfo::insert(InstanceData inst)
{
	auto cam = g_GetCurrScene()->defaultCamera();
	auto dist = cam->getWorldPos().distance(inst.transform.getTranslation());
	if(dist > 200.0f) return;//just ignore

	if(dist < 35.f)
	{
		m_lodBatch[0]->insertInstanceData(inst);
	}
	else if(dist < 100.f)
	{
		if(m_lodBatch[1])
		{
			m_lodBatch[1]->insertInstanceData(inst);
		}else
		{
			m_lodBatch[0]->insertInstanceData(inst);
		}
	}
	else
	{
		if(m_lodBatch[2])
		{
			m_lodBatch[2]->insertInstanceData(inst);
		} else
		{
			if(m_lodBatch[1])
			{
				m_lodBatch[1]->insertInstanceData(inst);
			}else
			{
				m_lodBatch[0]->insertInstanceData(inst);
			}	
		}
	}
}

bool VegetationInfo::anyHas()
{
	for(int i = 0; i < 3; i++)
	{
		if(m_lodBatch[i])
		{
			if(m_lodBatch[i]->m_totalCount) return true;
		}
	}
	return false;
}

void VegetationInfo::submitShadowDraw(RenderQueue * queues, int level)
{
	if(m_lodBatch[0] && m_lodBatch[0]->m_totalCount) m_lodBatch[0]->commitShadowRenderCmd(queues, level);
	if(m_lodBatch[1] && m_lodBatch[1]->m_totalCount) m_lodBatch[1]->commitShadowRenderCmd(queues, level);
}

TreeGroup::TreeGroup(int treeClass)
{
	m_treeClass = treeClass;
}

FoliageSystem::FoliageSystem()
{
	 
	m_isFinish = false;
	std::string materialName = "FoliageSystem";

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

int FoliageSystem::regVegetation(const VegetationBatInfo * lod0, const VegetationBatInfo * lod1, const VegetationBatInfo * lod2)
{
	auto info = new VegetationInfo();
	// info->m_type = type;
	info->init(lod0, lod1, lod2);
	m_infoList.push_back(info);
	return m_infoList.size() -1;
}

void FoliageSystem::addTreeGroup(TreeGroup* treeGroup)
{
	m_tree[treeGroup->m_treeClass].insert(treeGroup);
}

void FoliageSystem::clearTreeGroup()
{
	for(auto &p : m_tree)
	{
		p.second.clear();
	}
	for(auto info :m_infoList)
	{
		info->clear();	
	}
}


void FoliageSystem::setUpTransFormation(TransformationInfo &info)
{
	auto currCam = g_GetCurrScene()->defaultCamera();
	info.m_projectMatrix = currCam->projection();
	info.m_viewMatrix = currCam->getViewMatrix();
	Matrix44 mat;
	mat.setToIdentity();
	info.m_worldMatrix = mat;
}

void FoliageSystem::submitShadowDraw(RenderQueue * queues, int level)
{
	for(auto info :m_infoList)
	{
		if(info->anyHas())
		{
			info->submitShadowDraw(queues, level);

		}
	}
}

unsigned int FoliageSystem::getTypeId()
{
	return 2333;
}

void FoliageSystem::pushCommand(RenderFlag::RenderStage requirementType, RenderQueue * queues, int requirementArg)
{
	//regroup
	for(const auto& iter : m_tree)
	{
		// std::copy(iter->m_instance.begin(), iter->m_instance.end(), m_mesh->m_instanceOffset.begin());
		auto info = m_infoList[iter.first];
		auto &tg = iter;
		auto theSet = tg.second;
		for(auto group : theSet)
		{
			for(auto &inst :group->m_instance)
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
				info->commitRenderCmd(requirementType, queues, requirementArg);

			}
		}

	}
}

void FoliageSystem::finish()
{

	// m_mesh->finish();
	// m_leafMesh->finish();
	// // tlog("instance size %d", m_mesh->getInstanceSize());
	// // m_mesh->reSubmitInstanced();
	m_isFinish = true;
}

void FoliageSystem::submitDrawCmd(RenderFlag::RenderStage requirementType, RenderQueue * queues, int requirementArg)
{
	RenderCommand command(m_mesh, m_material,this, requirementType);
	setUpTransFormation(command.m_transInfo);
	queues->addRenderCommand(command, requirementArg);
}

void FoliageSystem::initMesh()
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
