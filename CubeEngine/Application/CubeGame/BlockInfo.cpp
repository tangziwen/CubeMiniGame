#include "BlockInfo.h"
#include "GameWorld.h"
namespace tzw {

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
        //top
        {vec3( -1.0f, 1.0f,  1.0f), m_topTexture->UV(0.0f, 0.0f)}, // v1
        {vec3( 1.0f,  1.0f,  1.0f), m_topTexture->UV(1.0f, 0.0f)},  // v2
        {vec3( 1.0f,  1.0f,  -1.0f),m_topTexture->UV(1.0f, 1.0f)}, // v3
        {vec3(-1.0f, 1.0f,  -1.0f), m_topTexture->UV(0.0f, 1.0f)},  // v0

        //bottom
        {vec3(-1.0f, -1.0f,  -1.0f), m_bottomTexture->UV(0.0f, 1.0f)},  // v0
        {vec3( 1.0f,  -1.0f,  -1.0f),m_bottomTexture->UV(1.0f, 1.0f)}, // v3
        {vec3( 1.0f,  -1.0f,  1.0f), m_bottomTexture->UV(1.0f, 0.0f)},  // v2
        {vec3( -1.0f, -1.0f,  1.0f), m_bottomTexture->UV(0.0f, 0.0f)}, // v1

        //----side---

        // front
        {vec3(-1.0f, -1.0f,  1.0f), m_sideTexture->UV(0.0f, 0.0f)},  // v0
        {vec3( 1.0f,  -1.0f,  1.0f), m_sideTexture->UV(1.0f, 0.0f)}, // v1
        {vec3( 1.0f,  1.0f,  1.0f), m_sideTexture->UV(1.0f, 1.0f)},  // v2
        {vec3( -1.0f, 1.0f,  1.0f), m_sideTexture->UV(0.0f, 1.0f)}, // v3

        // right
        {vec3(1.0f, -1.0f,  1.0f), m_sideTexture->UV(0.0f, 0.0f)},  // v4
        {vec3( 1.0f,  -1.0f,  -1.0f), m_sideTexture->UV(1.0f, 0.0f)}, // v5
        {vec3( 1.0f,  1.0f,  -1.0f), m_sideTexture->UV(1.0f, 1.0f)},  // v6
        {vec3( 1.0f, 1.0f,  1.0f), m_sideTexture->UV(0.0f, 1.0f)}, // v7

        // left
        {vec3( -1.0f, 1.0f,  1.0f), m_sideTexture->UV(1.0f, 1.0f)}, // v8
        {vec3( -1.0f,  1.0f,  -1.0f), m_sideTexture->UV(0.0f, 1.0f)},  // v9
        {vec3( -1.0f,  -1.0f,  -1.0f), m_sideTexture->UV(0.0f, 0.0f)}, // v10
        {vec3( -1.0f, -1.0f,  1.0f), m_sideTexture->UV(1.0f, 0.0f)},  // v11

        //back
        {vec3( -1.0f, 1.0f,  -1.0f), m_sideTexture->UV(1.0f, 1.0f)}, // v12
        {vec3( 1.0f,  1.0f,  -1.0f), m_sideTexture->UV(0.0f, 1.0f)},  // v13
        {vec3( 1.0f,  -1.0f,  -1.0f), m_sideTexture->UV(0.0f, 0.0f)}, // v14
        {vec3(-1.0f, -1.0f,  -1.0f), m_sideTexture->UV(1.0f, 0.0f)},  // v15

    };
    GLushort indices[] = {
         0,1,2,  0,2,3,//top
        4,5,6,  4,6,7,//bottom
        8,9,10,  8,10,11, //front
        12,13,14, 12,14,15, //right
       16,17,18, 16,18,19, //left
       20,21,22, 20,22,23// back
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




} // namespace tzw

