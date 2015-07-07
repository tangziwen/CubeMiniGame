#include "CMC_MeshData.h"

namespace tzw {

CMC_MeshData::CMC_MeshData()
{

}

CMC_MeshData::~CMC_MeshData()
{

}

CMC_Vertex *CMC_MeshData::getVertex(int index)
{
    return &m_vertices[index];
}

void CMC_MeshData::pushVertex(CMC_Vertex vertex)
{
    m_vertices.push_back (vertex);
}

void CMC_MeshData::pushIndex(unsigned short index)
{
    m_indices.push_back (index);
}

void CMC_MeshData::write(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.StartObject ();

    //material index
    writer.String ("material_index");
    writer.Int (m_materialIndex);

    //write vertices.
    writer.String ("vertices");
    writer.StartArray ();
    for(int i =0;i<m_vertices.size ();i++)
    {
        m_vertices[i].write (writer);
    }
    writer.EndArray ();

    //write indices.
    writer.String ("indices");
    writer.StartArray ();
    for(auto i : m_indices)
    {
        writer.Int (i);
    }
    writer.EndArray ();

    writer.EndObject ();
}
int CMC_MeshData::materialIndex() const
{
    return m_materialIndex;
}

void CMC_MeshData::setMaterialIndex(int materialIndex)
{
    m_materialIndex = materialIndex;
}

void CMC_MeshData::loadFromTZW(const rapidjson::Value &value)
{
    //load material index.
    m_materialIndex = value["material_index"].GetInt ();

    //load vertices
    auto& verticesObj = value["vertices"];
    for(auto iter = verticesObj.Begin ();iter!= verticesObj.End ();iter++)
    {
        auto& vertexObj = *iter;
        CMC_Vertex vertex;
        vertex.loadFromTZW (vertexObj);
        pushVertex (vertex);
    }

    //load indices
    auto& indicesObj = value["indices"];
    for(auto iter = indicesObj.Begin ();iter != indicesObj.End ();iter++)
    {
        auto& indexObj = * iter;
        pushIndex (indexObj.GetInt ());
    }
}


} // namespace tzw

