#include "ModelLoader.h"
#include "Model.h"
#include "../../Texture/TextureMgr.h"
#include "../Effect/Effect.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include <iostream>
#include "Utility/file/Tfile.h"
#include "Utility/misc/Tmisc.h"

namespace tzw {
TZW_SINGLETON_IMPL(ModelLoader)

ModelLoader::ModelLoader()
{

}

void ModelLoader::loadModel(Model *model, std::string filePath)
{
	rapidjson::Document doc;
	auto data = Tfile::shared()->getData(Tmisc::getUserPath(filePath),true);
	auto relativeFilePath = Tfile::shared()->getReleativePath(filePath);
	auto folder = Tfile::shared()->getFolder(filePath);
	doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
	if (doc.HasParseError())
	{
		tlogError("get json data err!");
		return;
	}

	//get the material
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
			mat->loadFromTemplate(materialData["effectType"].GetString());
		}else
		{
			mat->loadFromTemplate("ModelStd");
		}
		auto thestr = materialData["diffuseMap"].GetString();
		if (strcmp(thestr, "") != 0)
		{
			mat->setTex("diffuseMap",
				tmgr->getByPath(Tfile::shared()->toAbsFilePath(materialData["diffuseMap"].GetString(), folder)));
		}
		model->m_effectList.push_back(mat);
	}

	//get the Mesh
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
