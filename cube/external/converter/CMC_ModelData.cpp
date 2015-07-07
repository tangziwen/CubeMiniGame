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

CMC_Node *CMC_ModelData::findNodeByName(std::string nodeName)
{
    return findNodeByNameRecursively(m_rootBone,nodeName);
}

bool CMC_ModelData::isBone(std::string nodeName)
{
    if(this->m_BoneMetaInfoMapping.find (nodeName)!=this->m_BoneMetaInfoMapping.end ())
    {
        return true;
    }else
    {
        return false;
    }
}

int CMC_ModelData::findAnimateIndex(std::string animateName)
{
    for(int i =0;i<m_animates.size ();i++)
    {
        if(m_animates[i]->m_name == animateName)
        {
            return i;
        }
    }
    return -1;
}


CMC_Node *CMC_ModelData::findNodeByNameRecursively(CMC_Node *node, std::string nodeName)
{
    if(node->info ()->name ().compare (nodeName)==0)
    {
        return node;
    }
    for(int i =0;i<node->m_children.size ();i++)
    {
        auto result = findNodeByNameRecursively (node->m_children[i],nodeName);
        if(result)
        {
            return result;
        }
    }
    return nullptr;
}
bool CMC_ModelData::hasAnimation() const
{
    return m_hasAnimation;
}

std::vector<CMC_Material *> &CMC_ModelData::materialList()
{
    return m_materialList;
}

std::vector<CMC_MeshData *> &CMC_ModelData::meshList()
{
    return m_meshList;
}

std::vector<CMC_AnimateData *> &CMC_ModelData::animates()
{
    return m_animates;
}



} // namespace tzw

