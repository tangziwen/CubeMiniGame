#include "Chunk.h"
#include "GameMap.h"
#include "GameWorld.h"
#include "algorithm"
#include "time.h"
static int CHUNK_OFFSET  =BLOCK_SIZE * MAX_BLOCK / 2;
static int CHUNK_SIZE = BLOCK_SIZE * MAX_BLOCK;
namespace tzw {
Chunk::Chunk(int the_x, int the_y,int the_z)
    :m_isLoaded(false),x(the_x),y(the_y),z(the_z)
{
    m_localAABB.update(vec3(0,0,0));
    m_localAABB.update(vec3(MAX_BLOCK * BLOCK_SIZE,MAX_BLOCK * BLOCK_SIZE,MAX_BLOCK * BLOCK_SIZE));
    setPos(vec3(x*CHUNK_SIZE,y* CHUNK_SIZE , z* CHUNK_SIZE));
    m_mesh = nullptr;
    m_tech = nullptr;
}

vec3 Chunk::getGridPos(int the_x, int the_y, int the_z)
{
    return vec3(x*MAX_BLOCK+the_x,y*MAX_BLOCK+the_y,z*MAX_BLOCK+the_z);
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
    auto camera = SceneMgr::shared()->currentScene()->defaultCamera();
    auto vp = camera->getViewProjectionMatrix();
    auto m = getTransform();
    m_tech->setVar("TU_mvpMatrix", vp* m);
    m_tech->setVar("TU_color",m_uniformColor);
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
    block->setPos(vec3(i*BLOCK_SIZE,j*BLOCK_SIZE,k*BLOCK_SIZE ));
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
    //强制重缓存数据
    reCache();
    if (!m_mesh)
    {
        m_mesh = new Mesh();
        m_tech = new Technique("./Res/EngineCoreRes/Shaders/Simple_v.glsl","./Res/EngineCoreRes/Shaders/Simple_f.glsl");
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
                    block->setPos(vec3(i*BLOCK_SIZE,j*BLOCK_SIZE,k*BLOCK_SIZE ));
                    block->setIsAccpectOCTtree(false);
                    block->setParent(this);
                    block->setIsDrawable(true);
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
    for(int i =0;i<MAX_BLOCK;i++)
    {
        for(int j=0;j<MAX_BLOCK;j++)
        {
            for(int k=0;k<MAX_BLOCK;k++)
            {
                auto block = m_blockGroup[i][j][k];
                if(!block) continue;
                //检查方块的内部可见性，避免大量重复的mesh
                if(isEmpty(i,j+1,k) || isEmpty(i,j-1,k) ||isEmpty(i-1,j,k) || isEmpty(i+1,j,k) || isEmpty(i,j,k+1) || isEmpty(i,j,k-1))
                {
                    m_mesh->merge(block->info()->mesh(),block->getLocalTransform());
                }
            }
        }
    }
    if(!m_blockList.empty())
    {
        m_tech->setTex("TU_tex1",GameWorld::shared()->getBlockSheet()->texture());
    }
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
}




