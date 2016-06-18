#include "QtGLWidget.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include "../Engine/Engine.h"
#include "../Event/EventMgr.h"
#include <Qt>
#include <QDebug>
namespace tzw {

QtGLWidget::QtGLWidget(QWidget *parent)
    :QOpenGLWidget(parent),m_oldTicks(0),m_nowTicks(0),m_isFirstFrame(true)
{
    setFixedSize(1280,768);
    setAttribute(Qt::WidgetAttribute::WA_QuitOnClose,true);
    makeCurrent();
}

void QtGLWidget::timerEvent(QTimerEvent *e)
{
    repaint();
}

void QtGLWidget::keyPressEvent(QKeyEvent *event)
{
    EventMgr::shared()->handleKeyPress(event->text().toStdString());
}

void QtGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    EventMgr::shared()->handleKeyRelease(event->text().toStdString());
}

void QtGLWidget::mousePressEvent(QMouseEvent *event)
{
    auto height = Engine::shared()->windowHeight();
    auto pos = event->pos();
    EventMgr::shared()->handleMousePress(event->button(),vec2(pos.x(),height - pos.y()));
}

void QtGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    auto height = Engine::shared()->windowHeight();
    auto pos = event->pos();
    EventMgr::shared()->handleMouseRelease(event->button(),vec2(pos.x(),height - pos.y()));
}

void QtGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    auto height = Engine::shared()->windowHeight();
    auto pos = event->pos();
    EventMgr::shared()->handleMouseMove(vec2(pos.x(),height - pos.y()));
}

void QtGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // Set the background color
    glClearColor(0, 0, 0, 1);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Engine::shared()->onStart(width(),height());
    m_nowTicks = m_oldTicks = clock();
    // Use QBasicTimer because its faster than QTimer
    timer.start(1, this);
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

