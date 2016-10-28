#include "ModelLoader.h"
#include "Model.h"
#include "../../Texture/TextureMgr.h"
#include "../Effect/Effect.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include <iostream>
namespace tzw {
TZW_SINGLETON_IMPL(ModelLoader)

ModelLoader::ModelLoader()
{

}

void ModelLoader::loadModel(Model *model, std::string filePath)
{
	rapidjson::Document doc;
	auto data = Tfile::shared()->getData(filePath,true);
	auto relativeFilePath = Tfile::shared()->getReleativePath(filePath);
	auto folder = Tfile::shared()->getFolder(filePath);
	doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
	//判断读取成功与否 和 是否为数组类型
	if (doc.HasParseError())
	{
		printf("get json data err!");
		return;
	}

	//获得material
	auto matPool = MaterialPool::shared();
	auto tmgr = TextureMgr::shared();
	auto& materialList = doc["materialList"];
	auto mangleedName = matPool->getModelMangleedName(relativeFilePath);
	for (unsigned int i = 0;i<materialList.Size();i++)
	{
		auto & materialData = materialList[i];
		// reuse material
		auto mat = matPool->getMaterialByName(mangleedName);
		if (!mat)
		{
			mat = new Material();
			matPool->addMaterial(mangleedName,mat);
		}
		if(materialData.HasMember("effectType"))
		{
			mat->initFromEffect(materialData["effectType"].GetString());
		}else
		{
			mat->initFromEffect("ModelStd");
		}
		mat->setTex("diffuseMap",
						  tmgr->getByPath(Tfile::shared()->toAbsFilePath(materialData["diffuseMap"].GetString(), folder)));
		model->m_effectList.push_back(mat);
	}

	//获得Mesh
	auto& meshList = doc["MeshList"];
	for (unsigned int i = 0;i<meshList.Size();i++)
	{
		Mesh * theMesh = new Mesh();
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
		model->m_meshList.push_back(theMesh);
	}
}

} // namespace tzw
