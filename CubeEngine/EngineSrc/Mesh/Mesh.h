#ifndef TZW_MESH_H
#define TZW_MESH_H

#include "VertexData.h"
#include <vector>
#include "../Rendering/RenderBuffer.h"
#include "../Math/AABB.h"
#include "../Math/Matrix44.h"
#include "../3D/Material/Material.h"
#include "../Engine/EngineDef.h"
namespace tzw {

class Triangle
{
public:
    enum TriangleMode { VERTICES =0, NORMALS =1, TEXTURE = 2};
    Triangle ();

    int containsEdge ( int n1, int n2 ) const;

    int v[3];	// index to vertices
    int n[3];	// index to normals
    int t[3];	// index to triangle neighbours

    int findNeighborIndex (int) const;
    int getVertexIndex (int) const;

    int mode;
};

class SplitData
{
public:
    SplitData ( int i, int c, int n, int _t1, int _t2 ) : index(i), callee(c), newPoint(n), t1(_t1), t2(_t2) {}
    int index, callee, newPoint, t1,t2;
};

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
    void merge(std::vector<VertexData> & vertices, std::vector<short_u> & indices, const Matrix44 &transform);
    void createBufferObject();
    void clear();
    Material *getMat() const;
    void setMat(Material *mat);
    void caclNormals();
    VertexData getVertex(unsigned int index);
    void setVertex(unsigned int index, VertexData vertex);
    std::vector<short_u> m_indices;
    std::vector<VertexData> m_vertices;
    void subDivide(int level = 1);
    void cloneFrom(Mesh * other);
private:
    void triangleSplit ( int index, int callee, int newPoint, int t1, int t2 );
    void computeNewVerts ();
    void findAdjTriangles ();
    std::vector<Triangle>		triangles_;
    std::vector<bool>			processed_;
    std::vector<VertexData>		newverts_;
    std::vector<int> midPoints_;
    std::vector<int> midTriangles_;
    std::vector<Triangle> tmpTriangles_;
    std::vector<SplitData> splits_;

    void subDivideIter();
    void passToGPU();
    AABB m_aabb;
    RenderBuffer* m_arrayBuf;
    RenderBuffer* m_indexBuf;

    Material * m_mat;
    GLuint m_vbo,m_ibo;
};

} // namespace tzw

#endif // TZW_MESH_H
