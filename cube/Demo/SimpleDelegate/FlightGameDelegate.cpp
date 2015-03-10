#include "TerrainDelegate.h"
#include <QDebug>
#include "utility.h"
#include "shader/shaderpoll.h"
#include "texture/texturepool.h"
#include "material/materialpool.h"
#include "entity/skybox.h"
#include "geometry/terrain.h"
#include <qquaternion.h>
#include "Entity/cubeprimitve.h"
#include "geometry/ray.h"
#include "GUI/sprite.h"
#include "Entity/water.h"
FlightGameDelegate::FlightGameDelegate()
{

}

void FlightGameDelegate::onInit()
{
    scene = new Scene();
    this->move_forward=false;
    this->move_backward=false;
    this->move_left=false;
    this->move_right=false;
    this->move_up = false;
    this->move_down = false;

    AmbientLight * ambient = scene->getAmbientLight ();
    ambient->setColor (QVector3D(1,1,1));
    ambient->setIntensity (0.1);

    auto sprite = new Sprite();
    sprite->setTexture (TexturePool::getInstance ()->createOrGetTexture ("./res/texture/mygame/fps/cross_hair.png"));
    sprite->setCamera (scene->guiCamera ());
    sprite->setPos (QVector3D(1024/2-sprite->texture ()->width ()/2,768/2-sprite->texture ()->height ()/2,0));
    scene->root ()->addChild (sprite);

    scene->setCamera (&camera);

    SkyBox * sky_box = new SkyBox("./res/texture/sky_box/sp3right.jpg",
                                  "./res/texture/sky_box/sp3left.jpg",
                                  "./res/texture/sky_box/sp3top.jpg",
                                  "./res/texture/sky_box/sp3bot.jpg",
                                  "./res/texture/sky_box/sp3front.jpg",
                                  "./res/texture/sky_box/sp3back.jpg");
    sky_box->setCamera(&camera);
    scene->setSkyBox(sky_box);

    //set this scene as current scene
    scene->setAsCurrentScene();
    scene->root ()->addChild (&camera);

    Terrain a("./res/model/terrain/terrain.jpg");
    auto terrain_model = new Entity();
    terrain_model->setName ("terrain");
    terrain_model->setCamera(&camera);
    terrain_model->setShaderProgram (ShaderPool::getInstance()->get ("deferred"));
    a.mesh ()->getMaterial ()->getDiffuse ()->texture= TexturePool::getInstance ()->createOrGetTexture ("./res/model/terrain/sand.jpg");
    terrain_model->addMesh(a.mesh ());
    terrain_model->scale (10,10,10);
    terrain_model->setPos (QVector3D(0,-3,0));
   // scene->root ()->addChild (terrain_model);

    auto water = new Water(30,30,-3,1);
    water->setCamera(&camera);
    water->setShaderProgram (ShaderPool::getInstance()->get ("deferred"));
    water->mesh ()->getMaterial ()->getDiffuse ()->texture= TexturePool::getInstance ()->createOrGetTexture ("./res/model/terrain/sand.jpg");
    water->setIsEnableShadow (false);
    scene->root ()->addChild (water);

    for(int i = 0;i< 5;i++)
    {
        auto flight = new Entity("./res/model/spaceship/phoenix_ugv.md2");
        flight->setCamera (&camera);
        flight->setShaderProgram (ShaderPool::getInstance()->get ("deferred"));
        flight->setScalling (QVector3D(0.05,0.05,0.05));
        scene->root ()->addChild (flight);
        //flight->setIsEnableShadow (false);
        flight->setPos (QVector3D((rand()%5)*2,-1,-10-2*i));
    }

/*
    auto spotLight = scene->createSpotLight ();
    spotLight->setIntensity (1);
    spotLight->setColor (QVector3D(0,1,1));
    spotLight->setDirection (QVector3D(-1,-1,0));
    spotLight->setPos (QVector3D(10,10,-4));
    spotLight->setOutterAngle (35);
    spotLight->setAngle (20);
    spotLight->setRange (40);
*/

    //then add  a Directional light
    auto directional_light = scene->getDirectionalLight ();
    directional_light->setIntensity (1);
    directional_light->setColor (QVector3D(1,1,1));
    directional_light->setDirection (QVector3D(-1,-1,0));
}

void FlightGameDelegate::onRender()
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
void FlightGameDelegate::onResize(int w, int h)
{

    // Calculate aspect ratio
    float aspect = float(w) / float(h ? h : 1);

    const float zNear = 0.01, zFar = 100.0, fov = 45.0;

    // Reset projection
    camera.setPerspective(fov,aspect,zNear,zFar);
}

void FlightGameDelegate::onKeyPress(int key_code)
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

void FlightGameDelegate::onKeyRelease(int key_code)
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

void FlightGameDelegate::onTouchBegin(int x, int y)
{
    mousePressPosition = QVector2D(x,y);

    mouseLastPosition = QVector2D(x,y);
}

void FlightGameDelegate::onTouchMove(int x, int y)
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

void FlightGameDelegate::onTouchEnd(int x, int y)
{

}

void FlightGameDelegate::createTerrain()
{

}
