#include "mydelegate.h"
#include <QDebug>
#include "utility.h"
#include "shader/shaderpoll.h"
#include "texture/texturepool.h"
#include "material/materialpool.h"
#include "entity/skybox.h"
#include "geometry/terrain.h"
myDelegate::myDelegate()
{

}

void myDelegate::onInit()
{
    scene = new Scene();
    this->move_forward=false;
    this->move_backward=false;
    this->move_left=false;
    this->move_right=false;
    this->move_up = false;
    this->move_down = false;


    SkyBox * sky_box = new SkyBox("./res/texture/sky_box/sp3right.jpg",
                                  "./res/texture/sky_box/sp3left.jpg",
                                  "./res/texture/sky_box/sp3top.jpg",
                                  "./res/texture/sky_box/sp3bot.jpg",
                                  "./res/texture/sky_box/sp3front.jpg",
                                  "./res/texture/sky_box/sp3back.jpg");
    sky_box->setCamera(&camera);
    scene->setSkyBox(sky_box);

    Terrain a("./res/model/terrain/terrain.jpg");
    Entity * terrain_model = new Entity();
    terrain_model->setCamera(&camera);
    a.mesh ()->getMaterial ()->getDiffuse ()->texture= TexturePool::getInstance ()->createTexture ("./res/model/terrain/sand.jpg");
    terrain_model->addMesh(a.mesh ());
    terrain_model->scale (10,10,10);
    terrain_model->setPos (QVector3D(0,-3,0));
    scene->root ()->addChild (terrain_model);

    Entity * box_cube = new Entity("res/model/box/box.obj");
    box_cube->setCamera(&camera);
    box_cube->translate(0,0,-5);
    box_cube->scale (5,5,1);
    box_cube->rotate(0,0,0);
    box_cube->setName ("box_cube");
    //scene->root ()->addChild (box_cube);

    entity =new Entity("res/model/bob/boblampclean.md5mesh");
    entity->setCamera(&camera);
    entity->scale(0.05,0.05,0.05);
    entity->translate(0,-2,-10);
    entity->rotate(-90,0,0);

    entity->setName ("entity");
    scene->root ()->addChild (entity);


    Entity * weapon = new Entity("res/model/m16/M16.dae");
    weapon->setCamera(&camera);
    weapon->rotate (-90,180,0);
    weapon->translate (0.6,-0.55,-0.8);
    camera.addChild (weapon);

    // a spotlight
    SpotLight * light = scene->createSpotLight();
    light->setPos(QVector3D(0,0,-25));
    light->setColor(QVector3D(1,0,0));
    light->setDirection(QVector3D(0,0,1));
    light->setRange(100);
    light->setAngle(utility::Ang2Radius(10));
    light->setOutterAngle(utility::Ang2Radius(12));

    //set this scene as current scene
    scene->setAsCurrentScene();
    scene->root ()->addChild (&camera);
}

void myDelegate::onRender()
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
        camera.moveBy (0,0.1,0);
    }
    if(move_down)
    {
        camera.moveBy (0,-0.1,0);
    }
    entity->setAnimateTime(entity->animateTime()+0.01);
}

void myDelegate::onResize(int w, int h)
{

    // Calculate aspect ratio
    float aspect = float(w) / float(h ? h : 1);

    const float zNear = 0.01, zFar = 100.0, fov = 45.0;

    // Reset projection
    camera.setPerspective(fov,aspect,zNear,zFar);
}

void myDelegate::onKeyPress(int key_code)
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

void myDelegate::onKeyRelease(int key_code)
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

void myDelegate::onTouchBegin(int x, int y)
{
    mousePressPosition = QVector2D(x,y);

    mouseLastPosition = QVector2D(x,y);
}

void myDelegate::onTouchMove(int x, int y)
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

void myDelegate::onTouchEnd(int x, int y)
{

}
