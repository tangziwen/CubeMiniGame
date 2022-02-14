#include "Button.h"
#include "../Font/FontMgr.h"
#include "../Event/EventMgr.h"
#include "GUIStyleMgr.h"

#define BORDER_LEFT 0
#define BORDER_RIGHT 1
#define BORDER_BOTTOM 2
#define BORDER_TOP 3
namespace tzw
{
Button::Button():
    m_isTouched(false)
{
    EventMgr::shared()->addNodePiorityListener(this,this);
    setIsSwallow(true);
}

Button *Button::create(std::string str)
{
    Button * btn = new Button();
    btn->init();
    btn->m_type = Type::SimpleText;
    btn->m_label = LabelNew::create(str,FontMgr::shared()->getDefaultFont());
    btn->m_label->setAnchorPoint(vec2(0.5,0.5));
    btn->m_frameBG = GUIFrame::create(GUIStyleMgr::shared()->defaultPalette()->buttonFrameColor);
    btn->addChild(btn->m_label);
    btn->addChild(btn->m_frameBG);
    btn->m_frameBG->setLocalPiority(0);
    btn->m_label->setLocalPiority(1);
    btn->setStr(str);

    return btn;
}

Button *Button::create(vec4 color, vec2 Size)
{
    Button * btn = new Button();
    btn->init();
    btn->m_type = Type::SimpleFrame;
    btn->m_label = nullptr;
    btn->m_frameBG = GUIFrame::create(color,Size);
    btn->addChild(btn->m_frameBG);
    btn->m_frameBG->setLocalPiority(0);
    //btn->setContentSize(Size);
    btn->adjustBorders();
    return btn;
}

void Button::setStr(std::string str)
{
    if(m_type != Type::SimpleText)
        return;
    m_label->setString(str);
    auto CS = m_label->getContentSize() + vec2(8,14);
    m_frameBG->setContentSize(CS);
    Drawable2D::setContentSize(CS);
    m_label->setPos2D(CS.x / 2, CS.y / 2);
    adjustBorders();
}

bool Button::onMouseRelease(int button, vec2 pos)
{
    if(m_isTouched)
        {
            if(m_type == Type::SimpleText)
                {
                    m_frameBG->setColor(GUIStyleMgr::shared()->defaultPalette()->buttonFrameColor);
                    auto cs = m_frameBG->getContentSize();
                    m_label->setPos2D(cs.x/2,cs.y/2);
                }

            m_isTouched = false;
            if (m_frameBG->isInTheRect(pos))
                {
                    m_evtBtn = button;
                    m_evtPos = pos;
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

            if(m_type == Type::SimpleText)
                {
                    m_frameBG->setColor(GUIStyleMgr::shared()->defaultPalette()->buttonFrameColorHightLight);
                    auto cs = m_frameBG->getContentSize();
                    m_label->setPos2D(cs.x/2 + 2,cs.y/2 -2);
                }
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
        }
    else
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

void Button::setContentSize(const vec2 &getContentSize)
{
    Drawable2D::setContentSize(getContentSize);
    setFrameSize(getContentSize);
}

vec2 Button::getContentSize()
{
    return m_frameBG->getContentSize();
}

LabelNew *Button::getLabel() const
{
    return m_label;
}

vec2 Button::getFrameSize()
{
    return m_frameBG->getContentSize();
}

void Button::setFrameSize(vec2 size)
{
    m_frameBG->setContentSize(size);
    adjustBorders();
    if(m_label)
        {
            m_label->setPos2D(size.x/2,size.y/2);
        }
}

void Button::manualTrigger()
{
    if(m_onBtnClicked)
        {
            m_onBtnClicked(this);
        }
}

GUIFrame *Button::getFrameBG() const
{
    return m_frameBG;
}

void Button::setEnable(bool isEnable)
{
    bool changed = isEnable != m_enable;
    m_enable = isEnable;
    if(changed)
    {
        if(isEnable)
        {
            m_label->setColor(vec4(1, 1, 1, 1));
        }
        else
        {
            m_label->setColor(vec4(0.45, 0.45, 0.45, 1));
        }
    }
}

void Button::init()
{
    initBorders();
}

void Button::adjustBorders()
{
    int border = 2;
    auto cs = m_frameBG->getContentSize();
    m_borders[BORDER_LEFT]->setContentSize(vec2(border,cs.y + 4));

    m_borders[BORDER_LEFT]->setPos2D(-border,-border);

    m_borders[BORDER_RIGHT]->setContentSize(vec2(border,cs.y + 4));
    m_borders[BORDER_RIGHT]->setPos2D(cs.x,-border);

    m_borders[BORDER_TOP]->setContentSize(vec2(cs.x + 4,border));
    m_borders[BORDER_TOP]->setPos2D(-border,cs.y);

    m_borders[BORDER_BOTTOM]->setContentSize(vec2(cs.x + 4,border));
    m_borders[BORDER_BOTTOM]->setPos2D(-border,-border);
}

void Button::initBorders()
{
    for (int i =0; i<4; i++)
        {
            m_borders[i] = GUIFrame::create(vec4(33.0/255,33.0/255,37.0/255,1.0));
            this->addChild(m_borders[i]);
            m_borders[i]->setLocalPiority(1);
        }
}


} // namespace tzw
