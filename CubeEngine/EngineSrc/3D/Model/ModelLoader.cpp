#include "ModelLoader.h"
#include "Model.h"
#include "../../Texture/TextureMgr.h"
#include "../Effect/Effect.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include <iostream>
#include "Utility/file/Tfile.h"
#include "Utility/misc/Tmisc.h"

namespace tzw {

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
	auto tmgr = TextureMgr::shared();
	auto& materialList = doc["materialList"];
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
		for(size_t matI = 0; matI != matList.Size(); matI++)
		{
			auto & matNode = matList[matI];
			auto mat = Material::createFromJson(matNode, folder);
			model->m_effectList.push_back(mat);
		}
	}
	else // old version
	{
		for (unsigned int i = 0;i<materialList.Size();i++)
		{
			auto & materialData = materialList[i];
			// reuse material // there is a problem
			Material* mat = nullptr;//matPool->getMaterialByName(mangleedName);
			//if (!mat)
			//{
			//	mat = new Material();
			//	matPool->addMaterial(mangleedName,mat);
			//}

			if(materialData.HasMember("effectType"))
			{
				mat = Material::createFromTemplate(materialData["effectType"].GetString());
			}else
			{
				mat = Material::createFromTemplate("ModelPBR");
			}
			auto thestr = materialData["diffuseMap"].GetString();
			if (strcmp(thestr, "") != 0)
			{
				mat->setTex("DiffuseMap",
					tmgr->getByPath(Tfile::shared()->toAbsFilePath(materialData["diffuseMap"].GetString(), folder), true));
			}
			if (strcmp(materialData["normalMap"].GetString(), "") != 0)
			{
				mat->setTex("NormalMap",
					tmgr->getByPath(Tfile::shared()->toAbsFilePath(materialData["normalMap"].GetString(), folder), true));
			}
			else
			{
				//default Normal Map
				mat->setTex("NormalMap", tmgr->getByPath("Texture/BuiltInTexture/defaultNormalMap.png"));
			}

			if (materialData.HasMember("roughnessMap") && strcmp(materialData["roughnessMap"].GetString(), "") != 0)
			{
				mat->setTex("RoughnessMap",
					tmgr->getByPath(Tfile::shared()->toAbsFilePath(materialData["roughnessMap"].GetString(), folder), true));
			}
			else
			{
				//default Normal Map
				mat->setTex("NormalMap", tmgr->getByPath("Texture/BuiltInTexture/defaultNormalMap.png"));
			}
			//default Roughness Map
			mat->setTex("RoughnessMap", tmgr->getByPath("Texture/BuiltInTexture/defaultRoughnessMap.png"));
			mat->setTex("MetallicMap", TextureMgr::shared()->getByPath("Texture/BuiltInTexture/defaultMetallic.png"));
			model->m_effectList.push_back(mat);
		}
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
