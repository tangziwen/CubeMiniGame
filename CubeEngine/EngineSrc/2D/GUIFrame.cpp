#include "GUIFrame.h"
#include "../Scene/SceneMgr.h"
#include "../Rendering/Renderer.h"
namespace tzw {

GUIFrame::GUIFrame()
    :m_contentSize(vec2(100,100))
{
    m_mesh = new Mesh();
    m_technique = new Technique("./Res/EngineCoreRes/Shaders/SpriteColor_v.glsl","./Res/EngineCoreRes/Shaders/SpriteColor_f.glsl");
    m_technique->setVar("TU_color",getUniformColor());
}

void GUIFrame::setRenderRect()
{
    m_mesh->clear();
    GLushort indices[] = {
         0,1,2,  0,2,3,
    };
    auto w = m_contentSize.x;
    auto h = m_contentSize.y;
    VertexData vertices[] = {
        // front
        vec3(0, 0,  -1.0f),  // v0
        vec3( w,  0,  -1.0f), // v1
        vec3( w,  h,  -1.0f),  // v2
        vec3( 0, h,  -1.0f), // v3
    };
    m_mesh->addVertices(vertices,sizeof(vertices)/sizeof(VertexData));
    m_mesh->addIndices(indices,sizeof(indices)/sizeof(GLushort));
    m_mesh->finish();
}

GUIFrame *GUIFrame::create(vec4 color, vec2 size)
{
    auto frame = new GUIFrame();
    frame->setUniformColor(color);
    frame->setContentSize(size);
    return frame;
}

GUIFrame *GUIFrame::create(vec4 color)
{
    auto frame = new GUIFrame();
    frame->setUniformColor(color);
    return frame;
}

void GUIFrame::draw()
{
    auto camera = SceneMgr::shared()->currentScene()->defaultGUICamera();
    auto vp = camera->getViewProjectionMatrix();
    auto m = getTransform();
    technique()->setVar("TU_mvpMatrix", vp* m);
    technique()->setVar("TU_color",getUniformColor());
    RenderCommand command(m_mesh,technique(),RenderCommand::RenderType::GUI);
    command.setZorder(m_globalPiority);
    Renderer::shared()->addRenderCommand(command);
}

vec2 GUIFrame::getContentSize() const
{
    return m_contentSize;
}

void GUIFrame::setContentSize(const vec2 &contentSize)
{
    m_contentSize = contentSize;
    setRenderRect();
}

bool GUIFrame::isInTheRect(vec2 touchPos)
{
    auto origin = getWorldPos2D();
    auto contentSize = m_contentSize;
    touchPos = touchPos - origin;
    if (touchPos.x >=0 && touchPos.x<= contentSize.x  && touchPos.y >=0 && touchPos.y<= contentSize.y)
    {
        return true;
    }else
    {
        return false;
    }
}

} // namespace tzw
