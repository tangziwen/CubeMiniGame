#include "mydelegate.h"
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
    scene->setRenderType (DEFERRED_SHADING);
    if(scene->renderType () == FORWARD_SHADING)
    {
        auto sky_box = new SkyBox("./res/texture/sky_box/sp3right.jpg",
                                  "./res/texture/sky_box/sp3left.jpg",
                                  "./res/texture/sky_box/sp3top.jpg",
                                  "./res/texture/sky_box/sp3bot.jpg",
                                  "./res/texture/sky_box/sp3front.jpg",
                                  "./res/texture/sky_box/sp3back.jpg");
        sky_box->setCamera(&camera);
        scene->setSkyBox(sky_box);
        Terrain a("./res/model/terrain/terrain.jpg");
        auto terrain_model = new Entity();
        terrain_model->setCamera(&camera);
        a.mesh ()->getMaterial ()->getDiffuse ()->texture= TexturePool::getInstance ()->createOrGetTexture ("./res/model/terrain/sand.jpg");
        terrain_model->addMesh(a.mesh ());
        terrain_model->scale (10,10,10);
        terrain_model->setPos (QVector3D(0,-3,0));
        scene->root ()->addChild (terrain_model);

        auto box_cube = new Entity("res/model/box/box.obj");
        box_cube->setCamera(&camera);
        box_cube->translate(0,0,-5);
        box_cube->scale (5,5,1);
        box_cube->rotate(0,0,0);
        box_cube->setName ("box_cube");
        scene->root ()->addChild (box_cube);

        entity =new Entity("res/model/bob/boblampclean.md5mesh");
        entity->setCamera(&camera);
        entity->scale(0.05,0.05,0.05);
        entity->translate(0,-2,-10);
        entity->rotate(-90,0,0);
        entity->setName ("entity");
        scene->root ()->addChild (entity);
        auto  weapon = new Entity("res/model/m16/M16.dae");
        weapon->setGroupMask (NodeGroup::G_1);
        weapon->setCamera(&camera);
        weapon->rotate (-90,180,0);
        weapon->translate (0.6,-0.55,-0.8);
        camera.addChild (weapon);

        // a spotlight
        auto * light = scene->createSpotLight();
        light->setPos(QVector3D(0,0,-25));
        light->setColor(QVector3D(1,0,0));
        light->setDirection(QVector3D(0,0,1));
        light->setRange(100);
        light->setAngle(utility::Ang2Radius(10));
        light->setOutterAngle(utility::Ang2Radius(12));

    }else
    {

        AmbientLight * ambient = scene->getAmbientLight ();
        ambient->setColor (QVector3D(1,1,1));
        ambient->setIntensity (0.5);

        Entity * weapon = new Entity("res/model/m16/M16.dae");
        weapon->setCamera(&camera);
        weapon->setName ("weapon");
        weapon->setGroupMask (NodeGroup::G_1);
        weapon->rotate (-90,180,0);
        weapon->translate (0.6,-0.55,-0.8);
        weapon->setShaderProgram (ShaderPool::getInstance()->get ("deferred"));
        camera.addChild (weapon);

        CubePrimitve * bileishou = new CubePrimitve("./res/texture/mygame/bileishou.jpg");
        bileishou->setName ("bileishou");
        bileishou->setShaderProgram (ShaderPool::getInstance()->get ("deferred"));
        bileishou->setCamera(&camera);
        scene->root ()->addChild (bileishou);

        CubePrimitve * xiaoxiao = new CubePrimitve("./res/texture/mygame/wool_colored_white.png");
        xiaoxiao->setName ("xiaoxiao");
        xiaoxiao->setShaderProgram (ShaderPool::getInstance()->get ("deferred"));
        xiaoxiao->setCamera(&camera);
        xiaoxiao->setPos (QVector3D(1,0,10));
        scene->root ()->addChild (xiaoxiao);
        // a spotlight
        auto light = scene->createSpotLight();
        light->setPos(QVector3D(0,0,-25));
        light->setColor(QVector3D(1,1,1));
        light->setDirection(QVector3D(0,0,1));
        light->setRange(300);
        light->setAngle(utility::Ang2Radius(10));
        light->setOutterAngle(utility::Ang2Radius(12));



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
    }
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
    //entity->setAnimateTime(entity->animateTime()+0.01);
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
    case Qt::Key_F:
    {

        QVector4D nearPoint(0,0,-1,1);
        QVector4D farPoint(0,0,1,1);
        //unproject it
        QMatrix4x4 mat = camera.getProjection () * camera.getViewMatrix ();
        mat = mat.inverted ();
        nearPoint = mat* nearPoint;
        QVector3D nearPoint3 = nearPoint.toVector3D ()/nearPoint.w ();
        farPoint = mat*farPoint;
        QVector3D farPoint3 = farPoint.toVector3D ()/farPoint.w ();
        QVector3D direction = farPoint3 - nearPoint3;
        Ray ray(nearPoint3,direction);
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
                qDebug()<<"hit "<<entity->name ();
                switch (SideResult) {
                case RayAABBSide::up:
                {
                    qDebug()<<"up";
                    CubePrimitve * bileishou = new CubePrimitve("./res/texture/mygame/bileishou.jpg");
                    bileishou->setShaderProgram (ShaderPool::getInstance()->get ("deferred"));
                    bileishou->setCamera(&camera);
                    bileishou->setPos (entity->pos ()+QVector3D(0,1,0));
                    scene->root ()->addChild (bileishou);
                }
                    break;
                case RayAABBSide::down:
                {
                    qDebug()<<"down";
                    CubePrimitve * bileishou = new CubePrimitve("./res/texture/mygame/bileishou.jpg");
                    bileishou->setShaderProgram (ShaderPool::getInstance()->get ("deferred"));
                    bileishou->setCamera(&camera);
                    bileishou->setPos (entity->pos ()+QVector3D(0,-1,0));
                    scene->root ()->addChild (bileishou);
                }
                    break;
                case RayAABBSide::left:
                {
                    qDebug()<<"left";
                    CubePrimitve * bileishou = new CubePrimitve("./res/texture/mygame/bileishou.jpg");
                    bileishou->setShaderProgram (ShaderPool::getInstance()->get ("deferred"));
                    bileishou->setCamera(&camera);
                    bileishou->setPos (entity->pos ()+QVector3D(-1,0,0));
                    scene->root ()->addChild (bileishou);
                }
                    break;
                case RayAABBSide::right:
                {
                    qDebug()<<"right";
                    CubePrimitve * bileishou = new CubePrimitve("./res/texture/mygame/bileishou.jpg");
                    bileishou->setShaderProgram (ShaderPool::getInstance()->get ("deferred"));
                    bileishou->setCamera(&camera);
                    bileishou->setPos (entity->pos ()+QVector3D(1,0,0));
                    scene->root ()->addChild (bileishou);
                }
                    break;
                case RayAABBSide::back:
                {
                    qDebug()<<"back";
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
                    qDebug()<<"front";
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

void myDelegate::createTerrain()
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
