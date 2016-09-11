#include "Chunk.h"
#include "GameMap.h"
#include "GameWorld.h"
#include "algorithm"
#include "time.h"
#include "../EngineSrc/Texture/TextureMgr.h"
#include "../EngineSrc/3D/Terrain/MarchingCubes.h"
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
    m_basePoint = vec3(x*CHUNK_SIZE,y* CHUNK_SIZE , -1 * z* CHUNK_SIZE);
    setPos(m_basePoint);
    m_mesh = nullptr;
    m_tech = nullptr;
    m_needToUpdate = true;
    reCacheAABB();
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
    if(this->hitFirst(ray, hitPoint))
    {
        return this;
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
    auto vp = camera()->getViewProjectionMatrix();
    auto v = camera()->getViewMatrix();
    Matrix44 m;
    m.setToIdentity();//no need to pass world transformation.
    m_tech->setVar("TU_mvpMatrix", vp* m);
    m_tech->setVar("TU_vpMatrix", vp);
    m_tech->setVar("TU_mvMatrix", v * m);
    m_tech->setVar("TU_vMatrix", v);

    m_tech->setVar("TU_mMatrix", m);
    m_tech->setVar("TU_normalMatrix", (v * m).inverted().transpose());
    m_tech->setVar("TU_color",getUniformColor());
    m_tech->setVar("TU_roughness",4.0f);

    RenderCommand command(m_mesh,m_tech,RenderCommand::RenderType::Common);
    command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
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
        m_tech = new Technique("./Res/EngineCoreRes/Shaders/VoxelTerrain_v.glsl", "./Res/EngineCoreRes/Shaders/VoxelTerrain_f.glsl");
        m_tech->setTex("TU_tex1", TextureMgr::shared()->getOrCreateTexture("./Res/TestRes/rock_texture.png"), 0);
    }
    mcPoints = new vec4[(MAX_BLOCK +1) * (MAX_BLOCK+1) * (MAX_BLOCK + 1)];
    int YtimeZ = (MAX_BLOCK + 1) * (MAX_BLOCK + 1);
    for(int i =0;i<MAX_BLOCK + 1;i++)
    {
        for(int j=0;j<MAX_BLOCK + 1;j++)
        {
            for(int k=0;k<MAX_BLOCK + 1;k++)
            {
                vec4 verts[8];
                verts[0] = vec4(i * BLOCK_SIZE, j * BLOCK_SIZE, -1 * k * BLOCK_SIZE, -1) + vec4(m_basePoint, 0);
                if ( GameMap::shared()->isSurface(verts[0].toVec3()))
                {
                    verts[0].w = 1;
                }
                else
                {
                    verts[0].w = -1;
                }
                //x y z
                int ind = i*YtimeZ + j*(MAX_BLOCK + 1) + k;
                mcPoints[ind] = verts[0];
            }
        }
    }
    finish();
    //

}

void Chunk::unload()
{
    m_isLoaded = false;
    m_mesh->clear();
}

void Chunk::deformAround(vec3 pos, float value)
{
    vec3 relativePost = pos - m_basePoint;
    relativePost = relativePost / BLOCK_SIZE;
    relativePost.z *= -1;
    int posX = relativePost.x;
    int posY = relativePost.y;
    int posZ = relativePost.z;
    for (int i = -1; i<=1; i++)
    {
        for(int j = -1; j<=1; j++)
        {
            for(int k = -1; k<=1; k++)
            {
                addVoexlScalar(posX + i, posY + j, posZ + k, value);
            }
        }
    }
    finish();
}

void Chunk::setVoxelScalar(int x, int y, int z, float scalar)
{
    int YtimeZ = (MAX_BLOCK + 1) * (MAX_BLOCK + 1);
    int ind = x*YtimeZ + y*(MAX_BLOCK + 1) + z;
    mcPoints[ind].w = scalar;
}

void Chunk::addVoexlScalar(int x, int y, int z, float scalar)
{
    int YtimeZ = (MAX_BLOCK + 1) * (MAX_BLOCK + 1);
    int ind = x*YtimeZ + y*(MAX_BLOCK + 1) + z;
    mcPoints[ind].w += scalar;
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

}

void Chunk::finish()
{
    MarchingCubes::shared()->generate(m_mesh, MAX_BLOCK, MAX_BLOCK, MAX_BLOCK, mcPoints, 0.0f);
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
bool Chunk::isEmpty(int x, int y, int z)
{
    if(x<0 ||x >=MAX_BLOCK || y<0 || y>=MAX_BLOCK || z<0 || z>=MAX_BLOCK)
    {
        return true;
    }
    return !m_blockGroup[x][y][z];
}

bool Chunk::hitAny(Ray &ray, vec3 &result)
{
    auto size = m_mesh->getIndicesSize();
    float t = 0;
    for (auto i =0; i< size; i+=3)
    {
        if(ray.intersectTriangle(m_mesh->m_vertices[i].m_pos,m_mesh->m_vertices[i + 1].m_pos,m_mesh->m_vertices[i + 2].m_pos, &t))
        {
            result = ray.origin() + ray.direction() * t;
            return true;
        }
    }
    return false;
}

bool Chunk::hitFirst(const Ray &ray, vec3 &result)
{
    if(!m_isLoaded)
        return false;
    auto size = m_mesh->getIndicesSize();
    std::vector<vec3> resultList;
    float t = 0;
    for (auto i =0; i< size; i+=3)
    {
        if(ray.intersectTriangle(m_mesh->m_vertices[i].m_pos,m_mesh->m_vertices[i + 1].m_pos,m_mesh->m_vertices[i + 2].m_pos, &t))
        {
            result = ray.origin() + ray.direction() * t;
            resultList.push_back(result);
        }
    }
    if(!resultList.empty())
    {
        std::sort(resultList.begin(),resultList.end(),[ray](const vec3 & v1, const vec3 & v2)    {
            float dist1 = ray.origin().distance(v1);
            float dist2 = ray.origin().distance(v2);
            return dist1<dist2;
        });
        result = resultList[0];
        return true;
    }
    return false;
}

}




