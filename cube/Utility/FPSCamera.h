#ifndef TZW_FPSCAMERA_H
#define TZW_FPSCAMERA_H
#include "external/TUtility/TUtility.h"
#include "base/camera.h"
#include "listener/KeyListener.h"
#include "listener/TouchListener.h"
#include "listener/RenderListener.h"
#include <QVector2D>
namespace tzw {

class FPSCamera : public Camera
{
public:
    FPSCamera();
    Tvector velocity() const;
    void setVelocity(const Tvector &velocity);
    void handleKeyPress(KeyListener * self, int keycode);
    void handleKeyRelease(KeyListener * self,int keycode);
    void handleTouchPress(TouchListener * self, QVector2D pos);
    void handleTouchMove(TouchListener * self, QVector2D pos);
    void handleTouchRelease(TouchListener * self,QVector2D pos);
    void handleOnRender(RenderListener * self, float dt);
private:
    void init();
private:
    Tvector m_velocity;
    QVector2D mousePressPosition;
    QVector2D mouseLastPosition;
    bool move_forward;
    bool move_backward;
    bool move_left;
    bool move_right;
    bool move_up;
    bool move_down;
};

} // namespace tzw

#endif // TZW_FPSCAMERA_H
