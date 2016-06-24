#include "ModelLoader.h"
#include "Model.h"
namespace tzw {
TZW_SINGLETON_IMPL(ModelLoader)

ModelLoader::ModelLoader()
{

}

void ModelLoader::loadModel(Model *model, std::string filePath)
{
    rapidjson::Document doc;
    auto data = Tfile::getInstance()->getData(filePath,true);
    doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
    //判断读取成功与否 和 是否为数组类型
    if (doc.HasParseError())
    {
        printf("get json data err!");
        return;
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
            auto& vertexData_j = verticesData[k];
            theMesh->addVertex(VertexData(vec3(vertexData_j[0].GetDouble(),vertexData_j[1].GetDouble(),vertexData_j[2].GetDouble()),
                    vec2(vertexData_j[6].GetDouble(),vertexData_j[7].GetDouble())));
        }
        //get indices
        auto &indicesData = meshData["indices"];
        for(unsigned int k = 0; k < indicesData.Size();k++)
        {
            theMesh->addIndex(indicesData[k].GetInt());
        }
        theMesh->finish(true);
        model->m_meshList.push_back(theMesh);
    }
}

} // namespace tzw
