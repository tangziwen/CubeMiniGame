#include "AbstractDevice.h"
#include "GL/glew.h"
#include "../Engine/Engine.h"
#include "../Event/EventMgr.h"
#include "RenderBackEnd.h"
namespace tzw {
TZW_SINGLETON_IMPL(AbstractDevice)

void AbstractDevice::keyPressEvent(int theCode)
{
    EventMgr::shared()->handleKeyPress(theCode);
}

void AbstractDevice::keyReleaseEvent(int theCode)
{
    EventMgr::shared()->handleKeyRelease(theCode);
}

void AbstractDevice::charInputEvent(unsigned int theCode)
{
    EventMgr::shared()->handleCharInput(theCode);
}

void AbstractDevice::mousePressEvent(int buttonCode,vec2 pos)
{
    auto height = Engine::shared()->windowHeight();
    EventMgr::shared()->handleMousePress(buttonCode,vec2(pos.x,height - pos.y));
}

void AbstractDevice::mouseReleaseEvent(int buttonCode,vec2 pos)
{
    auto height = Engine::shared()->windowHeight();
    EventMgr::shared()->handleMouseRelease(buttonCode,vec2(pos.x,height - pos.y));
}

void AbstractDevice::mouseMoveEvent(vec2 pos)
{
    auto height = Engine::shared()->windowHeight();
    EventMgr::shared()->handleMouseMove(vec2(pos.x,height - pos.y));
}

void AbstractDevice::init(int width,int height)
{
    RenderBackEnd::shared()->initDevice();
    Engine::shared()->onStart();
    m_nowTicks = m_oldTicks = clock();
}

void AbstractDevice::resizeGL(int w, int h)
{
}

void AbstractDevice::update()
{
    //calculate delta
    m_nowTicks = clock();
    float delta = (m_nowTicks - m_oldTicks)*1.0 /CLOCKS_PER_SEC;
    m_oldTicks = m_nowTicks;
    if (delta > 0.1)
    {
        m_isFirstFrame = false;
    }
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Engine::shared()->update(delta);
}

} // namespace tzw

