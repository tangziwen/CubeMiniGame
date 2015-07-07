#include "CMC_ModelData.h"
#include "external/rapidjson/document.h"
#include "external/rapidjson/prettywriter.h"
#include "external/rapidjson/filewritestream.h"
#include "external/rapidjson/stringbuffer.h"
#include "external/rapidjson/document.h"
#include "external/TUtility/file/Tfile.h"
#include "external/TUtility/Data.h"
#include <iostream>
namespace tzw {

CMC_ModelData::CMC_ModelData()
{
    m_numBones = 0;
    m_rootBone = nullptr;
}

CMC_ModelData::~CMC_ModelData()
{

}



void CMC_ModelData::addMesh(CMC_MeshData *mesh)
{
    m_meshList.push_back (mesh);
}
QMatrix4x4 CMC_ModelData::globalInverseTransform() const
{
    return m_globalInverseTransform;
}

void CMC_ModelData::setGlobalInverseTransform(const QMatrix4x4 &globalInverseTransform)
{
    m_globalInverseTransform = globalInverseTransform;
}

void CMC_ModelData::writeToFile(const char *fielName)
{
    printf("will write model data to %s.\n",fielName);
    printf("writing...\n");
    rapidjson::StringBuffer s;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);

    writer.StartObject();
    //the fucking thing.
    writer.String ("version");
    writer.String ("caonima");

    //materials
    writer.String("materials");
    writer.StartArray ();
    for(int i =0;i<m_materialList.size ();i++)
    {
        m_materialList[i]->write(writer);
    }
    writer.EndArray ();

    //meshes
    writer.String("meshes");
    writer.StartArray ();
    for(int i =0;i<m_meshList.size ();i++)
    {
        m_meshList[i]->write(writer);
    }

    writer.EndArray ();
    writer.EndObject();
    s.ShrinkToFit ();
    FILE * f = fopen(fielName,"w");
    fprintf(f,s.GetString ());
    fclose(f);
    printf("Done.\n");
}

void CMC_ModelData::addMaterial(CMC_Material *material)
{
    m_materialList.push_back (material);
}

void CMC_ModelData::loadFromTZW(const char *fileName)
{

    // load All materials
    auto data = tzw::Tfile::getInstance()->getData (fileName,true);
    rapidjson::Document d;
    d.Parse((const char *)data.getBytes ());

    auto& materialsObj= d["materials"];
    for(auto iter = materialsObj.Begin ();iter!=materialsObj.End ();iter++)
    {
        auto& materialObj = *iter;
        auto material = new CMC_Material();
        material->loadFromTZW (materialObj);
        addMaterial (material);
    }

    //load all meshes.
    auto& meshesObj = d["meshes"];
    for(auto iter = meshesObj.Begin ();iter!= meshesObj.End ();iter++)
    {
        auto& meshObj = *iter;
        auto mesh = new CMC_MeshData();
        mesh->loadFromTZW (meshObj);
        addMesh (mesh);
    }
}
CMC_Node *CMC_ModelData::rootBone() const
{
    return m_rootBone;
}

void CMC_ModelData::setRootBone(CMC_Node *rootBone)
{
    m_rootBone = rootBone;
}


} // namespace tzw

