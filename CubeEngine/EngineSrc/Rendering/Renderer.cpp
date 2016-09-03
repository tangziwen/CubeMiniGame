#include "Renderer.h"
#include "../BackEnd/RenderBackEnd.h"
#include <algorithm>
#include "../Engine/Engine.h"
#include "../Scene/SceneMgr.h"
#include "../Mesh/Mesh.h"
namespace tzw {

Renderer * Renderer::m_instance = nullptr;
Renderer::Renderer()
{
    m_enable3DRender = true;
    m_enableGUIRender = true;
    m_gbuffer = new RenderTarget();
    m_gbuffer->init(Engine::shared()->windowWidth(),Engine::shared()->windowHeight());
    m_dirLightProgram = ShaderMgr::shared()->createOrGet("./Res/EngineCoreRes/Shaders/LightPass_v.glsl",
                                                         "./Res/EngineCoreRes/Shaders/LightPassDir_f.glsl");

    initQuad();
}

Renderer *Renderer::shared()
{
    if(!Renderer::m_instance)
    {
        Renderer::m_instance = new Renderer();
    }
    return Renderer::m_instance;
}

void Renderer::addRenderCommand(RenderCommand command)
{
    Engine::shared()->increaseDrawCallCount();
    switch(command.type())
    {
    case RenderCommand::RenderType::GUI:
        m_GUICommandList.push_back(command);
        break;
    case RenderCommand::RenderType::Common:
        m_CommonCommand.push_back(command);
        break;
    default:
        break;
    }
}

bool GUICommandSort(const RenderCommand &a,const RenderCommand &b)
{
    return a.Zorder() < b.Zorder();
}

void Renderer::renderAll()
{
//    glDisable(GL_CULL_FACE);
    RenderBackEnd::shared()->setDepthMaskWriteEnable(true);
    RenderBackEnd::shared()->setDepthTestEnable(true);
    m_gbuffer->bindForWriting();
    RenderBackEnd::shared()->disableFunction(RenderFlag::RenderFunction::AlphaBlend);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(m_enable3DRender)
    {
        renderAllCommon();
    }

    auto WINDOW_WIDTH = 1024;
    auto WINDOW_HEIGHT = 768;
    GLsizei HalfWidth = (GLsizei)(WINDOW_WIDTH / 2.0f);
    GLsizei HalfHeight = (GLsizei)(WINDOW_HEIGHT / 2.0f);
    RenderBackEnd::shared()->bindFrameBuffer(0);
    //    m_gbuffer->setReadBuffer(0);
    //    RenderBackEnd::shared()->blitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
    //                    0, HalfHeight, HalfWidth, WINDOW_HEIGHT);

    //    m_gbuffer->setReadBuffer(1);
    //    RenderBackEnd::shared()->blitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
    //                    HalfWidth, HalfHeight, WINDOW_WIDTH, WINDOW_HEIGHT);
    //     glClear(GL_COLOR_BUFFER_BIT);
    //        m_gbuffer->setReadBuffer(2);
    //        RenderBackEnd::shared()->blitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
    //                                                 0, 0, HalfWidth, HalfHeight);

    //    m_gbuffer->setReadBuffer(3);
    //    RenderBackEnd::shared()->blitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
    //                    HalfWidth, 0, WINDOW_WIDTH, HalfHeight);


    LightingPass();

    if(m_enableGUIRender)
    {
        renderAllGUI();
    }

    clearCommands();
}

void Renderer::renderAllCommon()
{
    for(auto i = m_CommonCommand.begin();i!=m_CommonCommand.end();i++)
    {
        RenderCommand &command = (*i);
        renderCommon(command);
    }
}

void Renderer::renderAllGUI()
{
    RenderBackEnd::shared()->enableFunction(RenderFlag::RenderFunction::AlphaBlend);
    RenderBackEnd::shared()->setBlendFactor(RenderFlag::BlendingFactor::SrcAlpha,
                                            RenderFlag::BlendingFactor::OneMinusSrcAlpha);
    std::stable_sort(m_GUICommandList.begin(),m_GUICommandList.end(),GUICommandSort);
    RenderBackEnd::shared()->disableFunction(RenderFlag::RenderFunction::DepthTest);
    for(auto i =m_GUICommandList.begin();i!=m_GUICommandList.end();i++)
    {
        RenderCommand &command = (*i);
        renderGUI(command);
    }
    RenderBackEnd::shared()->enableFunction(RenderFlag::RenderFunction::DepthTest);
}

void Renderer::renderGUI(RenderCommand &command)
{
    render(command);
}

void Renderer::renderCommon(RenderCommand &command)
{
    RenderBackEnd::shared()->setDepthTestMethod(command.depthTestPolicy().depthTest());
    render(command);
}

void Renderer::clearCommands()
{
    m_CommonCommand.clear();
    m_GUICommandList.clear();
}

void Renderer::render(RenderCommand &command)
{
    command.m_technique->use();
    renderPrimitive(command.m_mesh, command.m_technique->program(), command.m_primitiveType);
}

void Renderer::renderPrimitive(Mesh * mesh, ShaderProgram * program,RenderCommand::PrimitiveType primitiveType)
{
    program->use();
    mesh->getArrayBuf()->use();
    mesh->getIndexBuf()->use();
    // Offset for position
    quintptr offset = 0;
    Engine::shared()->increaseVerticesIndicesCount(mesh->getVerticesSize(), mesh->getIndicesSize());
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));
    offset += sizeof(vec3);

    int normalLocation = program->attributeLocation("a_normal");
    program->enableAttributeArray(normalLocation);
    program->setAttributeBuffer(normalLocation,GL_FLOAT,offset,3,sizeof(VertexData));
    offset += sizeof(vec3);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));
    offset += sizeof(vec2);

    int colorLocation = program->attributeLocation("a_color");
    program->enableAttributeArray(colorLocation);
    program->setAttributeBuffer(colorLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    switch(primitiveType)
    {
    case RenderCommand::PrimitiveType::TRIANGLES:
        RenderBackEnd::shared()->drawElement(RenderFlag::IndicesType::Triangles,mesh->getIndicesSize(),0);
        break;
    case RenderCommand::PrimitiveType::TRIANGLE_STRIP:
        RenderBackEnd::shared()->drawElement(RenderFlag::IndicesType::TriangleStrip,mesh->getIndicesSize(),0);
        break;
    }
}
///
/// \brief 获取是否禁用3D渲染
/// \return
///
bool Renderer::enable3DRender() const
{
    return m_enable3DRender;
}
///
/// \brief 决定是否禁用3D渲染
/// \param enable3DRender
///
void Renderer::setEnable3DRender(bool enable3DRender)
{
    m_enable3DRender = enable3DRender;
}

///
/// \brief 获取是否渲染GUI
/// \return
///
bool Renderer::enableGUIRender() const
{
    return m_enableGUIRender;
}

///
/// \brief 决定是否禁用GUI渲染
/// \param enableGUIRender
///
void Renderer::setEnableGUIRender(bool enableGUIRender)
{
    m_enableGUIRender = enableGUIRender;
}

void Renderer::initQuad()
{
    m_quad = new Mesh();
    m_quad->addVertex(VertexData(vec3(-1.0,-1.0,0.0),vec2(0.0,0.0)));
    m_quad->addVertex(VertexData(vec3(1.0,-1.0,0.0),vec2(1.0,0.0)));
    m_quad->addVertex(VertexData(vec3(1.0,1.0,0.0),vec2(1.0,1.0)));
    m_quad->addVertex(VertexData(vec3(-1.0,1.0,0.0),vec2(0.0,1.0)));
    unsigned short indics[] = {0,1,2,0,2,3};
    m_quad->addIndices(indics,6);
    m_quad->finish(true);
}

void Renderer::LightingPass()
{
    RenderBackEnd::shared()->bindFrameBuffer(0);
    RenderBackEnd::shared()->enableFunction(RenderFlag::RenderFunction::AlphaBlend);
    RenderBackEnd::shared()->setBlendEquation(RenderFlag::BlendingEquation::Add);
    RenderBackEnd::shared()->setBlendFactor(RenderFlag::BlendingFactor::One,
                                            RenderFlag::BlendingFactor::One);
    RenderBackEnd::shared()->setDepthMaskWriteEnable(false);
    RenderBackEnd::shared()->setDepthTestEnable(false);
    m_gbuffer->bindForReading();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BITS);

    directionalLightPass();
}

void Renderer::directionalLightPass()
{
    auto currScene = SceneMgr::shared()->currentScene();
    m_dirLightProgram->use();
    m_dirLightProgram->setUniformInteger("TU_colorBuffer",0);
    m_dirLightProgram->setUniformInteger("TU_posBuffer",1);
    m_dirLightProgram->setUniformInteger("TU_normalBuffer",2);
    m_dirLightProgram->setUniformInteger("TU_GBUFFER4",3);
    auto cam = currScene->defaultCamera();

    auto dirLight = currScene->getDirectionLight();
    auto dirInViewSpace = (cam->getViewMatrix() * vec4(dirLight->dir(),0.0)).toVec3();
    m_dirLightProgram->setUniform3Float("gDirectionalLight.direction",dirInViewSpace);
    m_dirLightProgram->setUniform3Float("gDirectionalLight.color",dirLight->color());

    auto ambient = currScene->getAmbient();
    m_dirLightProgram->setUniform3Float("gAmbientLight.color",ambient->color());
    m_dirLightProgram->setUniformFloat("gAmbientLight.intensity",ambient->intensity());

    m_dirLightProgram->use();
    renderPrimitive(m_quad, m_dirLightProgram, RenderCommand::PrimitiveType::TRIANGLES);
}




} // namespace tzw

