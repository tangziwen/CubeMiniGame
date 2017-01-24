#ifndef CHUNK_H
#define CHUNK_H
#include "EngineSrc/CubeEngine.h"
#include <string>
#include <vector>

struct vertexInfo
{
    unsigned int index;
};

namespace tzw
{
class Chunk : public Drawable3D
{
public:
    Chunk(int the_x, int the_y, int the_z);
    int x;
    int y;
    int z;
    vec3 getGridPos(int the_x, int the_y, int the_z);
    virtual bool intersectByAABB(const AABB & other, vec3 &overLap);
    virtual Drawable3D * intersectByRay(const Ray & ray,vec3 &hitPoint);
    virtual bool intersectBySphere(const t_Sphere & sphere, std::vector<vec3> & hitPoint);
    virtual void logicUpdate(float delta);
    bool getIsAccpectOCTtree() const;
    virtual void submitDrawCmd();
    void load();
    void unload();
    void deformAround(vec3 pos, float value, float range = 1.0f);
    void deformWithNeighbor(int X, int Y, int Z, float value);
    void setVoxelScalar(int x, int y, int z, float scalar);
    void addVoexlScalar(int x, int y, int z, float scalar);
    void genNormal();
    vec4 getPoint(int index);
    vec4 getPoint(int x, int y, int z);
    int getIndex(int x, int y, int z);
    void genMesh();
    void initData();
    virtual void checkCollide(ColliderEllipsoid * package);
    virtual void setUpTransFormation(TransformationInfo & info);
	void setLod(unsigned int newLod);
	unsigned int getLod();
private:
    bool m_isLoaded;
    bool m_isInitData;
    Mesh * m_mesh;
	bool isInEdge(int i, int j, int k);
    bool isInRange(int i,int j, int k);
    bool isInOutterRange(int i, int j, int k);
    bool isInInnerRange(int i, int j, int k);
    bool isEdge(int i);

    bool hitAny(Ray & ray, vec3 & result);
    bool hitFirst(const Ray &ray, vec3 & result);
    vec4 * mcPoints;
    vec3 m_basePoint;
    std::vector<Chunk *> m_tmpNeighborChunk;
	unsigned int m_lod;
	bool m_isNeedSubmitMesh;
};
}


#endif // CHUNK_H
