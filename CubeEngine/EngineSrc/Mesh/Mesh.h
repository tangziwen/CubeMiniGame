#ifndef TZW_MESH_H
#define TZW_MESH_H

#include "VertexData.h"
#include <vector>
#include "../Rendering/RenderBuffer.h"
#include "../Math/AABB.h"
#include "../Math/Matrix44.h"
#include "../Engine/EngineDef.h"
#include "../Math/Ray.h"
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

#pragma pack(push,1)
struct InstanceData
{
	vec4 posAndScale;
	vec4 extraInfo;
	vec4 rotateInfo;
};
#pragma pack(pop)

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

    integer_u vbo() const;
    void setVbo(const integer_u &vbo);

    integer_u ibo() const;
    void setIbo(const integer_u &ibo);
    size_t getIndicesSize();
    size_t getVerticesSize();
    RenderBuffer *getArrayBuf() const;
    RenderBuffer *getIndexBuf() const;
	RenderBuffer *getInstanceBuf() const;
    void clearVertices();
    void clearIndices();
    AABB getAabb() const;
    void setAabb(const AABB &aabb);
    void calculateAABB();
    void merge(Mesh * other, const Matrix44 &transform);
    void merge(std::vector<VertexData> & vertices, std::vector<short_u> & indices, const Matrix44 &transform);
    void createBufferObject();
    void clear();
    void caclNormals();
    void calBaryCentric();
    VertexData getVertex(unsigned int index);
    void setVertex(unsigned int index, VertexData vertex);
    std::vector<short_u> m_indices;
    std::vector<VertexData> m_vertices;
    void subDivide(int level = 1);
    void cloneFrom(Mesh * other);
    void setMatIndex(unsigned int matIndex);

    unsigned int getMatIndex() const;
	unsigned int getIndex(int id);
	int getInstanceSize();
	bool isEmpty();
	void pushInstance(InstanceData instanceData);
	void pushInstances(std::vector<InstanceData> instancePos);
	void clearInstances();
	void calcTangents();
	void submitInstanced(int preserveNumber = 0);
	void reSubmitInstanced();
	void reSubmit();
	void submitOnlyVO_IO();
	bool intersectWithRay(const Ray & rayInMeshSpace, vec3 * hitPoint);
	void submit(RenderFlag::BufferStorageType storageType = RenderFlag::BufferStorageType::STATIC_DRAW);
	~Mesh();
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
	std::vector<InstanceData> m_instanceOffset;

    void subDivideIter();
    
    AABB m_aabb;
    RenderBuffer* m_arrayBuf;
    RenderBuffer* m_indexBuf;
	RenderBuffer* m_instanceBuf;

    unsigned int m_matIndex;
    integer_u m_vbo,m_ibo;
};

} // namespace tzw

#endif // TZW_MESH_H
