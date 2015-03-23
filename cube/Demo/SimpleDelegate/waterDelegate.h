#ifndef SIMPLE_DELEGATE_H
#define SIMPLE_DELEGATE_H
#include "base/renderdelegate.h"
#include "base/camera.h"
#include "scene/scene.h"
#include "shader/shader_program.h"
class FlightGameDelegate : public RenderDelegate
{
public:
    FlightGameDelegate();
    virtual void onInit() ;
    virtual void onRender();
    virtual void onResize(int w, int h);

    virtual void onKeyPress(int key_code);
    virtual void onKeyRelease(int key_code);
    virtual void onTouchBegin(int x,int y);
    virtual void onTouchMove(int x , int y);
    virtual void onTouchEnd(int x , int y);

private:
    void createTerrain();
    void createBlock();
    void removeBlock();
private:
    Camera camera;
    Camera cameraFixed;
    Scene * scene;
    QVector2D mousePressPosition;
    QVector2D mouseLastPosition;
    bool move_forward;
    bool move_backward;
    bool move_left;
    bool move_right;
    bool move_up;
    bool move_down;
};

#endif // MYDELEGATE_H
