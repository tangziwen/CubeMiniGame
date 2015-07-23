#include "RenderListener.h"

RenderListener::RenderListener()
    :onRender(nullptr)
{

}

void RenderListener::handleOnRender(float dt)
{

}


BaseListener::Type RenderListener::getListenerType()
{
    return Type::RenderListener;
}

