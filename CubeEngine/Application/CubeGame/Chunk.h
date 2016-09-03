#ifndef CHUNK_H
#define CHUNK_H
#include "EngineSrc/CubeEngine.h"
#include "Block.h"
#include <string>
#include <vector>
#define MAX_BLOCK 16
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
    Block * getBlock(int x,int y,int z);
    virtual bool intersectByAABB(const AABB & other, vec3 &overLap);
    virtual Drawable3D * intersectByRay(const Ray & ray,vec3 &hitPoint);
    virtual void update(float delta);
    bool getIsAccpectOCTtree() const;
    virtual void draw();
    std::vector<Block *> m_blockList;
    void removeBlock(Block * block);
    void addBlock(std::string blockInfoName, int i, int j, int k);
    void load();
    void unload();
private:
    void relax();
    void verticesInfoClear();
    bool m_isLoaded;
    Technique * m_tech;
    Mesh * m_mesh;
    bool isInRange(int i,int j, int k);
    void reset();
    void generateBlocks();
    void finish();
    void sortByDist(vec3 pos);
    void addBlockToNeighbor(std::string blockInfoName,int i,int j,int k);
    bool isEmpty(int x,int y,int z);
    Block* m_blockGroup[MAX_BLOCK][MAX_BLOCK][MAX_BLOCK];
    int m_indicesGroup[MAX_BLOCK + 1][MAX_BLOCK + 1][MAX_BLOCK + 1];
    int getVerticesAt(int x, int y, int z);
    int addVertexAt(int x, int y, int z, VertexData vertex);
};
}


#endif // CHUNK_H
