#include "SkyBox.h"
#include "../Scene/SceneMgr.h"
namespace tzw {
Mesh * SkyBox::m_skyBoxMesh = nullptr;
SkyBox::SkyBox()
{

}

SkyBox *SkyBox::create(std::string PosXFilename, std::string NegXFilename, std::string PosYFilename, std::string NegYFilename, std::string PosZFilename, std::string NegZFilename)
{
    auto cube = new SkyBox();
    cube->init(PosXFilename,NegXFilename,PosYFilename,NegYFilename,PosZFilename,NegZFilename);
    cube->getDepthPolicy().setDepthTest(RenderFlag::DepthTestMethod::LessOrEqual);
    return cube;
}

void SkyBox::init(std::string PosXFilename, std::string NegXFilename, std::string PosYFilename, std::string NegYFilename, std::string PosZFilename, std::string NegZFilename)
{
    initSkyBoxMesh();
    setCamera(g_GetCurrScene()->defaultCamera());
    m_skyBoxTexture = TextureMgr::shared()->getByPath(PosXFilename,NegXFilename,PosYFilename,NegYFilename,PosZFilename,NegZFilename);
    m_skyBoxTexture->setFilter(Texture::FilterType::Linear);
    m_skyBoxTexture->setWarp(RenderFlag::WarpAddress::Clamp);
    m_skyBoxTechnique = Material::createFromTemplate("SkyBox");
    m_skyBoxTechnique->setTex("TU_tex1",m_skyBoxTexture);
    setIsAccpectOcTtree(false);
}

Material *SkyBox::getMaterial() const
{
    return m_skyBoxTechnique;
}

Mesh *SkyBox::skyBoxMesh()
{
    return m_skyBoxMesh;
}

void SkyBox::prepare()
{
    setPos(camera()->getPos());
    reCache();
}

void SkyBox::initSkyBoxMesh()
{
    if(m_skyBoxMesh) return;
    m_skyBoxMesh = new tzw::Mesh();
    VertexData vertices[] = {
        VertexData(vec3( -1.0f, 1.0f,  1.0f), vec2(0.0f, 1.0f)), //0
        VertexData(vec3( -1.0f,  -1.0f,  1.0f), vec2(1.0f, 0.0f)), //1
        VertexData(vec3( 1.0f,  -1.0f,  1.0f), vec2(1.0f, 1.0f)), //2
        VertexData(vec3(1.0f, 1.0f,  1.0f), vec2(0.0f, 0.0f)), //3
        VertexData(vec3( -1.0f, 1.0f,  -1.0f), vec2(0.0f, 1.0f)), //4
        VertexData(vec3( -1.0f,  -1.0f,  -1.0f), vec2(1.0f, 0.0f)),  //5
        VertexData(vec3( 1.0f,  -1.0f,  -1.0f), vec2(1.0f, 1.0f)), //6
        VertexData(vec3(1.0f, 1.0f,  -1.0f), vec2(0.0f, 0.0f)),  //7
    };
    unsigned short indices[] = {
         3,2,1,  3,1,0,
        4,5,6,   4,6,7,
        7,6,2,   7,2,3,
        0,1,5,   0,5,4,
        0,4,7,   0,7,3,
        5,1,2,   5,2,6,
    };
    m_skyBoxMesh->addVertices(vertices,sizeof(vertices)/sizeof(VertexData));
    m_skyBoxMesh->addIndices(indices,sizeof(indices)/sizeof(unsigned short));
    m_skyBoxMesh->finish();
}
} // namespace tzw
