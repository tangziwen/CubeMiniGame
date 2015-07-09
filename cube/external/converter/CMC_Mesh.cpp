#include "CMC_Mesh.h"

namespace tzw {

CMC_Mesh::CMC_Mesh()
{

}

CMC_Mesh::~CMC_Mesh()
{

}

CMC_Vertex *CMC_Mesh::getVertex(int index)
{
    return &m_vertices[index];
}

void CMC_Mesh::pushVertex(CMC_Vertex vertex)
{
    m_vertices.push_back (vertex);
}

void CMC_Mesh::pushIndex(unsigned short index)
{
    m_indices.push_back (index);
}

void CMC_Mesh::write(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
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
int CMC_Mesh::materialIndex() const
{
    return m_materialIndex;
}

void CMC_Mesh::setMaterialIndex(int materialIndex)
{
    m_materialIndex = materialIndex;
}

void CMC_Mesh::loadFromTZW(const rapidjson::Value &value)
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

