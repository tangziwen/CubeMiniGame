#include "button.h"
#include <QVector2D>
#include "geometry/rect.h"
#include <Event/eventmgr.h>
Button::Button()
{

}

void Button::init(const char * texture, const char * texture_pressed)
{
    EventMgr::get()->addTouchableListener (this);
    textureNormal = TexturePool::getInstance ()->createOrGetTexture (texture);
    texturePressed = TexturePool::getInstance ()->createOrGetTexture (texture_pressed);
    this->setTexture(textureNormal);
}

bool Button::checkTouchPress(QVector2D pos)
{
    Rect rect(this->m_pos.toVector2D (),this->m_pos.toVector2D () +this->getSize ());
    if(rect.isInRect (pos))
    {
        return true;
    }else{
        return false;
    }
}

void Button::handleTouchPress(QVector2D pos)
{
    this->setTexture(texturePressed);
}

void Button::handleTouchRelease(QVector2D pos)
{
    this->setTexture(textureNormal);
}

