#include "BoxContainer.h"
#include "GUIStyleMgr.h"
namespace tzw {

BoxContainer::BoxContainer()
{

}

BoxContainer *BoxContainer::create(vec2 size, vec2 boxSize)
{
    auto obj = new BoxContainer();
    obj->m_frameBG = GUIFrame::create(GUIStyleMgr::shared()->defaultPalette()->buttonFrameColor);
    obj->m_frameBG->setContentSize(size);
    obj->addChild(obj->m_frameBG);
    obj->m_boxSize = boxSize;
    return obj;
}

void BoxContainer::insert(Drawable2D *theElement)
{
    m_elementList.push_back(theElement);
    addChild(theElement);
}

void BoxContainer::format()
{
    auto cs = m_frameBG->getContentSize();
    int pX = 0;
    int pY = cs.y - m_boxSize.y;
    for(auto element : m_elementList)
    {
        element->setPos2D(pX,pY);
        pX += m_boxSize.x + 5;
        if(cs.x - pX < m_boxSize.x)
        {
            pX = 0;
            pY -= m_boxSize.y + 5;
        }
    }
}

vec2 BoxContainer::boxSize() const
{
    return m_boxSize;
}

void BoxContainer::setBoxSize(const vec2 &boxSize)
{
    m_boxSize = boxSize;
}

vec2 BoxContainer::getContentSize()
{
    return m_frameBG->getContentSize();
}

void BoxContainer::setContentSize(vec2 Size)
{
    m_frameBG->setContentSize(Size);
    format();
}

} // namespace tzw
