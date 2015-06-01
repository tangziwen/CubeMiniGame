#include "touchable.h"



Touchable::Touchable()
    :onTouchPress(nullptr),onTouchRelease(nullptr)
{

}

bool Touchable::checkTouchPress(QVector2D pos)
{
    return false;
}

void Touchable::handleTouchPress(QVector2D pos)
{

}

void Touchable::handleTouchRelease(QVector2D pos)
{

}

