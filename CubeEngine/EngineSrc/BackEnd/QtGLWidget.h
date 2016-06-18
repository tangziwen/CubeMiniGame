#ifndef TZW_QTGLWIDGET_H
#define TZW_QTGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <time.h>
#include <QBasicTimer>
namespace tzw {

class QtGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit QtGLWidget(QWidget *parent = 0);
protected:
    void timerEvent(QTimerEvent *e) Q_DECL_OVERRIDE;
    virtual void keyPressEvent(QKeyEvent * event);
    virtual void keyReleaseEvent(QKeyEvent * event);
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
    virtual void mouseMoveEvent(QMouseEvent * event);
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    clock_t m_oldTicks,m_nowTicks;
    QBasicTimer timer;
    bool m_isFirstFrame;
};

} // namespace tzw

#endif // TZW_QTGLWIDGET_H
