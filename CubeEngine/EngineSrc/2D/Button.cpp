#include "Button.h"
#include "../Font/FontMgr.h"
#include "../Event/EventMgr.h"
#include "GUIStyleMgr.h"
namespace tzw {
Button::Button():
    m_isTouched(false)
{
    EventMgr::shared()->addEventListener(this);
    setEventPiority(1000);
    setIsSwallow(true);
}

Button *Button::create(std::string str)
{
    Button * btn = new Button();
    btn->m_label = LabelNew::create(str,FontMgr::shared()->getDefaultFont());
    btn->m_frameBG = GUIFrame::create(GUIStyleMgr::shared()->defaultPalette()->buttonFrameColor);
    btn->addChild(btn->m_label);
    btn->addChild(btn->m_frameBG);
    btn->m_frameBG->setPiority(0);
    btn->m_label->setPiority(1);
    btn->setStr(str);
    return btn;
}

void Button::setStr(std::string str)
{
    m_label->setString(str);
    m_frameBG->setContentSize(m_label->contentSize() + vec2(18,12));
    m_label->setPos2D(9,8);
}

bool Button::onMouseRelease(int button, vec2 pos)
{
    if(m_isTouched)
    {
        m_frameBG->setUniformColor(GUIStyleMgr::shared()->defaultPalette()->buttonFrameColor);
        m_isTouched = false;
        if (m_frameBG->isInTheRect(pos))
        {
            if(m_onBtnClicked)
            {
                m_onBtnClicked(this);
            }
            return true;
        }
    }
    return false;
}

bool Button::onMousePress(int button, vec2 pos)
{
    if (m_frameBG->isInTheRect(pos))
    {
        m_frameBG->setUniformColor(GUIStyleMgr::shared()->defaultPalette()->buttonFrameColorHightLight);
        m_isTouched = true;
        return true;
    }
    return false;
}

bool Button::onMouseMove(vec2 pos)
{
    if(m_isTouched)
    {
        return true;
    }else
    {
        return false;
    }
}

//bool Button::isInTheRect(vec2 touchPos)
//{
//    auto origin = getWorldPos2D();
//    auto contentSize = m_frameBG->getContentSize();
//    touchPos = touchPos - origin;
//    if (touchPos.x >=0 && touchPos.x<= contentSize.x  && touchPos.y >=0 && touchPos.y<= contentSize.y)
//    {
//        return true;
//    }else
//    {
//        return false;
//    }
//}

std::function<void (Button *)> Button::onBtnClicked() const
{
    return m_onBtnClicked;
}

void Button::setOnBtnClicked(const std::function<void (Button *)> &onBtnClicked)
{
    m_onBtnClicked = onBtnClicked;
}

vec2 Button::getContentSize()
{
   return m_frameBG->getContentSize();
}

LabelNew *Button::getLabel() const
{
    return m_label;
}

void Button::manualTrigger()
{
    if(m_onBtnClicked)
    {
        m_onBtnClicked(this);
    }
}


} // namespace tzw
