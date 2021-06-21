#ifndef TZW_QTGLWIDGET_H
#define TZW_QTGLWIDGET_H


#include <time.h>
#include <string>
#include "../Engine/EngineDef.h"
#include "../Math/vec2.h"
#include "Base/TimerMgr.h"
class GLFWwindow;
namespace tzw {

class AbstractDevice : public Singleton<AbstractDevice>
{
public:
    void keyPressEvent(int theCode);
    void keyReleaseEvent(int theCode);
    void charInputEvent(unsigned int theCode);
    void mousePressEvent(int buttonCode, vec2 pos);
    void mouseReleaseEvent(int buttonCode, vec2 pos);
    void mouseMoveEvent(vec2 pos);
	void scrollEvent(float x, float y);
    void init(int width, int height);
    void resizeGL(int w, int h);
    void update();
    void setRenderDevice(RenderDeviceType deviceType);
    void createRenderBackEnd(GLFWwindow * window);
    RenderDeviceType getRenderDeviceType();
    Counter m_oldTicks,m_nowTicks;
    bool m_isFirstFrame;
    RenderDeviceType m_deviceType;
};

} // namespace tzw

#endif // TZW_QTGLWIDGET_H
