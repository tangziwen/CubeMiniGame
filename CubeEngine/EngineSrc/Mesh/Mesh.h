#ifndef TZW_MESH_H
#define TZW_MESH_H

#include "VertexData.h"
#include <vector>
#include <QOpenGLBuffer>
#include "../Rendering/RenderBuffer.h"
#include "../Math/AABB.h"
#include "../Math/Matrix44.h"
namespace tzw {

class Mesh
{
public:
    Mesh();
    void addIndex(unsigned int index);
    void addIndices(unsigned short * index,int size);
    void addVertex(VertexData vertexData);
    void addVertices(VertexData * vertices,int size);
    void finish(bool isPassToGPU = true);

    GLuint vbo() const;
    void setVbo(const GLuint &vbo);

    GLuint ibo() const;
    void setIbo(const GLuint &ibo);
    size_t getIndicesSize();
    size_t getVerticesSize();
    RenderBuffer *getArrayBuf() const;
    RenderBuffer *getIndexBuf() const;
    void clearVertices();
    void clearIndices();
    AABB getAabb() const;
    void setAabb(const AABB &aabb);
    void calculateAABB();
    void merge(Mesh * other, const Matrix44 &transform);
    void createBufferObject();
    void clear();
private:
    void passToGPU();
    AABB m_aabb;
    RenderBuffer* m_arrayBuf;
    RenderBuffer* m_indexBuf;
    std::vector<GLushort> m_indices;
    std::vector<VertexData> m_vertices;
    GLuint m_vbo,m_ibo;
};

} // namespace tzw

#endif // TZW_MESH_H
