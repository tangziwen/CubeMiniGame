#include "QtGLWidget.h"
#include "GL/glew.h"
#include "../Engine/Engine.h"
#include "../Event/EventMgr.h"

#include <Qt>
namespace tzw {
TZW_SINGLETON_IMPL(QtGLWidget)

void QtGLWidget::keyPressEvent(std::string theCode)
{
    EventMgr::shared()->handleKeyPress(theCode);
}

void QtGLWidget::keyReleaseEvent(std::string theCode)
{
    EventMgr::shared()->handleKeyRelease(theCode);
}

void QtGLWidget::mousePressEvent(int buttonCode,vec2 pos)
{
    auto height = Engine::shared()->windowHeight();
    EventMgr::shared()->handleMousePress(buttonCode,vec2(pos.x,height - pos.y));
}

void QtGLWidget::mouseReleaseEvent(int buttonCode,vec2 pos)
{
    auto height = Engine::shared()->windowHeight();
    EventMgr::shared()->handleMouseRelease(buttonCode,vec2(pos.x,height - pos.y));
}

void QtGLWidget::mouseMoveEvent(vec2 pos)
{
    auto height = Engine::shared()->windowHeight();
    EventMgr::shared()->handleMouseMove(vec2(pos.x,height - pos.y));
}

void QtGLWidget::initializeGL(int width,int height)
{
    // Set the background color
    glClearColor(0, 0, 0, 1);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Engine::shared()->onStart(width,height);
    m_nowTicks = m_oldTicks = clock();
}

void QtGLWidget::resizeGL(int w, int h)
{
}

void QtGLWidget::paintGL()
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

