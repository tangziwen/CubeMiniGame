#include "TouchListener.h"



TouchListener::TouchListener()
    :onTouchPress(nullptr),onTouchRelease(nullptr)
    ,onTouchMove(nullptr)
{

}


void TouchListener::handleTouchPress(QVector2D pos)
{

}

void TouchListener::handleTouchMove(QVector2D pos)
{

}

void TouchListener::handleTouchRelease(QVector2D pos)
{

}

BaseListener::Type TouchListener::getListenerType()
{
    return Type::TouchListener;
}

