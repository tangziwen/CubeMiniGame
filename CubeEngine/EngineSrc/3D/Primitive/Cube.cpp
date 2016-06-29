#include "Cube.h"
#include "../../Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
namespace tzw {
Mesh * Cube::m_mesh[3] = {nullptr};
Mesh * Cube::m_skyBoxMesh = nullptr;
Cube::Cube()
{
    m_cubeType = CubeType::Cube;
}

void Cube::init(Texture *top, Texture *bottom, Texture *side)
{
    initTopMesh();
    initBottomMesh();
    initSideMesh();
    for(int i =0;i<3;i++)
    {
        m_localAABB.merge(m_mesh[i]->getAabb());
    }
    m_texture[0] = top;
    m_texture[1] = bottom;
    m_texture[2] = side;
    for(int i =0;i<3;i++)
    {
        m_texture[i]->setFilter(Texture::FilterType::Nearest);
        m_technique[i] = new Technique("./Res/EngineCoreRes/Shaders/Simple_v.glsl","./Res/EngineCoreRes/Shaders/Simple_f.glsl");
        m_technique[i]->setTex("texture_1",m_texture[i]);
    }
    setCamera(SceneMgr::shared()->currentScene()->defaultCamera());
}

void Cube::init(std::string top,std::string bottom,std::string side)
{
    initTopMesh();
    initBottomMesh();
    initSideMesh();
    for(int i =0;i<3;i++)
    {
        m_localAABB.merge(m_mesh[i]->getAabb());
    }
    std::vector<std::string> filePath;
    filePath.push_back(top);
    filePath.push_back(bottom);
    filePath.push_back(side);
    for(int i =0;i<3;i++)
    {
        m_texture[i] = TextureMgr::shared()->getOrCreateTexture(filePath[i]);
        m_texture[i]->setFilter(Texture::FilterType::Nearest);
        m_technique[i] = new Technique("./Res/EngineCoreRes/Shaders/Simple_v.glsl","./Res/EngineCoreRes/Shaders/Simple_f.glsl");
        m_technique[i]->setTex("TU_tex1",m_texture[i]);
    }
    initTopMesh();
    initBottomMesh();
    initSideMesh();
    setCamera(SceneMgr::shared()->currentScene()->defaultCamera());
}

void Cube::init(std::string  PosXFilename,
                std::string  NegXFilename,
                std::string  PosYFilename,
                std::string  NegYFilename,
                std::string  PosZFilename,
                std::string  NegZFilename)
{
    initSkyBoxMesh();
    setCamera(SceneMgr::shared()->currentScene()->defaultCamera());
    m_skyBoxTexture = TextureMgr::shared()->getOrCreateTexture(PosXFilename,NegXFilename,PosYFilename,NegYFilename,PosZFilename,NegZFilename);
    m_skyBoxTexture->setFilter(Texture::FilterType::Nearest);
    m_skyBoxTexture->setWarp(RenderFlag::WarpAddress::Repeat);
    m_skyBoxTechnique = new Technique("./Res/EngineCoreRes/Shaders/SkyBox_v.glsl","./Res/EngineCoreRes/Shaders/SkyBox_f.glsl");
    m_skyBoxTechnique->setTex("TU_tex1",m_skyBoxTexture);
    m_cubeType = CubeType::Cube;
    setIsAccpectOCTtree(false);
}

Cube *Cube::create(std::string top,std::string bottom,std::string side)
{
    auto cube = new Cube();
    cube->init(top,bottom,side);
    cube->m_cubeType = CubeType::Cube;
    return cube;
}

Cube *Cube::create(std::string PosXFilename, std::string NegXFilename, std::string PosYFilename, std::string NegYFilename, std::string PosZFilename, std::string NegZFilename)
{
    auto cube = new Cube();
    cube->init(PosXFilename,NegXFilename,PosYFilename,NegYFilename,PosZFilename,NegZFilename);
    cube->m_cubeType = CubeType::Skybox;
    cube->getDepthPolicy().setDepthTest(RenderFlag::DepthTestMethod::LessOrEqual);
    return cube;
}

void Cube::draw()
{
    if(m_cubeType == CubeType::Cube)
    {
        for(int i =0;i<3;i++)
        {
            m_technique[i]->applyFromDrawable(this);
            RenderCommand command(m_mesh[i],m_technique[i],RenderCommand::RenderType::Common);
            Renderer::shared()->addRenderCommand(command);
        }
    }else
    {
        setPos(camera()->getPos());
        reCache();
        m_skyBoxTechnique->applyFromDrawable(this);
        RenderCommand command(m_skyBoxMesh,m_skyBoxTechnique,RenderCommand::RenderType::Common);
        command.setDepthTestPolicy(getDepthPolicy());
        Renderer::shared()->addRenderCommand(command);
    }

}

Cube::CubeType Cube::getCubeType() const
{
    return m_cubeType;
}


void Cube::initSkyBoxMesh()
{
    if(m_skyBoxMesh) return;
    m_skyBoxMesh = new tzw::Mesh();
    VertexData vertices[] = {
        {vec3( -1.0f, 1.0f,  1.0f), vec2(0.0f, 0.0f)}, // v1
        {vec3( -1.0f,  -1.0f,  1.0f), vec2(1.0f, 0.0f)},  // v2
        {vec3( 1.0f,  -1.0f,  1.0f), vec2(1.0f, 1.0f)}, // v3
        {vec3(1.0f, 1.0f,  1.0f), vec2(0.0f, 1.0f)},  // v0

        {vec3( -1.0f, 1.0f,  -1.0f), vec2(0.0f, 0.0f)}, // v1
        {vec3( -1.0f,  -1.0f,  -1.0f), vec2(1.0f, 0.0f)},  // v2
        {vec3( 1.0f,  -1.0f,  -1.0f), vec2(1.0f, 1.0f)}, // v3
        {vec3(1.0f, 1.0f,  -1.0f), vec2(0.0f, 1.0f)},  // v0
    };
    GLushort indices[] = {
         3,2,1,  3,1,0,
        4,5,6,   4,6,7,
        7,6,2,   7,2,3,
        0,1,5,   0,5,4,
        0,4,7,   0,7,3,
        5,1,2,   5,2,6,
    };
    m_skyBoxMesh->addVertices(vertices,sizeof(vertices)/sizeof(VertexData));
    m_skyBoxMesh->addIndices(indices,sizeof(indices)/sizeof(GLushort));
    m_skyBoxMesh->finish();
}

void Cube::initTopMesh()
{
    if(m_mesh[0]) return;
    m_mesh[0] = new tzw::Mesh();
    VertexData vertices[] = {
        // Vertex data for face 0
        {vec3( -1.0f, 1.0f,  1.0f), vec2(0.0f, 0.0f)}, // v1
        {vec3( 1.0f,  1.0f,  1.0f), vec2(1.0f, 0.0f)},  // v2
        {vec3( 1.0f,  1.0f,  -1.0f), vec2(1.0f, 1.0f)}, // v3
        {vec3(-1.0f, 1.0f,  -1.0f), vec2(0.0f, 1.0f)},  // v0
    };

    GLushort indices[] = {
         0,1,2,  0,2,3
    };
    m_mesh[0]->addVertices(vertices,sizeof(vertices)/sizeof(VertexData));
    m_mesh[0]->addIndices(indices,sizeof(indices)/sizeof(GLushort));
    m_mesh[0]->finish();
}

void Cube::initBottomMesh()
{
    if(m_mesh[1]) return;
    m_mesh[1] = new tzw::Mesh();
    VertexData vertices[] = {
        // Vertex data for face 0
        {vec3(-1.0f, -1.0f,  -1.0f), vec2(0.0f, 1.0f)},  // v0
        {vec3( 1.0f,  -1.0f,  -1.0f), vec2(1.0f, 1.0f)}, // v3
        {vec3( 1.0f,  -1.0f,  1.0f), vec2(1.0f, 0.0f)},  // v2
        {vec3( -1.0f, -1.0f,  1.0f), vec2(0.0f, 0.0f)}, // v1
    };

    GLushort indices[] = {
         0,1,2,  0,2,3
    };
    m_mesh[1]->addVertices(vertices,sizeof(vertices)/sizeof(VertexData));
    m_mesh[1]->addIndices(indices,sizeof(indices)/sizeof(GLushort));
    m_mesh[1]->finish();
}

void Cube::initSideMesh()
{
    if(m_mesh[2]) return;
    m_mesh[2] = new tzw::Mesh();
    VertexData vertices[] = {
        // front
        {vec3(-1.0f, -1.0f,  1.0f), vec2(0.0f, 0.0f)},  // v0
        {vec3( 1.0f,  -1.0f,  1.0f), vec2(1.0f, 0.0f)}, // v1
        {vec3( 1.0f,  1.0f,  1.0f), vec2(1.0f, 1.0f)},  // v2
        {vec3( -1.0f, 1.0f,  1.0f), vec2(0.0f, 1.0f)}, // v3

        // right
        {vec3(1.0f, -1.0f,  1.0f), vec2(0.0f, 0.0f)},  // v4
        {vec3( 1.0f,  -1.0f,  -1.0f), vec2(1.0f, 0.0f)}, // v5
        {vec3( 1.0f,  1.0f,  -1.0f), vec2(1.0f, 1.0f)},  // v6
        {vec3( 1.0f, 1.0f,  1.0f), vec2(0.0f, 1.0f)}, // v7

        // left
        {vec3( -1.0f, 1.0f,  1.0f), vec2(1.0f, 1.0f)}, // v8
        {vec3( -1.0f,  1.0f,  -1.0f), vec2(0.0f, 1.0f)},  // v9
        {vec3( -1.0f,  -1.0f,  -1.0f), vec2(0.0f, 0.0f)}, // v10
        {vec3( -1.0f, -1.0f,  1.0f), vec2(1.0f, 0.0f)},  // v11

        //back
        {vec3( -1.0f, 1.0f,  -1.0f), vec2(1.0f, 1.0f)}, // v12
        {vec3( 1.0f,  1.0f,  -1.0f), vec2(0.0f, 1.0f)},  // v13
        {vec3( 1.0f,  -1.0f,  -1.0f), vec2(0.0f, 0.0f)}, // v14
        {vec3(-1.0f, -1.0f,  -1.0f), vec2(1.0f, 0.0f)},  // v15
    };

    GLushort indices[] = {
         0,1,2,  0,2,3, //front
         4,5,6, 4,6,7, //right
        8,9,10, 8,10,11, //left
        12,13,14, 12,14,15// back
    };
    m_mesh[2]->addVertices(vertices,sizeof(vertices)/sizeof(VertexData));
    m_mesh[2]->addIndices(indices,sizeof(indices)/sizeof(GLushort));
    m_mesh[2]->finish();
}

} // namespace tzw

