#ifndef GUIDELEGATE_H
#define GUIDELEGATE_H

#include "base/renderdelegate.h"
#include "base/camera.h"
#include "scene/scene.h"
#include "shader/shader_program.h"
class GUIDelegate :public RenderDelegate
{
public:
    GUIDelegate();
    virtual void onInit() ;
    virtual void onRender();
    virtual void onResize(int w, int h);

    virtual void onKeyPress(int key_code);
    virtual void onKeyRelease(int key_code);
    virtual void onTouchBegin(int x,int y);
    virtual void onTouchMove(int x , int y);
    virtual void onTouchEnd(int x , int y);

private:
private:
    Scene * scene;
};

#endif // GUIDELEGATE_H
