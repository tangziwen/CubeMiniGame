#include "button.h"
#include <QVector2D>
#include "geometry/rect.h"
#include <Event/EventMgr.h>
Button::Button()
    :m_isHolding(false),onTouch(nullptr)
{

}

void Button::init(const char * texture, const char * texture_pressed)
{
    EventMgr::get()->addListener (this);
    m_textureNormal = TexturePool::getInstance ()->createOrGetTexture (texture);
    m_texturePressed = TexturePool::getInstance ()->createOrGetTexture (texture_pressed);
    this->setTexture(m_textureNormal);
}

bool Button::checkTouchPress(QVector2D pos)
{
    Rect rect(this->m_pos.toVector2D (),this->m_pos.toVector2D () +this->getSize ());
    if(rect.containsPoint (pos))
    {
        return true;
    }else{
        return false;
    }
}

void Button::handleTouchPress(QVector2D pos)
{
    if(checkTouchPress(pos))
    {
        this->setTexture(m_texturePressed);
        m_isHolding = true;
    }
}

void Button::handleTouchRelease(QVector2D pos)
{
    if(m_isHolding)
    {
        this->setTexture(m_textureNormal);
        m_isHolding = false;
        if(onTouch)onTouch();
    }
}

