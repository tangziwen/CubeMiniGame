#include "GUIFrame.h"
#include "../Scene/SceneMgr.h"
#include "GUIStyleMgr.h"
#include "../Technique/MaterialPool.h"
namespace tzw {

GUIFrame::GUIFrame()
{
    m_contentSize = vec2(100,100);
    m_mesh = new Mesh();
	m_material = MaterialPool::shared()->getMatFromTemplate("GUIColor");
	m_color = GUIStyleMgr::shared()->defaultPalette()->backGroundColor;
    setCamera(g_GetCurrScene()->defaultGUICamera());
}

void GUIFrame::setRenderRect()
{
    m_mesh->clear();
	unsigned short indices[] = {
         0,1,2,  0,2,3,
    };
    auto w = m_contentSize.x;
    auto h = m_contentSize.y;
    VertexData vertices[] = {
        // front
		VertexData(vec3(0, 0,  -1.0f), m_color),  // v0
        VertexData(vec3( w,  0,  -1.0f), m_color), // v1
        VertexData(vec3( w,  h,  -1.0f), m_color),  // v2
        VertexData(vec3( 0, h,  -1.0f), m_color)// v3
    };
    m_mesh->addVertices(vertices,sizeof(vertices)/sizeof(VertexData));
    m_mesh->addIndices(indices,sizeof(indices)/sizeof(unsigned short));
    m_mesh->finish();
}

GUIFrame *GUIFrame::create(vec4 color, vec2 size)
{
    auto frame = new GUIFrame();
    frame->setColor(color);
    frame->setContentSize(size);
    return frame;
}

GUIFrame *GUIFrame::create(vec4 color)
{
    auto frame = new GUIFrame();
    frame->setColor(color);
    return frame;
}

GUIFrame *GUIFrame::create(vec2 size)
{
    auto frame = new GUIFrame();
    frame->setColor(vec4(80.0/255,80.0/255,80.0/255,1.0));
    frame->setContentSize(size);
    return frame;
}

void GUIFrame::submitDrawCmd(RenderFlag::RenderStage stageType, RenderQueue * queues, int requirementArg)
{
    RenderCommand command(m_mesh, m_material, this, RenderFlag::RenderStage::GUI);
    setUpTransFormation(command.m_transInfo);
    command.setZorder(m_globalPiority);
    queues->addRenderCommand(command, requirementArg);
}

void GUIFrame::setContentSize(const vec2 &contentSize)
{
    Drawable2D::setContentSize(contentSize);
    //m_contentSize = contentSize;
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

void GUIFrame::setColor(vec4 color)
{
	m_color = color;
	setRenderRect();
}

void GUIFrame::setColor(vec3 color)
{
	this->setColor(vec4(color,1.0));
}

} // namespace tzw
