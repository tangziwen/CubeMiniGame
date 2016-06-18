#include "GUITitledFrame.h"
#include "../Font/FontMgr.h"
#include "GUIStyleMgr.h"
#include "../Event/EventMgr.h"
#include "../Engine/EngineDef.h"
#include "GUIWindowMgr.h"
#define BORDER_LEFT 0
#define BORDER_RIGHT 1
#define BORDER_BOTTOM 2
#define BORDER_TOP 3

#include "../Scene/SceneMgr.h"
#include "../Rendering/Renderer.h"

namespace tzw {

GUITitledFrame *GUITitledFrame::create(std::string titleText, vec4 color, vec2 size)
{
    auto frame = new GUITitledFrame();
    frame->initLabel();
    frame->initBorders();
    frame->setUniformColor(color);
    frame->setContentSize(size);
    frame->setTitle(titleText);
    return frame;
}

GUITitledFrame *GUITitledFrame::create(std::string titleText, vec2 size)
{
    return GUITitledFrame::create(titleText,GUIStyleMgr::shared()->defaultPalette()->backGroundColor,size);
}

GUITitledFrame::GUITitledFrame()
{
    m_label = nullptr;
    m_isDraging = false;
    EventMgr::shared()->addEventListener(this);
    setIsSwallow(true);
    m_isDragable = true;
}

std::string GUITitledFrame::title() const
{
    return m_title;
}

void GUITitledFrame::setTitle(const std::string &title)
{
    m_title = title;
    m_label->setString(title);
    m_labelFrame->setContentSize( vec2(m_contentSize.x,35));
    m_label->setPos2D(vec2(15,7));
}

void GUITitledFrame::setContentSize(const vec2 &contentSize)
{
    GUIFrame::setContentSize(contentSize);
    m_labelFrame->setPos2D(vec2(0,m_contentSize.y));
    adjustBorders();
}

vec2 GUITitledFrame::getOuterContentSize()
{
    return getContentSize() + m_labelFrame->getContentSize()/2.0f;
}

bool GUITitledFrame::onMouseRelease(int button, vec2 pos)
{
    if (!m_isDragable) return false;
    m_isDraging = false;
    return false;
}

bool GUITitledFrame::onMousePress(int button, vec2 pos)
{
    if(m_labelFrame->isInTheRect(pos) && m_isDragable)
    {
        m_preMousePos = pos;
        m_isDraging = true;
        GUIWindowMgr::shared()->focus(this);
        return true;
    }
    else if(this->isInTheRect(pos))
    {
        GUIWindowMgr::shared()->focus(this);
        return true;
    }
    return false;
}

bool GUITitledFrame::onMouseMove(vec2 pos)
{
    if (!m_isDragable) return false;
    if(m_isDraging == true)
    {
        auto mouseForce = pos - m_preMousePos;
        setPos2D(getPos2D() + mouseForce);
        m_preMousePos = pos;
        return true;
    }
    return false;
}

void GUITitledFrame::initLabel()
{
    m_label = LabelNew::create(" ",FontMgr::shared()->getTitleFont());
    m_labelFrame = GUIFrame::create(vec4(33.0/255,33.0/255,37.0/255,1.0));
    m_labelFrame->addChild(m_label);
    addChild(m_labelFrame);
}

void GUITitledFrame::initBorders()
{
    for (int i =0; i<4; i++)
    {
        m_borders[i] = GUIFrame::create(vec4(33.0/255,33.0/255,37.0/255,1.0));
        this->addChild(m_borders[i]);
    }
}

void GUITitledFrame::adjustBorders()
{
    m_borders[BORDER_LEFT]->setContentSize(vec2(3,m_contentSize.y));
    m_borders[BORDER_RIGHT]->setContentSize(vec2(3,m_contentSize.y));
    m_borders[BORDER_RIGHT]->setPos2D(m_contentSize.x - 3,0);
    m_borders[BORDER_TOP]->setContentSize(vec2(m_contentSize.x,3));
    m_borders[BORDER_TOP]->setPos2D(0,m_contentSize.y - 3);
    m_borders[BORDER_BOTTOM]->setContentSize(vec2(m_contentSize.x,3));
}

bool GUITitledFrame::getIsDragable() const
{
    return m_isDragable;
}

void GUITitledFrame::setIsDragable(bool isDragable)
{
    m_isDragable = isDragable;
}

bool GUITitledFrame::getIsFocus() const
{
    return m_isFocus;
}

void GUITitledFrame::setIsFocus(bool isFocus)
{
    m_isFocus = isFocus;
}

} // namespace tzw
