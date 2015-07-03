#include "CMC_Model.h"
#include "external/rapidjson/document.h"
#include "external/rapidjson/prettywriter.h"
#include "external/rapidjson/filewritestream.h"
#include "external/rapidjson/stringbuffer.h"
#include "external/rapidjson/document.h"
#include "external/TUtility/file/Tfile.h"
#include "external/TUtility/Data.h"
#include <iostream>
namespace tzw {

CMC_Model::CMC_Model()
{
    m_numBones = 0;
    m_rootBone = nullptr;
}

CMC_Model::~CMC_Model()
{

}



void CMC_Model::addMesh(CMC_Mesh *mesh)
{
    m_meshList.push_back (mesh);
}
QMatrix4x4 CMC_Model::globalInverseTransform() const
{
    return m_globalInverseTransform;
}

void CMC_Model::setGlobalInverseTransform(const QMatrix4x4 &globalInverseTransform)
{
    m_globalInverseTransform = globalInverseTransform;
}

void CMC_Model::writeToFile(const char *fielName)
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

void CMC_Model::addMaterial(CMC_Material *material)
{
    m_materialList.push_back (material);
}

void CMC_Model::loadFromTZW(const char *fileName)
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
        auto mesh = new CMC_Mesh();
        mesh->loadFromTZW (meshObj);
        addMesh (mesh);
    }
}
CMC_Bone *CMC_Model::rootBone() const
{
    return m_rootBone;
}

void CMC_Model::setRootBone(CMC_Bone *rootBone)
{
    m_rootBone = rootBone;
}


} // namespace tzw

