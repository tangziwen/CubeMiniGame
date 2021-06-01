#include "GamePartRenderMgr.h"


#include "PartSurface.h"
#include "3D/Model/Model.h"
#include "3D/Primitive/CubePrimitive.h"
#include "3D/Primitive/CylinderPrimitive.h"
#include "3D/Primitive/RightPrismPrimitive.h"
#include "GamePart.h"
#include "BearPart.h"
#include "GamePartRenderNode.h"
static tzw::Material * g_material;
namespace tzw
{
GamePartRenderMgr::GamePartRenderMgr()
{
	g_material = Material::createFromTemplate("ModelSTD");
	g_material->setIsEnableInstanced(true);
	g_material->reload();
	m_previewMat = nullptr;
}

void GamePartRenderMgr::getRenderInfo(bool isInstancing, GamePartRenderNode * part,  VisualInfo visualInfo, PartSurface* surface, std::vector<GamePartRenderInfo> &infoList)
{
	GamePartRenderInfo info;
	
	if(visualInfo.type != VisualInfo::VisualInfoType::Mesh)
	{
		info.mesh = findOrCreateSingleMesh(visualInfo);
		info.material = findOrCreateSingleMaterial(isInstancing, part, surface);
		infoList.emplace_back(info);
	}
	else
	{
		auto model = getModel(isInstancing, visualInfo);
		auto meshList = model->getMeshList();
		for(auto mesh : meshList)
		{
			Material * material = nullptr;
			auto matListIter = m_modelToMatList[model].find(getStateAndRenderModeStr(part));
			if(matListIter!= m_modelToMatList[model].end())
			{
				auto&matList = matListIter->second.m_matList;
				if(matList.size() > 1)
				{
					material = matList[mesh->getMatIndex()];
				}
				else
				{
					material = matList[0];
				}
			}
			else
			{
				auto& newMatList = createModelMatList(model, part);
				if(newMatList.m_matList.size() > 1)
				{
					material = newMatList.m_matList[mesh->getMatIndex()];
				}
				else
				{
					material = newMatList.m_matList[0];
				}
			}
			GamePartRenderInfo info;
			info.mesh = mesh;
			info.material = material;
			infoList.emplace_back(info);
		}
	}
	return;
}

AABB GamePartRenderMgr::getPartLocalAABB(VisualInfo visualInfo)
{
	Mesh * mesh = nullptr;
	if(visualInfo.type != VisualInfo::VisualInfoType::Mesh){
		mesh = findOrCreateSingleMesh(visualInfo);
	}else
	{
		mesh = getModel(false, visualInfo)->getMesh(0);
	}
	return mesh->getAabb();
}
Mesh * GamePartRenderMgr::findOrCreateSingleMesh(VisualInfo visualInfo)
{
	if(visualInfo.type != VisualInfo::VisualInfoType::Mesh)
	{
		auto typeStr = getVisualTypeStr(visualInfo);
		if(m_meshMap.find(typeStr) == m_meshMap.end())
		{
			Mesh * mesh;
			switch(visualInfo.type)
			{
			case VisualInfo::VisualInfoType::CubePrimitive:
			{
				auto size = visualInfo.size;
				auto node = new CubePrimitive(size.x, size.y, size.z, false);
				mesh = node->getMesh();
			}
			break;
			case VisualInfo::VisualInfoType::CylinderPrimitive:
			{
				auto size = visualInfo.size;
				auto node = new CylinderPrimitive(size.x, size.y, size.z);
				mesh = node->getMesh();
			}
			break;
			case VisualInfo::VisualInfoType::Mesh:
			{
			}
			break;
				
	        case VisualInfo::VisualInfoType::RightPrismPrimitive:
			{
	        	auto size = visualInfo.size;
				auto node = new RightPrismPrimitive(size.x, size.y, size.z);
	        	mesh = node->getMesh();
	        }
			break;
			default: ;
			}
			m_meshMap[typeStr] = mesh;
			return mesh;
		}
		else
		{
			return m_meshMap[typeStr];
		}
	}else// model mesh
	{
		//auto model = getModel(visualInfo);
		//return model->getMesh(0);
	}
	return nullptr;
}
Material* GamePartRenderMgr::findOrCreateSingleMaterial(bool isInsatnce, GamePartRenderNode * part, PartSurface* surface)
{
	std::unordered_map<std::string, Material *> * targetMap = nullptr;
	if(isInsatnce)
	{
		targetMap = &m_materialMap;
	}
	else
	{
		targetMap = &m_materialMapSingle;
	}
	std::string surfaceStr;
	std::string state = part->getState();
	surfaceStr = getSurfaceStr(surface) + part->getState();
	GamePartModelMatList newMatList;
	if(targetMap->find(surfaceStr) == targetMap->end())
	{
		Material * mat;
		if(state == "Preview")
		{
			mat = Material::createFromTemplate("ModelRimLight");
			newMatList.m_matList.emplace_back(mat);
			auto texture =  TextureMgr::shared()->getByPath(surface->getDiffusePath());
			mat->setTex("DiffuseMap", texture);

			auto metallicTexture =  TextureMgr::shared()->getByPath(surface->getMetallicPath());
			mat->setTex("MetallicMap", metallicTexture);

			auto roughnessTexture =  TextureMgr::shared()->getByPath(surface->getRoughnessPath());
			mat->setTex("RoughnessMap", roughnessTexture);

			auto normalMapTexture =  TextureMgr::shared()->getByPath(surface->getNormalMapPath());
			mat->setTex("NormalMap", normalMapTexture);
		}
		else
		{
			mat = Material::createFromTemplate("ModelPBR");
			mat->setIsEnableInstanced(true);
			mat->reload();

			auto texture =  TextureMgr::shared()->getByPath(surface->getDiffusePath());
			mat->setTex("DiffuseMap", texture);

			auto metallicTexture =  TextureMgr::shared()->getByPath(surface->getMetallicPath());
			mat->setTex("MetallicMap", metallicTexture);

			auto roughnessTexture =  TextureMgr::shared()->getByPath(surface->getRoughnessPath());
			mat->setTex("RoughnessMap", roughnessTexture);

			auto normalMapTexture =  TextureMgr::shared()->getByPath(surface->getNormalMapPath());
			mat->setTex("NormalMap", normalMapTexture);

			newMatList.m_matList.emplace_back(mat);
		
		}//default;

		processMatList(part, &newMatList);
		(*targetMap)[surfaceStr] = mat;
		return mat;
	}
	else
	{
		return (*targetMap)[surfaceStr];
	}
}
Material* GamePartRenderMgr::findOrCreateMaterialTransparent(VisualInfo visualInfo, PartSurface* surface)
{
	return nullptr;
}
std::string GamePartRenderMgr::getVisualTypeStr(VisualInfo visualInfo)
{
	std::string prefix ="";
	switch(visualInfo.type)
	{
	case VisualInfo::VisualInfoType::CubePrimitive:
	{
			prefix = "Cube";
	}
	break;
	case VisualInfo::VisualInfoType::CylinderPrimitive:
	{
			prefix = "Cylinder";
	}
	break;
	case VisualInfo::VisualInfoType::Mesh:
	{
			prefix = "Mesh";
	}
	break;
    case VisualInfo::VisualInfoType::RightPrismPrimitive:
	{
        	prefix = "RightPrism";
    }
	break;
	default: ;
	}
	int sizeIntX = visualInfo.size.x * 100;
	int sizeIntY = visualInfo.size.y * 100;
	int sizeIntZ = visualInfo.size.z * 100;
	char resultTmp[128];
	sprintf(resultTmp, "%s:%d %d%d", prefix.c_str(), sizeIntX, sizeIntY, sizeIntZ);
	return resultTmp;
}
std::string GamePartRenderMgr::getSurfaceStr(PartSurface* surface)
{
	char resultTmp[128];
	sprintf(resultTmp, "%s", surface->getName().c_str());
	return std::string(resultTmp);
}
Model* GamePartRenderMgr::getModel(bool isInsatnce, VisualInfo visualInfo)
{
	std::unordered_map<std::string, Model *> * targetMap;
	targetMap = &m_modelMap;
	if(targetMap->find(visualInfo.filePath) == targetMap->end())
	{
		auto size = visualInfo.size;
		//sorry not supported yet
		auto model = Model::create(visualInfo.filePath);
		auto tex = TextureMgr::shared()->getByPath(visualInfo.diffusePath, true);
		std::unordered_map<std::string, GamePartModelMatList> empty;
		//prototype
		if(model->getMatCount() > 1)
		{
			for (auto mesh: model->getMeshList())
			{
				auto material = model->getMat(mesh->getMatIndex());

				material->setTex("DiffuseMap", tex);
				material->setTex("RoughnessMap", TextureMgr::shared()->getByPath(visualInfo.roughnessPath, true));
				material->setTex("MetallicMap", TextureMgr::shared()->getByPath(visualInfo.metallicPath, true));
				material->setTex("NormalMap", TextureMgr::shared()->getByPath(visualInfo.normalMapPath, true));
				material->setIsEnableInstanced(true);
				material->reload();
				empty["default"].m_matList.emplace_back(material);
			}
		}else
		{
			auto material = model->getMat(0);
			material->setTex("DiffuseMap", tex);
			material->setTex("RoughnessMap", TextureMgr::shared()->getByPath(visualInfo.roughnessPath, true));
			material->setTex("MetallicMap", TextureMgr::shared()->getByPath(visualInfo.metallicPath, true));
			material->setTex("NormalMap", TextureMgr::shared()->getByPath(visualInfo.normalMapPath, true));
			material->setIsEnableInstanced(true);
			material->reload();
			empty["default"].m_matList.emplace_back(material);
		}
		m_modelToMatList[model] = empty;
		model->setScale(size.x, size.y, size.z);
		for(int i = 0; i < visualInfo.extraFileList.size(); i++)
		{
			auto modelExtra = Model::create(visualInfo.extraFileList[i]);
			model->addExtraMeshList(modelExtra->getMeshList());
		}
		(*targetMap)[visualInfo.filePath] = model;
	}
	return (*targetMap)[visualInfo.filePath];
}

//根据Status还有RenderMode拷贝构造出新的材质
GamePartModelMatList& GamePartRenderMgr::createModelMatList(Model* model, GamePartRenderNode * part)
{
	std::string state = part->getState();
	std::string key = getStateAndRenderModeStr(part);
	GamePartModelMatList newMatList;
	if(state == "Preview")
	{
		newMatList.m_matList.emplace_back(Material::createFromTemplate("ModelRimLight"));
	}
	else if(state == "isFlipped")//bearing
	{
		//clone from default.
		for(auto mat : m_modelToMatList[model]["default"].m_matList)
		{
			auto newMat = mat->clone();
			auto tex = TextureMgr::shared()->getByPath("Blocks/Bearing/diffuse_inverted.png");
			newMat->setTex("DiffuseMap", tex);
			newMatList.m_matList.emplace_back(newMat);
		}
	}
	else
	{
		//clone from default.
		for(auto mat : m_modelToMatList[model]["default"].m_matList)
		{
			auto newMat = mat->clone();
			newMatList.m_matList.emplace_back(newMat);
		}
	}

	processMatList(part, &newMatList);
	m_modelToMatList[model][key] = newMatList;
	return m_modelToMatList[model][key];
}
std::string GamePartRenderMgr::getStateAndRenderModeStr(GamePartRenderNode * part)
{
	int a = static_cast<int>(part->getRenderMode());
	return part->getState() + std::to_string(a);
}
void GamePartRenderMgr::processMatList(GamePartRenderNode* part, GamePartModelMatList* matList)
{
	for(auto mat : matList->m_matList)
	{
		switch(part->getRenderMode())
		{
		case GamePartRenderNode::RenderMode::COMMON:
			mat->setIsEnableInstanced(true);
			mat->reload();
			break;
		case GamePartRenderNode::RenderMode::NO_INSTANCING:
			mat->setIsEnableInstanced(false);
			mat->reload();
			break;
		case GamePartRenderNode::RenderMode::AFTER_DEPTH:
			mat->setIsEnableInstanced(false);
			mat->reload();
			break;
		}
	}
}
}

