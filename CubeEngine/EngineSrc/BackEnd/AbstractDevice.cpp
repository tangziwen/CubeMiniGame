#include "AbstractDevice.h"
#include "GL/glew.h"
#include "../Engine/Engine.h"
#include "../Event/EventMgr.h"
#include "RenderBackEnd.h"
#include "VkRenderBackEnd.h"
namespace tzw {
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

void AbstractDevice::scrollEvent(float x, float y)
{
	EventMgr::shared()->handleScroll(vec2(x, y));
}

void AbstractDevice::init(int width,int height)
{

   
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
    Engine::shared()->update(delta);
}

void AbstractDevice::setRenderDevice(RenderDeviceType deviceType)
{
    m_deviceType = deviceType;
}

void AbstractDevice::createRenderBackEnd(GLFWwindow * window)
{
    if(m_deviceType == RenderDeviceType::OpenGl_Device)
    {
        Engine::shared()->setRenderBackEnd(RenderBackEnd::shared());
        Engine::shared()->setRenderDeviceType(RenderDeviceType::OpenGl_Device);
        RenderBackEnd::shared()->initDevice(window);

    }
    else
    {
        Engine::shared()->setRenderBackEnd(VKRenderBackEnd::shared());
        Engine::shared()->setRenderDeviceType(RenderDeviceType::Vulkan_Device);
        VKRenderBackEnd::shared()->initDevice(window);

    }
}

RenderDeviceType AbstractDevice::getRenderDeviceType()
{
    return m_deviceType;
}

} // namespace tzw

