#include "ModelLoader.h"
#include "Model.h"
#include "../../Texture/TextureMgr.h"
#include "../Effect/Effect.h"
#include "EngineSrc/Technique/Material.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include <iostream>
#include <cstring>
#include <vector>
#include "Utility/file/Tfile.h"
#include "Utility/misc/Tmisc.h"

namespace tzw {
namespace
{

RenderFlag::BlendingFactor parseBlendFactor(const std::string& str, RenderFlag::BlendingFactor defaultFactor)
{
	if(str == "One")
	{
		return RenderFlag::BlendingFactor::One;
	}
	if(str == "Zero")
	{
		return RenderFlag::BlendingFactor::Zero;
	}
	if(str == "SrcAlpha")
	{
		return RenderFlag::BlendingFactor::SrcAlpha;
	}
	if(str == "OneMinusSrcAlpha")
	{
		return RenderFlag::BlendingFactor::OneMinusSrcAlpha;
	}
	if(str == "ConstantAlpha")
	{
		return RenderFlag::BlendingFactor::ConstantAlpha;
	}
	return defaultFactor;
}

RenderFlag::RenderStage parseRenderStage(const std::string& str, RenderFlag::RenderStage defaultStage)
{
	if(str == "COMMON")
	{
		return RenderFlag::RenderStage::COMMON;
	}
	if(str == "TRANSPARENT")
	{
		return RenderFlag::RenderStage::TRANSPARENT;
	}
	if(str == "AFTER_DEPTH_CLEAR")
	{
		return RenderFlag::RenderStage::AFTER_DEPTH_CLEAR;
	}
	if(str == "GUI")
	{
		return RenderFlag::RenderStage::GUI;
	}
	if(str == "SHADOW")
	{
		return RenderFlag::RenderStage::SHADOW;
	}
	if(str == "DEBUG_LAYER")
	{
		return RenderFlag::RenderStage::DEBUG_LAYER;
	}
	return defaultStage;
}

RenderFlag::CullMode parseCullMode(const std::string& str, RenderFlag::CullMode defaultMode)
{
	if(str == "front" || str == "Front")
	{
		return RenderFlag::CullMode::Front;
	}
	if(str == "back" || str == "Back")
	{
		return RenderFlag::CullMode::Back;
	}
	return defaultMode;
}

RasterFillMode parseRasterFillMode(const std::string& str, RasterFillMode defaultMode)
{
	if(str == "Wireframe")
	{
		return RasterFillMode::Wireframe;
	}
	if(str == "Fill")
	{
		return RasterFillMode::Fill;
	}
	return defaultMode;
}

std::string resolvePathInFolder(const std::string& filePath, const std::string& envFolder)
{
	auto filePathInFolder = Tfile::shared()->toAbsFilePath(filePath, envFolder);
	if(Tfile::shared()->isExist(filePathInFolder))
	{
		return filePathInFolder;
	}
	return filePath;
}

void applyRenderStateOverrides(Material * material, rapidjson::Value& materialData)
{
	if(materialData.HasMember("cullFace"))
	{
		material->setIsCullFace(materialData["cullFace"].GetBool());
	}
	if(materialData.HasMember("CullMode"))
	{
		material->setCullMode(parseCullMode(materialData["CullMode"].GetString(), material->getCullMode()));
	}
	if(materialData.HasMember("RasterFillMode"))
	{
		material->setRasterFillMode(parseRasterFillMode(materialData["RasterFillMode"].GetString(), material->getRasterFillMode()));
	}
	if(materialData.HasMember("DepthTestEnable"))
	{
		material->setIsDepthTestEnable(materialData["DepthTestEnable"].GetBool());
	}
	if(materialData.HasMember("DepthWriteEnable"))
	{
		material->setIsDepthWriteEnable(materialData["DepthWriteEnable"].GetBool());
	}
	if(materialData.HasMember("BlendEnable"))
	{
		material->setIsEnableBlend(materialData["BlendEnable"].GetBool());
	}
	if(materialData.HasMember("EnableInstanced"))
	{
		material->setIsEnableInstanced(materialData["EnableInstanced"].GetBool());
	}
	if(materialData.HasMember("SrcBlendFactor"))
	{
		material->setFactorSrc(parseBlendFactor(materialData["SrcBlendFactor"].GetString(), material->getFactorSrc()));
	}
	if(materialData.HasMember("DstBlendFactor"))
	{
		material->setFactorDst(parseBlendFactor(materialData["DstBlendFactor"].GetString(), material->getFactorDst()));
	}
	if(materialData.HasMember("RenderStage"))
	{
		material->setRenderStage(parseRenderStage(materialData["RenderStage"].GetString(), material->getRenderStage()));
	}
}

void applyAttributeOverride(Material * material, rapidjson::Value& attribute)
{
	if(!attribute.HasMember("name") || !attribute.HasMember("type"))
	{
		return;
	}

	std::string name = attribute["name"].GetString();
	std::string typeStr = attribute["type"].GetString();
	rapidjson::Value * val = nullptr;
	if(attribute.HasMember("value"))
	{
		val = &attribute["value"];
	}
	else if(attribute.HasMember("default"))
	{
		val = &attribute["default"];
	}

	if(!val)
	{
		return;
	}

	if(typeStr == "int")
	{
		material->setDefaultVar(name, val->GetInt());
	}
	else if(typeStr == "float")
	{
		material->setDefaultVar(name, static_cast<float>(val->GetDouble()));
	}
	else if(typeStr == "vec2")
	{
		material->setDefaultVar(name, vec2((*val)[0].GetDouble(), (*val)[1].GetDouble()));
	}
	else if(typeStr == "vec3")
	{
		material->setDefaultVar(name, vec3((*val)[0].GetDouble(), (*val)[1].GetDouble(), (*val)[2].GetDouble()));
	}
	else if(typeStr == "vec4")
	{
		material->setDefaultVar(name, vec4((*val)[0].GetDouble(), (*val)[1].GetDouble(), (*val)[2].GetDouble(), (*val)[3].GetDouble()));
	}
}

void applyMaterialVariantOverrides(Material * material, rapidjson::Value& materialData, const std::string& folder)
{
	applyRenderStateOverrides(material, materialData);

	if(materialData.HasMember("attributes"))
	{
		auto& attributes = materialData["attributes"];
		for(unsigned int i = 0; i < attributes.Size(); i++)
		{
			applyAttributeOverride(material, attributes[i]);
		}
	}

	if(materialData.HasMember("maps") && materialData["maps"].IsObject())
	{
		auto& maps = materialData["maps"];
		for(auto iter = maps.MemberBegin(); iter != maps.MemberEnd(); ++iter)
		{
			if(!iter->value.IsString() || !strlen(iter->value.GetString()))
			{
				continue;
			}
			auto texturePath = resolvePathInFolder(iter->value.GetString(), folder);
			material->setDefaultTex(iter->name.GetString(), TextureMgr::shared()->getByPath(texturePath, true));
		}
	}
}

MaterialInstance * createLegacyModelMaterial(rapidjson::Value& materialData, const std::string& folder)
{
	auto tmgr = TextureMgr::shared();
	MaterialInstance* mat = nullptr;
	if(materialData.HasMember("effectType"))
	{
		mat = MaterialInstance::createFromMaterial(materialData["effectType"].GetString());
	}
	else
	{
		mat = MaterialInstance::createFromMaterial("ModelPBR");
	}

	if(materialData.HasMember("diffuseMap") && strcmp(materialData["diffuseMap"].GetString(), "") != 0)
	{
		mat->setTex("DiffuseMap",
			tmgr->getByPath(Tfile::shared()->toAbsFilePath(materialData["diffuseMap"].GetString(), folder), true));
	}
	if(materialData.HasMember("normalMap") && strcmp(materialData["normalMap"].GetString(), "") != 0)
	{
		mat->setTex("NormalMap",
			tmgr->getByPath(Tfile::shared()->toAbsFilePath(materialData["normalMap"].GetString(), folder), true));
	}
	else
	{
		mat->setTex("NormalMap", tmgr->getByPath("Texture/BuiltInTexture/defaultNormalMap.png", true));
	}
	if(materialData.HasMember("roughnessMap") && strcmp(materialData["roughnessMap"].GetString(), "") != 0)
	{
		mat->setTex("RoughnessMap",
			tmgr->getByPath(Tfile::shared()->toAbsFilePath(materialData["roughnessMap"].GetString(), folder), true));
	}
	else
	{
		mat->setTex("RoughnessMap", tmgr->getByPath("Texture/BuiltInTexture/defaultRoughnessMap.png", true));
	}
	mat->setTex("MetallicMap", TextureMgr::shared()->getByPath("Texture/BuiltInTexture/defaultMetallic.png", true));
	return mat;
}

MaterialInstance * createModelMaterial(rapidjson::Value& materialData, const std::string& folder)
{
	if(materialData.HasMember("Material"))
	{
		auto material = Material::getFromLib(materialData["Material"].GetString())->clone();
		applyMaterialVariantOverrides(material, materialData, folder);
		return MaterialInstance::createFromMaterial(material);
	}
	if(materialData.HasMember("shaders") || materialData.HasMember("MaterialInstance"))
	{
		return MaterialInstance::createFromJson(materialData, folder);
	}
	return createLegacyModelMaterial(materialData, folder);
}

void loadMaterialList(std::vector<MaterialInstance *>& targetList, rapidjson::Value& materialList, const std::string& folder)
{
	for(size_t matI = 0; matI != materialList.Size(); matI++)
	{
		targetList.push_back(createModelMaterial(materialList[matI], folder));
	}
}

}

ModelLoader::ModelLoader()
{

}

void ModelLoader::loadModel(Model *model, std::string filePath, bool useCache)
{
	if(useCache)
	{
		auto iter = m_modelCache.find(filePath);
		if(iter != m_modelCache.end())
		{
			model->m_effectList = iter->second->m_effectList;
			model->m_meshList = iter->second->m_meshList;
			return;
		}
	}
	model->filePath = filePath;
	rapidjson::Document doc;
	auto data = Tfile::shared()->getData(Tmisc::getUserPath(filePath),true);
	auto relativeFilePath = Tfile::shared()->getReleativePath(filePath);
	auto folder = Tfile::shared()->getFolder(filePath);
	doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
	if (doc.HasParseError())
	{
		tlog("[error] get json data err! %s %d offset %d",
			filePath.c_str(),
			doc.GetParseError(),
			doc.GetErrorOffset());
		return;
	}

	//get the material
	auto matPool = MaterialPool::shared();
	auto mangleedName = matPool->getModelMangleedName(relativeFilePath);
	if(doc.HasMember("MaterialsFileName"))
	{
		
		auto mData = Tfile::shared()->getData(Tfile::shared()->toAbsFilePath(doc["MaterialsFileName"].GetString(), folder),true);
		rapidjson::Document matDoc;
		matDoc.Parse<rapidjson::kParseDefaultFlags>(mData.getString().c_str());
		if (matDoc.HasParseError())
		{
			tlog("[error] get json data err! %s %d offset %d",
				filePath.c_str(),
				matDoc.GetParseError(),
				matDoc.GetErrorOffset());
			abort();
		}
		auto& matList = matDoc["MaterialList"];
		loadMaterialList(model->m_effectList, matList, folder);
	}
	else
	{
		auto& materialList = doc["materialList"];
		loadMaterialList(model->m_effectList, materialList, folder);
	}
	

	//get the Mesh
	auto& meshList = doc["MeshList"];
	for (unsigned int i = 0;i<meshList.Size();i++)
	{
		char meshName[512];
		sprintf(meshName,"%s_%d",mangleedName.c_str(), i);
		Mesh * theMesh = MaterialPool::shared()->getMeshByName(meshName);
		if(!theMesh)//mesh use cache
		{
			theMesh = new Mesh();
			auto & meshData = meshList[i];
			//get vertices
			auto &verticesData = meshData["vertices"];
			for(unsigned int k = 0; k < verticesData.Size();k++)
			{
				auto& v = verticesData[k];
				theMesh->addVertex(VertexData(vec3(v[0].GetDouble(),v[1].GetDouble(),v[2].GetDouble()),
						vec3(v[3].GetDouble(),v[4].GetDouble(),v[5].GetDouble()),
						vec2(v[6].GetDouble(),v[7].GetDouble())));
			}
			//get indices
			auto &indicesData = meshData["indices"];
			for(unsigned int k = 0; k < indicesData.Size();k++)
			{
				theMesh->addIndex(indicesData[k].GetInt());
			}
			//material
			theMesh->setMatIndex(meshData["materialIndex"].GetInt());
			//theMesh->caclNormals();
			theMesh->finish(true);
			MaterialPool::shared()->addMesh(meshName, theMesh);
		}
		model->m_meshList.push_back(theMesh);
	}
	if(useCache)
	{
		m_modelCache[filePath] = model;
	}
}

} // namespace tzw
