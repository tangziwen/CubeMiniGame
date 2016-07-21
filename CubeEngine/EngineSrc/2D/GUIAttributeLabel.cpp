#include "GUIAttributeLabel.h"
#include "../External/TUtility/TUtility.h"
namespace tzw {

GUIAttributeLabel::GUIAttributeLabel(std::string theDesc, float theValue)
{
    m_desc = theDesc;
    m_valueF = theValue;
    m_margin = vec2(10,10);
    auto tmp = Tmisc::StrFormat("%s:%g",theDesc.c_str(),theValue);
    m_label = LabelNew::create(tmp);
    m_label->setAnchorPoint(vec2(0.5,0.5));
    m_frame = GUIFrame::create(vec4(0.1,0.1,0.1,1.0),m_label->getContentSize() + m_margin);
    m_label->setPos2D(m_frame->getContentSize()/2);
    m_frame->addChild(m_label);
}

GUIAttributeLabel::GUIAttributeLabel()
{

}

bool GUIAttributeLabel::isNeedShowBG() const
{
    return m_isNeedShowBG;
}

void GUIAttributeLabel::setIsNeedShowBG(bool isNeedShowBG)
{
    m_isNeedShowBG = isNeedShowBG;
    m_frame->setIsDrawable(isNeedShowBG);
}

vec2 GUIAttributeLabel::getContentSize()
{
    return m_frame->getContentSize();
}

GUIAttributeLabel *GUIAttributeLabel::createWithFixedSize(std::string theDesc, float theValue, vec2 Size, AlignMode alignMode)
{
    auto obj = new GUIAttributeLabel();
    obj->m_desc = theDesc;
    obj->m_valueF = theValue;
    obj->m_margin = vec2(10,10);
    auto tmp = Tmisc::StrFormat("%s:%g",theDesc.c_str(),theValue);
    obj->m_label = LabelNew::create(tmp);
    obj->m_frame = GUIFrame::create(vec4(0.1,0.1,0.1,1.0),Size);
    switch(alignMode)
    {
    case AlignMode::Centre:
        obj->m_label->setAnchorPoint(vec2(0.5,0.5));
        obj->m_label->setPos2D(obj->m_frame->getContentSize()/2);
        break;
    case AlignMode::Left:
        obj->m_label->setAnchorPoint(vec2(0,0.5));
        obj->m_label->setPos2D(obj->m_margin.x,obj->m_frame->getContentSize().y / 2);
        break;
    }


    obj->m_frame->addChild(obj->m_label);
    obj->m_isFixedSize = true;
    obj->addChild(obj->m_frame);
    return obj;
}

GUIAttributeLabel *GUIAttributeLabel::create(std::string theDesc, float theValue)
{
    auto obj = new GUIAttributeLabel();
    obj->m_desc = theDesc;
    obj->m_valueF = theValue;
    obj->m_margin = vec2(10,10);
    auto tmp = Tmisc::StrFormat("%s:%g",theDesc.c_str(),theValue);
    obj->m_label = LabelNew::create(tmp);
    obj->m_label->setAnchorPoint(vec2(0.5,0.5));
    obj->m_frame = GUIFrame::create(vec4(0.1,0.1,0.1,1.0),obj->m_label->getContentSize() + obj->m_margin);
    obj->m_label->setPos2D(obj->m_frame->getContentSize()/2);
    obj->m_frame->addChild(obj->m_label);
    obj->m_isFixedSize = false;
    obj->addChild(obj->m_frame);
    return obj;
}

std::string GUIAttributeLabel::getDesc() const
{
    return m_desc;
}

void GUIAttributeLabel::setDesc(const std::string &desc)
{
    m_desc = desc;
    constructUI();
}

void GUIAttributeLabel::constructUI()
{
    m_label->setString(Tmisc::StrFormat("%s:%g",m_desc.c_str(),m_valueF));
    if(!m_isFixedSize)
    {
        m_frame->setContentSize(m_label->getContentSize() + m_margin * 2);
    }
}

vec2 GUIAttributeLabel::getMargin() const
{
    return m_margin;
}

void GUIAttributeLabel::setMargin(const vec2 &margin)
{
    m_margin = margin;
    constructUI();
}

float GUIAttributeLabel::getValueF() const
{
    return m_valueF;
}

void GUIAttributeLabel::setValueF(float valueF)
{
    m_valueF = valueF;
    constructUI();
}

} // namespace tzw
