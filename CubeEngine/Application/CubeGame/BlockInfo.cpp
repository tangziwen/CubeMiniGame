#include "BlockInfo.h"
#include "GameWorld.h"
namespace tzw {

const float BlockSize = 1.0f;

BlockInfo::BlockInfo()
{

}

void BlockInfo::finish()
{
    std::string pre_fix = "./Res/User/CubeGame/texture/blocks/";
    auto blockSheet = GameWorld::shared()->getBlockSheet();
    m_topTexture = blockSheet->getFrame(m_topFile+ ".png");
    m_bottomTexture = blockSheet->getFrame(m_bottomFile+ ".png");
    m_sideTexture = blockSheet->getFrame(m_sideFile+ ".png");
    initMesh();
}


std::string BlockInfo::topFile() const
{
    return m_topFile;
}

void BlockInfo::setTopFile(const std::string &topFile)
{
    m_topFile = topFile;
}
std::string BlockInfo::sideFile() const
{
    return m_sideFile;
}

void BlockInfo::setSideFile(const std::string &sideFile)
{
    m_sideFile = sideFile;
}
std::string BlockInfo::bottomFile() const
{
    return m_bottomFile;
}

void BlockInfo::setBottomFile(const std::string &bottomFile)
{
    m_bottomFile = bottomFile;
}

TextureFrame *BlockInfo::topTexture() const
{
    return m_topTexture;
}

void BlockInfo::setTopTexture(TextureFrame *topTexture)
{
    m_topTexture = topTexture;
}

TextureFrame *BlockInfo::sideTexture() const
{
    return m_sideTexture;
}

void BlockInfo::setSideTexture(TextureFrame *sideTexture)
{
    m_sideTexture = sideTexture;
}

TextureFrame *BlockInfo::bottomTexture() const
{
    return m_bottomTexture;
}

void BlockInfo::setBottomTexture(TextureFrame *bottomTexture)
{
    m_bottomTexture = bottomTexture;
}

void BlockInfo::initMesh()
{
    VertexData vertices[] = {
        {vec3( 0, 0,  0), m_topTexture->UV(0.0f, 0.0f)}, // v0
        {vec3( 0,  BlockSize,  0), m_topTexture->UV(1.0f, 0.0f)},  // v1
        {vec3( BlockSize,  BlockSize,  0),m_topTexture->UV(1.0f, 1.0f)}, // v2
        {vec3( BlockSize, 0,  0), m_topTexture->UV(0.0f, 1.0f)},  // v3

        {vec3( BlockSize, 0,  -BlockSize), m_topTexture->UV(0.0f, 0.0f)}, // v4
        {vec3( BlockSize,  BlockSize,  -BlockSize), m_topTexture->UV(1.0f, 0.0f)},  // v5
        {vec3( 0,  BlockSize,  -BlockSize),m_topTexture->UV(1.0f, 1.0f)}, // v6
        {vec3(0, 0,  -BlockSize), m_topTexture->UV(0.0f, 1.0f)},  // v7

    };
    GLushort indices[] = {
         0,3,2,  1,0,2,//front
        3,4,5,  2,3,5,//right
        7,0,1,  6,7,1, //left
        1,2,5, 6,1,5, //top
       7,4,3, 0,7,3, //bottom
       4,7,6, 5,4,6// back
    };
    m_mesh = new Mesh();
    m_mesh->addVertices(vertices,sizeof(vertices)/sizeof(VertexData));
    m_mesh->addIndices(indices,sizeof(indices)/sizeof(GLushort));
    m_mesh->finish(false);//we don't pass it to the GPU
}

Mesh *BlockInfo::mesh() const
{
    return m_mesh;
}

VertexData BlockInfo::getVertex(int index, Matrix44 transform)
{
    auto vertex = m_mesh->m_vertices[index];
    vertex.m_pos = transform.transformVec3(vertex.m_pos);
    return vertex;
}




} // namespace tzw

