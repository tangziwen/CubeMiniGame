#include "GamePartRenderMgr.h"


#include "PartSurface.h"
#include "3D/Model/Model.h"
#include "3D/Primitive/CubePrimitive.h"
#include "3D/Primitive/CylinderPrimitive.h"
#include "3D/Primitive/RightPrismPrimitive.h"
static tzw::Material * g_material;
namespace tzw
{
GamePartRenderMgr::GamePartRenderMgr()
{
	g_material = Material::createFromTemplate("ModelSTD");
	g_material->setIsEnableInstanced(true);
	g_material->reload();
}

GamePartRenderInfo GamePartRenderMgr::getRenderInfo(VisualInfo visualInfo, PartSurface* surface)
{
	GamePartRenderInfo info;
	info.mesh = findOrCreateMesh(visualInfo);
	if(visualInfo.type != VisualInfo::VisualInfoType::Mesh)
	{
		info.material = findOrCreateMaterial(surface);
	}else
	{
		info.material = m_modelMap[visualInfo.filePath]->getMat(0);
	}
	return info;
}

AABB GamePartRenderMgr::getPartLocalAABB(VisualInfo visualInfo)
{
	auto mesh = findOrCreateMesh(visualInfo);
	return mesh->getAabb();
}
Mesh * GamePartRenderMgr::findOrCreateMesh(VisualInfo visualInfo)
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
		if(m_modelMap.find(visualInfo.filePath) == m_modelMap.end())
		{
			auto size = visualInfo.size;
			//sorry not supported yet
			auto model = Model::create(visualInfo.filePath);
			auto tex = TextureMgr::shared()->getByPath(visualInfo.diffusePath, true);
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
			}

			model->setScale(size.x, size.y, size.z);
			for(int i = 0; i < visualInfo.extraFileList.size(); i++)
			{
				auto modelExtra = Model::create(visualInfo.extraFileList[i]);
				model->addExtraMeshList(modelExtra->getMeshList());
			}
			m_modelMap[visualInfo.filePath] = model;
		}
		return m_modelMap[visualInfo.filePath]->getMesh(0);
	}

}
Material* GamePartRenderMgr::findOrCreateMaterial(PartSurface* surface)
{
	if(m_materialMap.find(surface) == m_materialMap.end())
	{
		auto mat = Material::createFromTemplate("ModelPBR");
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

		m_materialMap[surface] = mat;
		return mat;
	}
	else
	{
		return m_materialMap[surface];
	}
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
Model* GamePartRenderMgr::getModel(VisualInfo visualInfo)
{
	return m_modelMap[visualInfo.filePath];
}
}

