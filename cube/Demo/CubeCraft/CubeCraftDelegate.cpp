#include "CubeCraftDelegate.h"
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
CubeCraftDelegate::CubeCraftDelegate()
{

}

void CubeCraftDelegate::onInit()
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

    CubePrimitve * bileishou = new CubePrimitve("./res/texture/mygame/bileishou.jpg");
    bileishou->setName ("bileishou");
    bileishou->setShaderProgram (ShaderPool::getInstance()->get ("deferred"));
    bileishou->setCamera(&camera);
    scene->root ()->addChild (bileishou);

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
}

void CubeCraftDelegate::onRender()
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
void CubeCraftDelegate::onResize(int w, int h)
{

    // Calculate aspect ratio
    float aspect = float(w) / float(h ? h : 1);

    const float zNear = 0.01, zFar = 100.0, fov = 45.0;

    // Reset projection
    camera.setPerspective(fov,aspect,zNear,zFar);
}

void CubeCraftDelegate::onKeyPress(int key_code)
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

void CubeCraftDelegate::onKeyRelease(int key_code)
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
    case Qt::Key_F:
        createBlock();
        break;
    case Qt::Key_R:
        removeBlock();
        break;
    case Qt::Key_Z:
        //replace delegate
        break;
    }
}

void CubeCraftDelegate::onTouchBegin(int x, int y)
{
    mousePressPosition = QVector2D(x,y);

    mouseLastPosition = QVector2D(x,y);
}

void CubeCraftDelegate::onTouchMove(int x, int y)
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

void CubeCraftDelegate::onTouchEnd(int x, int y)
{

}

void CubeCraftDelegate::createTerrain()
{
    Terrain a("./res/model/terrain/terrain.jpg");
    auto terrain_model = new Entity();
    terrain_model->setName ("terrain");
    terrain_model->setCamera(&camera);
    terrain_model->setShaderProgram (ShaderPool::getInstance()->get ("deferred"));
    a.mesh ()->getMaterial ()->getDiffuse ()->texture= TexturePool::getInstance ()->createOrGetTexture ("./res/model/terrain/sand.jpg");
    terrain_model->addMesh(a.mesh ());
    terrain_model->scale (10,10,10);
    terrain_model->setPos (QVector3D(0,-3,0));
    scene->root ()->addChild (terrain_model);
}

void CubeCraftDelegate::createBlock()
{
    QVector3D nearPoint(0,0,-1);
    QVector3D farPoint(0,0,1);
    //unproject it
    nearPoint = camera.ScreenToWorld (nearPoint);
    farPoint = camera.ScreenToWorld (farPoint);
    QVector3D direction = farPoint - nearPoint;
    Ray ray(nearPoint,direction);
    auto entityList = scene->getPotentialEntityList ();
    for(Entity * entity : entityList)
    {
        if(!entity->isGroupMask (NodeGroup::G_Default))
        {
            continue;
        }
        RayAABBSide SideResult;
        bool result = ray.intersect (entity->getAABB (),&SideResult);
        if(result)
        {
            switch (SideResult) {
            case RayAABBSide::up:
            {
                CubePrimitve * bileishou = new CubePrimitve("./res/texture/mygame/bileishou.jpg");
                bileishou->setShaderProgram (ShaderPool::getInstance()->get ("deferred"));
                bileishou->setCamera(&camera);
                bileishou->setPos (entity->pos ()+QVector3D(0,1,0));
                scene->root ()->addChild (bileishou);
            }
                break;
            case RayAABBSide::down:
            {
                CubePrimitve * bileishou = new CubePrimitve("./res/texture/mygame/bileishou.jpg");
                bileishou->setShaderProgram (ShaderPool::getInstance()->get ("deferred"));
                bileishou->setCamera(&camera);
                bileishou->setPos (entity->pos ()+QVector3D(0,-1,0));
                scene->root ()->addChild (bileishou);
            }
                break;
            case RayAABBSide::left:
            {
                CubePrimitve * bileishou = new CubePrimitve("./res/texture/mygame/bileishou.jpg");
                bileishou->setShaderProgram (ShaderPool::getInstance()->get ("deferred"));
                bileishou->setCamera(&camera);
                bileishou->setPos (entity->pos ()+QVector3D(-1,0,0));
                scene->root ()->addChild (bileishou);
            }
                break;
            case RayAABBSide::right:
            {
                CubePrimitve * bileishou = new CubePrimitve("./res/texture/mygame/bileishou.jpg");
                bileishou->setShaderProgram (ShaderPool::getInstance()->get ("deferred"));
                bileishou->setCamera(&camera);
                bileishou->setPos (entity->pos ()+QVector3D(1,0,0));
                scene->root ()->addChild (bileishou);
            }
                break;
            case RayAABBSide::back:
            {
                CubePrimitve * bileishou = new CubePrimitve("./res/texture/mygame/bileishou.jpg");
                bileishou->setName ("bileishou");
                bileishou->setShaderProgram (ShaderPool::getInstance()->get ("deferred"));
                bileishou->setCamera(&camera);
                bileishou->setPos (entity->pos ()+QVector3D(0,0,-1));
                scene->root ()->addChild (bileishou);
            }
                break;
            case RayAABBSide::front:
            {
                CubePrimitve * bileishou = new CubePrimitve("./res/texture/mygame/bileishou.jpg");
                bileishou->setName ("bileishou");
                bileishou->setShaderProgram (ShaderPool::getInstance()->get ("deferred"));
                bileishou->setCamera(&camera);
                bileishou->setPos (entity->pos ()+QVector3D(0,0,1));
                scene->root ()->addChild (bileishou);
            }
                break;
            default:
                break;
            }
            break;
        }
    }
}

void CubeCraftDelegate::removeBlock()
{
    QVector3D nearPoint(0,0,-1);
    QVector3D farPoint(0,0,1);
    //unproject it
    nearPoint = camera.ScreenToWorld (nearPoint);
    farPoint = camera.ScreenToWorld (farPoint);
    QVector3D direction = farPoint - nearPoint;
    Ray ray(nearPoint,direction);
    auto entityList = scene->getPotentialEntityList ();
    for(Entity * entity : entityList)
    {
        if(!entity->isGroupMask (NodeGroup::G_Default))
        {
            continue;
        }
        RayAABBSide SideResult;
        bool result = ray.intersect (entity->getAABB (),&SideResult);
        if(result)
        {
            entity->removeFromParent ();
            break;
        }
    }
}
