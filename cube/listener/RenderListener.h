#ifndef RENDERLISTENER_H
#define RENDERLISTENER_H
#include <functional>
#include "BaseListener.h"

class RenderListener : public BaseListener
{
public:
    RenderListener();
    virtual void handleOnRender(float dt);
    virtual Type getListenerType();
    std::function<void (RenderListener * self, float dt)> onRender;
};

#endif // RENDERLISTENER_H
