#ifndef NORMALMAPPINGDELEGATE_H
#define NORMALMAPPINGDELEGATE_H

#include "base/node.h"
#include "base/camera.h"
#include "scene/scene.h"
#include "shader/shader_program.h"

class NormalMappingDelegate: public Node
{
public:
    static Scene * createScene();
    NormalMappingDelegate();
    virtual void onInit() ;
    virtual void onRender();
    virtual void onResize(int w, int h);
private:
    Camera *camera;
    Scene * scene;
};

#endif // NORMALMAPPINGDELEGATE_H
