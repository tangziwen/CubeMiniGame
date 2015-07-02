#include "normalmappingdelegate.h"
#include <QDebug>
#include "utility.h"
#include "shader/ShaderPool.h"
#include "texture/texturepool.h"
#include "material/materialpool.h"
#include "entity/skybox.h"
#include "geometry/terrain.h"
#include <qquaternion.h>
#include "Entity/cubeprimitve.h"
#include "geometry/ray.h"
#include "GUI/sprite.h"
NormalMappingDelegate::NormalMappingDelegate()
{

}


void NormalMappingDelegate::onInit()
{
    scene = new Scene();
    this->move_forward=false;
    this->move_backward=false;
    this->move_left=false;
    this->move_right=false;
    this->move_up = false;
    this->move_down = false;
    scene->setRenderType (DEFERRED_SHADING);

    AmbientLight * ambient = scene->getAmbientLight ();
    ambient->setColor (QVector3D(1,1,1));
    ambient->setIntensity (0.5);

    auto sprite = new Sprite();
    sprite->setTexture (TexturePool::getInstance ()->createOrGetTexture ("./res/texture/mygame/fps/cross_hair.png"));
    sprite->setCamera (scene->guiCamera ());
    sprite->setPos (QVector3D(1024/2-sprite->texture ()->width ()/2,768/2-sprite->texture ()->height ()/2,0));
    scene->root ()->addChild (sprite);

    scene->setCamera (&camera);

    //set this scene as current scene
    scene->setAsCurrentScene();
    scene->root ()->addChild (&camera);

    //create a box
    auto the_head = new Entity("./res/model/scan_head/Infinite-Level_02.obj");
    the_head->setShaderProgram (ShaderPool::getInstance ()->get ("deferred"));
    //set it's normal map
    auto material = the_head->getMesh (0)->getMaterial ();
    material->setNormalMap (TexturePool::getInstance ()->createOrGetTexture ("./res/model/scan_head/Infinite-Level_02_Tangent_SmoothUV.jpg"));
    the_head->setCamera (&camera);
    //scene->root ()->addChild (the_head);
    //this sample we don't use shadow
    the_head->setIsEnableShadow (false);
    the_head->onUpdate = [](Node* target){
        target->setRotation (target->rotation ()+QVector3D(0,0.1,0));
    };

    {
        auto the_box = new Entity("./res/model/box/box.tzw");
        the_box->setShaderProgram (ShaderPool::getInstance ()->get ("deferred"));
        the_box->setCamera (&camera);
        scene->root ()->addChild (the_box);
        the_box->setIsEnableShadow (false);
        //this sample we don't use shadow
        the_box->setIsEnableShadow (false);
    }


    //then add  a Directional light
    auto directional_light = scene->getDirectionalLight ();
    directional_light->setIntensity (1);
    directional_light->setColor (QVector3D(1,1,1));
    directional_light->setDirection (QVector3D(0,0,-1));

}

void NormalMappingDelegate::onRender()
{
    if(move_forward)
    {
        camera.moveBy(0,0,-0.1);
    }
    if(move_backward){
        camera.moveBy(0,0,0.1);
    }
    if(move_left)
    {
        camera.moveBy(-0.1,0,0);
    }
    if(move_right)
    {
        camera.moveBy(0.1,0,0);
    }
    if(move_up)
    {
        camera.moveBy (0,0.1,0,false);
    }
    if(move_down)
    {
        camera.moveBy (0,-0.1,0,false);
    }
}
void NormalMappingDelegate::onResize(int w, int h)
{

    // Calculate aspect ratio
    float aspect = float(w) / float(h ? h : 1);

    const float zNear = 0.01, zFar = 100.0, fov = 45.0;

    // Reset projection
    camera.setPerspective(fov,aspect,zNear,zFar);
}

void NormalMappingDelegate::onKeyPress(int key_code)
{
    switch(key_code)
    {
    case Qt::Key_W:
        this->move_forward=true;
        break;
    case Qt::Key_S:
        this->move_backward=true;
        break;
    case Qt::Key_A:
        this->move_left=true;
        break;
    case Qt::Key_D:
        this->move_right=true;
        break;
    case Qt::Key_Q:
        this->move_up = true;
        break;
    case Qt::Key_E:
        this->move_down = true;
        break;
    }
}

void NormalMappingDelegate::onKeyRelease(int key_code)
{
    switch(key_code)
    {
    case Qt::Key_W:
        this->move_forward=false;
        break;
    case Qt::Key_S:
        this->move_backward=false;
        break;
    case Qt::Key_A:
        this->move_left=false;
        break;
    case Qt::Key_D:
        this->move_right=false;
        break;
    case Qt::Key_Q:
        this->move_up = false;
        break;
    case Qt::Key_E:
        this->move_down = false;
        break;
    }
}

void NormalMappingDelegate::onTouchBegin(int x, int y)
{
    mousePressPosition = QVector2D(x,y);

    mouseLastPosition = QVector2D(x,y);
}

void NormalMappingDelegate::onTouchMove(int x, int y)
{
    QVector2D diff = QVector2D(x,y) - mouseLastPosition;
    if(diff.x()>2){
        camera.yawBy(-1);
    }else if(diff.x()<-2)
    {
        camera.yawBy(1);
    }
    if(diff.y ()>2)
    {
        camera.pitchBy (-1);
    }else if(diff.y ()<-2)
    {
        camera.pitchBy (1);
    }
    mouseLastPosition=QVector2D(x,y);
}

void NormalMappingDelegate::onTouchEnd(int x, int y)
{

}
