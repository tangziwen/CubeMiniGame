#include "Chunk.h"
#include "GameMap.h"
#include "GameWorld.h"
#include "algorithm"
#include "time.h"
#include "../EngineSrc/Texture/TextureMgr.h"
static int CHUNK_OFFSET  =BLOCK_SIZE * MAX_BLOCK / 2;
static int CHUNK_SIZE = BLOCK_SIZE * MAX_BLOCK;
namespace tzw {

static int v0[] = {7, 8, 19, };
static int v1[] = {0, 11, 16, };
static int v2[] = {1, 8, 15, };
static int v3[] = {6, 9, 12, };
static int v4[] = {4, 23, 18, };
static int v5[] = {17, 3, 20, };
static int v6[] = {2, 14, 21, };
static int v7[] = {5, 13, 22, };

Chunk::Chunk(int the_x, int the_y,int the_z)
    :m_isLoaded(false),x(the_x),y(the_y),z(the_z)
{
    reset();
    m_localAABB.update(vec3(0,0,0));
    m_localAABB.update(vec3(MAX_BLOCK * BLOCK_SIZE,MAX_BLOCK * BLOCK_SIZE,-1 * MAX_BLOCK * BLOCK_SIZE));
    setPos(vec3(x*CHUNK_SIZE,y* CHUNK_SIZE , -1 * z* CHUNK_SIZE));
    m_mesh = nullptr;
    m_tech = nullptr;
}

vec3 Chunk::getGridPos(int the_x, int the_y, int the_z)
{
    return vec3(x*MAX_BLOCK+the_x,y*MAX_BLOCK+the_y,-1 * (z*MAX_BLOCK+the_z));
}

Block *Chunk::getBlock(int x, int y, int z)
{
    return m_blockGroup[x][y][z];
}

bool Chunk::intersectByAABB(const AABB &other, vec3 &overLap)
{
    bool isHit =false;
    vec3 resultOverLap;
    for (Drawable3D * obj : m_blockList)
    {
        vec3 tmp;
        if(obj->intersectByAABB(other,tmp))
        {
            isHit = true;
            if(fabs(tmp.x) >fabs(resultOverLap.x))
            {
                resultOverLap.x = tmp.x;
            }
            if(fabs(tmp.y) >fabs(resultOverLap.y))
            {
                resultOverLap.y = tmp.y;
            }
            if(fabs(tmp.z) >fabs(resultOverLap.z))
            {
                resultOverLap.z = tmp.z;
            }
        }
    }
    if(isHit)
    {
        overLap = resultOverLap;
    }
    return isHit;
}

Drawable3D *Chunk::intersectByRay(const Ray &ray, vec3 &hitPoint)
{
    this->sortByDist(ray.origin());
    for(auto block : this->m_blockList)
    {
        if(block->intersectByRay(ray,hitPoint))
        {
            return block;
        }
    }
    return nullptr;
}

void Chunk::update(float delta)
{

}

bool Chunk::getIsAccpectOCTtree() const
{
    return true;
}

void Chunk::draw()
{
    if (!m_isLoaded) return;
    m_tech->applyFromDrawable(this);
    m_tech->setVar("TU_roughness",4.0f);
    RenderCommand command(m_mesh,m_tech,RenderCommand::RenderType::Common);
    Renderer::shared()->addRenderCommand(command);
}

void Chunk::removeBlock(Block *block)
{
    m_blockGroup[block->localGridX][block->localGridY][block->localGridZ] = nullptr;
    auto result = std::find(m_blockList.begin(),m_blockList.end(),block);
    if(result != m_blockList.end())
    {
        m_blockList.erase(result);
    }
    finish();
}

void Chunk::addBlock(std::string blockInfoName,int i,int j,int k)
{
    if(! isInRange(i,j,k))
    {
        //out of range,get neighbor chunk to add
        addBlockToNeighbor(blockInfoName,i,j,k);
        return;
    }
    auto block = Block::create(blockInfoName);
    block->grid_x = x*MAX_BLOCK + i;
    block->grid_y = y*MAX_BLOCK + j;
    block->grid_z = z*MAX_BLOCK + k;
    block->localGridX = i;
    block->localGridY = j;
    block->localGridZ = k;
    block->setPos(vec3(i*BLOCK_SIZE,j*BLOCK_SIZE, -k*BLOCK_SIZE ));
    block->setIsAccpectOCTtree(false);
    block->setParent(this);
    block->setIsDrawable(true);
    block->reCache();
    m_blockGroup[i][j][k] = block;
    m_blockList.push_back(block);
    this->finish();
}


void Chunk::load()
{
    if(m_isLoaded)return;
    m_isLoaded = true;
    setCamera(SceneMgr::shared()->currentScene()->defaultCamera());
    //强制重缓存数据
    reCache();
    if (!m_mesh)
    {
        m_mesh = new Mesh();
        m_tech = new Technique("./Res/EngineCoreRes/Shaders/VoxelTerrain_v.glsl","./Res/EngineCoreRes/Shaders/VoxelTerrain_f.glsl");
        m_tech->setTex("TU_tex1", TextureMgr::shared()->getOrCreateTexture("./Res/TestRes/rock_texture.png"), 0);
    }
    generateBlocks();
    finish();
    //

}

void Chunk::unload()
{
    m_isLoaded = false;
    m_mesh->clear();
}

void Chunk::relax()
{
    //use laplace relax
    for(int i =0;i<MAX_BLOCK + 1;i++)
    {
        for(int j=0;j<MAX_BLOCK + 1;j++)
        {
            for(int k=0;k<MAX_BLOCK + 1;k++)
            {
                if (m_indicesGroup[i][j][k] >= 0)
                {
                    int theCount = 0;
                    vec3 thePos;
                    int resultArray[9][3] = {{0,0,0},{-1,0,0},{1,0,0},{0,0,-1},{0,0,1},{-1,0,-1},{1,0,1},{-1,0,1},{1,0,-1}};
                    for (int m = 0; m < 9; m++)
                    {
                        auto x = i + resultArray[m][0];
                        auto y = j + resultArray[m][1];
                        auto z = k + resultArray[m][2];
                        if(getVerticesAt(x, y, z) >= 0)
                        {
                            thePos += m_mesh->getVertex(m_indicesGroup[x][y][z]).m_pos;
                            theCount += 1;
                        }
                    }
                    thePos = thePos / (theCount * 1.0f);
                    auto result = m_mesh->getVertex(m_indicesGroup[i][j][k]);
                    result.m_pos = thePos;
                    m_mesh->setVertex(m_indicesGroup[i][j][k],result);
                }
            }
        }
    }
}

void Chunk::verticesInfoClear()
{
    for(int i =0;i<MAX_BLOCK + 1;i++)
    {
        for(int j=0;j<MAX_BLOCK + 1;j++)
        {
            for(int k=0;k<MAX_BLOCK + 1;k++)
            {
                m_indicesGroup[i][j][k] = -1;
            }
        }
    }
}

bool Chunk::isInRange(int i, int j, int k)
{
    return i>=0 && i< MAX_BLOCK && j>=0 && j< MAX_BLOCK && k>=0 && k<MAX_BLOCK;
}

void Chunk::reset()
{
    for(int i =0;i<MAX_BLOCK;i++)
    {
        for(int j=0;j<MAX_BLOCK;j++)
        {
            for(int k=0;k<MAX_BLOCK;k++)
            {
                m_blockGroup[i][j][k] = nullptr;
            }
        }
    }
    verticesInfoClear();
}

void Chunk::generateBlocks()
{
    for(int i =0;i<MAX_BLOCK;i++)
    {
        for(int j=0;j<MAX_BLOCK;j++)
        {
            for(int k=0;k<MAX_BLOCK;k++)
            {
                if(GameMap::shared()->isBlock(this,i,j,k))
                {
                    Block * block = nullptr;
                    block = Block::create("mud.json");
                    if (GameMap::shared()->isSurface(this,i,j,k))
                    {
                        block = Block::create("grass.json");
                    }else
                    {
                        block = Block::create("mud.json");
                    }
                    block->grid_x = x*MAX_BLOCK + i;
                    block->grid_y = y*MAX_BLOCK + j;
                    block->grid_z = z*MAX_BLOCK + k;
                    block->localGridX = i;
                    block->localGridY = j;
                    block->localGridZ = k;
                    block->setPos(vec3(i*BLOCK_SIZE,j*BLOCK_SIZE,-k*BLOCK_SIZE ));
                    block->setParent(this);
                    block->reCache();
                    m_blockGroup[i][j][k] = block;
                    m_blockList.push_back(block);
                }else
                {
                    m_blockGroup[i][j][k] = nullptr;
                }
            }
        }
    }

}

void Chunk::finish()
{
    if (m_blockList.empty())
    {
        m_isLoaded = false;
        return;
    }
    auto m_oldTicks = clock();
    m_mesh->clear();
    verticesInfoClear();
    for(int i =0;i<MAX_BLOCK;i++)
    {
        for(int j=0;j<MAX_BLOCK;j++)
        {
            for(int k=0;k<MAX_BLOCK;k++)
            {
                auto block = m_blockGroup[i][j][k];
                if(block)
                {
                    //检查方块的内部可见性，避免大量重复的mesh
                    if(isEmpty(i,j+1,k) || isEmpty(i,j-1,k) ||isEmpty(i-1,j,k) || isEmpty(i+1,j,k) || isEmpty(i,j,k+1) || isEmpty(i,j,k-1))
                    {
                        auto localTransform = block->getLocalTransform();
                        auto info = block->info();
                        auto theMesh = block->info()->mesh();
                        auto vertices = theMesh->m_vertices;
                        if(isEmpty(i,j+1,k))//top
                        {
                            m_mesh->addIndex(addVertexAt(i, j+1, k, info->getVertex(1,localTransform)));
                            m_mesh->addIndex(addVertexAt(i + 1, j+1, k, info->getVertex(2,localTransform)));
                            m_mesh->addIndex(addVertexAt(i + 1, j+1, k + 1, info->getVertex(5,localTransform)));

                            m_mesh->addIndex(addVertexAt(i, j+1, k + 1, info->getVertex(6,localTransform)));
                            m_mesh->addIndex(addVertexAt(i, j+1, k, info->getVertex(1,localTransform)));
                            m_mesh->addIndex(addVertexAt(i + 1, j+1, k + 1, info->getVertex(5,localTransform)));
                        }

                        if(isEmpty(i,j-1,k))//bottom
                        {
                            m_mesh->addIndex(addVertexAt(i, j, k + 1, info->getVertex(7,localTransform)));
                            m_mesh->addIndex(addVertexAt(i +1, j, k + 1, info->getVertex(4,localTransform)));
                            m_mesh->addIndex(addVertexAt(i + 1, j, k, info->getVertex(3,localTransform)));

                            m_mesh->addIndex(addVertexAt(i, j, k, info->getVertex(0,localTransform)));
                            m_mesh->addIndex(addVertexAt(i, j, k + 1, info->getVertex(7,localTransform)));
                            m_mesh->addIndex(addVertexAt(i + 1, j, k, info->getVertex(3,localTransform)));
                        }

                        if(isEmpty(i-1,j,k))//left
                        {
                            m_mesh->addIndex(addVertexAt(i, j, k + 1, info->getVertex(7,localTransform)));
                            m_mesh->addIndex(addVertexAt(i, j, k, info->getVertex(0,localTransform)));
                            m_mesh->addIndex(addVertexAt(i, j + 1, k, info->getVertex(1,localTransform)));

                            m_mesh->addIndex(addVertexAt(i, j + 1, k + 1, info->getVertex(6,localTransform)));
                            m_mesh->addIndex(addVertexAt(i, j, k + 1, info->getVertex(7,localTransform)));
                            m_mesh->addIndex(addVertexAt(i, j + 1, k, info->getVertex(1,localTransform)));
                        }


                        if(isEmpty(i+1,j,k))//right
                        {
                            m_mesh->addIndex(addVertexAt(i + 1, j, k, info->getVertex(3,localTransform)));
                            m_mesh->addIndex(addVertexAt(i +1, j, k + 1, info->getVertex(4,localTransform)));
                            m_mesh->addIndex(addVertexAt(i + 1, j + 1, k + 1, info->getVertex(5,localTransform)));

                            m_mesh->addIndex(addVertexAt(i + 1, j + 1, k, info->getVertex(2,localTransform)));
                            m_mesh->addIndex(addVertexAt(i + 1, j, k, info->getVertex(3,localTransform)));
                            m_mesh->addIndex(addVertexAt(i + 1, j + 1, k + 1, info->getVertex(5,localTransform)));
                        }

                        if(isEmpty(i,j,k + 1))//back
                        {
                            m_mesh->addIndex(addVertexAt(i + 1, j, k + 1, info->getVertex(4,localTransform)));
                            m_mesh->addIndex(addVertexAt(i, j, k + 1, info->getVertex(7,localTransform)));
                            m_mesh->addIndex(addVertexAt(i, j + 1, k + 1, info->getVertex(6,localTransform)));

                            m_mesh->addIndex(addVertexAt(i + 1, j + 1, k + 1, info->getVertex(5,localTransform)));
                            m_mesh->addIndex(addVertexAt(i + 1, j, k + 1, info->getVertex(4,localTransform)));
                            m_mesh->addIndex(addVertexAt(i, j + 1, k + 1, info->getVertex(6,localTransform)));
                        }


                        if(isEmpty(i,j,k - 1))//front
                        {
                            m_mesh->addIndex(addVertexAt(i, j, k, info->getVertex(0,localTransform)));
                            m_mesh->addIndex(addVertexAt(i + 1, j, k, info->getVertex(3,localTransform)));
                            m_mesh->addIndex(addVertexAt(i + 1, j + 1, k, info->getVertex(2,localTransform)));

                            m_mesh->addIndex(addVertexAt(i, j + 1, k, info->getVertex(1,localTransform)));
                            m_mesh->addIndex(addVertexAt(i, j, k, info->getVertex(0,localTransform)));
                            m_mesh->addIndex(addVertexAt(i + 1, j + 1, k, info->getVertex(2,localTransform)));
                        }
//                        m_mesh->merge(vertices, theMesh->m_indices,block->getLocalTransform());
                    }
                }
            }
        }
    }
    if(!m_blockList.empty())
    {
        //m_tech->setTex("TU_tex1",GameWorld::shared()->getBlockSheet()->texture());
    }
    relax();
    m_mesh->caclNormals();
    m_mesh->finish();
    auto m_nowTicks = clock();
}

void Chunk::sortByDist(vec3 pos)
{
    std::sort(m_blockList.begin(),m_blockList.end(),
              [&](const Block * blockA, const Block * blockB)
    {
        auto gridPos = GameWorld::shared()->worldToGrid(pos);
        auto distA = std::abs(gridPos.x - blockA->grid_x) + std::abs(gridPos.y - blockA->grid_y) + std::abs(gridPos.z - blockA->grid_z);
        auto distB = std::abs(gridPos.x - blockB->grid_x) + std::abs(gridPos.y - blockB->grid_y) + std::abs(gridPos.z - blockB->grid_z);
        return distA < distB;
    }
    );
}

void Chunk::addBlockToNeighbor(std::string blockInfoName,int i, int j, int k)
{
    auto theX = this->x;
    auto theY = this->y;
    auto theZ = this->z;
    if(i<0)
    {
        theX-=1;
        i = MAX_BLOCK -1;
    }
    if(i>= MAX_BLOCK)
    {
        theX+=1;
        i = 0;
    }
    if(j<0)
    {
        theY-=1;
        j = MAX_BLOCK -1;
    }
    if(j>= MAX_BLOCK)
    {
        theY+=1;
        j = 0;
    }
    if(k<0)
    {
        theZ-=1;
        k = MAX_BLOCK -1;
    }
    if(k>= MAX_BLOCK)
    {
        theZ+=1;
        k = 0;
    }
    auto chunk = GameWorld::shared()->getChunkByChunk(theX,theY,theZ);
    if(chunk)
    {
        chunk->addBlock(blockInfoName,i,j,k);
    }else
    {
    }

}

bool Chunk::isEmpty(int x, int y, int z)
{
    if(x<0 ||x >=MAX_BLOCK || y<0 || y>=MAX_BLOCK || z<0 || z>=MAX_BLOCK)
    {
        return true;
    }
    return !m_blockGroup[x][y][z];
}

int Chunk::getVerticesAt(int x, int y, int z)
{
    if (x < 0 || x > MAX_BLOCK || y < 0 || y > MAX_BLOCK || z < 0 || z > MAX_BLOCK)
        return -1;
    return m_indicesGroup[x][y][z];
}

int Chunk::addVertexAt(int x, int y, int z, VertexData vertex)
{
    if(m_indicesGroup[x][y][z] >=0)
    {
        return m_indicesGroup[x][y][z];
    }else
    {
        m_mesh->addVertex(vertex);
        auto index = m_mesh->getVerticesSize() - 1;
        m_indicesGroup[x][y][z] = index;
        return index;
    }
}
}




