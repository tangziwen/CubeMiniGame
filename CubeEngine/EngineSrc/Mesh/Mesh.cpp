#include "Mesh.h"

namespace tzw {

Mesh::Mesh()
    :m_ibo(-1),m_vbo(-1),m_mat(nullptr)
{
    m_arrayBuf = new RenderBuffer(RenderBuffer::Type::VERTEX);
    m_indexBuf = new RenderBuffer(RenderBuffer::Type::INDEX);
}

void Mesh::addIndex(unsigned int index)
{
    m_indices.push_back(index);
}

void Mesh::addIndices(unsigned short *index, int size)
{
    for(int i = 0; i<size; i++)
    {
        addIndex(index[i]);
    }
}

void Mesh::addVertex(VertexData vertexData)
{
    m_vertices.push_back(vertexData);
}

void Mesh::addVertices(VertexData *vertices, int size)
{
    for(int i = 0; i<size; i++)
    {
        addVertex(vertices[i]);
    }
}

void Mesh::finish(bool isPassToGPU)
{
    calculateAABB();
    if(isPassToGPU)
    {
        passToGPU();
    }
}

void Mesh::passToGPU()
{
    if(m_ibo == -1)
    {
        createBufferObject();
    }
    //pass data to the VBO
    m_arrayBuf->use();
    m_arrayBuf->allocate(&m_vertices[0], m_vertices.size() * sizeof(VertexData));

    //pass data to the IBO
    m_indexBuf->use();
    m_indexBuf->allocate(&m_indices[0], m_indices.size() * sizeof(GLushort));


}

Material *Mesh::getMat() const
{
    return m_mat;
}

void Mesh::setMat(Material *mat)
{
    m_mat = mat;
}

void Mesh::caclNormals()
{
    size_t indexCount = m_indices.size();
    // Accumulate each triangle normal into each of the triangle vertices
    for (unsigned int i = 0 ; i < indexCount ; i += 3) {
        short_u index0 = m_indices[i];
        short_u index1 = m_indices[i + 1];
        short_u index2 = m_indices[i + 2];
        vec3 v1 = m_vertices[index1].m_pos - m_vertices[index0].m_pos;
        vec3 v2 = m_vertices[index2].m_pos - m_vertices[index0].m_pos;
        vec3 normal = vec3::CrossProduct(v1,v2);
        normal.normalize();

        m_vertices[index0].m_normal += normal;
        m_vertices[index1].m_normal += normal;
        m_vertices[index2].m_normal += normal;
    }

    size_t vertexCount = m_vertices.size();
    // Normalize all the vertex normals
    for (unsigned int i = 0 ; i < vertexCount ; i++) {
        m_vertices[i].m_normal.normalize();
    }
}

VertexData Mesh::getVertex(unsigned int index)
{
    return m_vertices[index];
}

void Mesh::setVertex(unsigned int index, VertexData vertex)
{
    m_vertices[index] = vertex;
}

GLuint Mesh::vbo() const
{
    return m_vbo;
}

void Mesh::setVbo(const GLuint &vbo)
{
    m_vbo = vbo;
}
GLuint Mesh::ibo() const
{
    return m_ibo;
}

void Mesh::setIbo(const GLuint &ibo)
{
    m_ibo = ibo;
}

size_t Mesh::getIndicesSize()
{
    return m_indices.size();
}

size_t Mesh::getVerticesSize()
{
    return m_vertices.size();
}

RenderBuffer *Mesh::getArrayBuf() const
{
    return m_arrayBuf;
}
RenderBuffer *Mesh::getIndexBuf() const
{
    return m_indexBuf;
}

void Mesh::clearVertices()
{
    m_vertices.clear();
}

void Mesh::clearIndices()
{
    m_indices.clear();
}
AABB Mesh::getAabb() const
{
    return m_aabb;
}

void Mesh::setAabb(const AABB &aabb)
{
    m_aabb = aabb;
}

void Mesh::calculateAABB()
{
    for(int i=0;i<m_vertices.size();i++)
    {
        m_aabb.update(m_vertices[i].m_pos);
    }
}

void Mesh::merge(Mesh *other, const Matrix44 &transform)
{
    auto vOffset = m_vertices.size();
    //merge vertex
    for(auto vertext : other->m_vertices)
    {
        //transform by matrix
        auto pos = vec4(vertext.m_pos.x,vertext.m_pos.y,vertext.m_pos.z,1.0);
        auto resultPos = transform * pos;
        vertext.m_pos = vec3(resultPos.x,resultPos.y,resultPos.z);

        m_vertices.push_back(vertext);
    }

    //merge indices
    for(auto index : other->m_indices)
    {
        m_indices.push_back(index+vOffset);
    }
}

void Mesh::merge(std::vector<VertexData> &vertices, std::vector<short_u> &indices, const Matrix44 &transform)
{
    auto vOffset = m_vertices.size();
    //merge vertex
    for(auto vertext : vertices)
    {
        //transform by matrix
        auto pos = vec4(vertext.m_pos.x,vertext.m_pos.y,vertext.m_pos.z,1.0);
        auto resultPos = transform * pos;
        vertext.m_pos = vec3(resultPos.x,resultPos.y,resultPos.z);

        m_vertices.push_back(vertext);
    }

    //merge indices
    for(auto index : indices)
    {
        m_indices.push_back(index+vOffset);
    }
}

void Mesh::createBufferObject()
{
    //create vbo and ibo buffer
    m_arrayBuf->create();
    m_indexBuf->create();

    //record the handle
    m_ibo = m_indexBuf->bufferId();
    m_vbo = m_arrayBuf->bufferId();
}

void Mesh::clear()
{
    this->clearIndices();
    this->clearVertices();
}





} // namespace tzw

