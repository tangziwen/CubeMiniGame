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
#include "Utility/FPSCamera.h"

Scene *NormalMappingDelegate::createScene()
{
    auto scene = new Scene();
    auto m = new NormalMappingDelegate();
    m->scene = scene;
    m->onInit ();
    scene->root ()->addChild (m);
    return scene;
}

NormalMappingDelegate::NormalMappingDelegate()
{

}


void NormalMappingDelegate::onInit()
{
    scene = new Scene();
    camera = new tzw::FPSCamera();
    scene->setRenderType (DEFERRED_SHADING);

    AmbientLight * ambient = scene->getAmbientLight ();
    ambient->setColor (QVector3D(1,1,1));
    ambient->setIntensity (0.5);

    auto sprite = new Sprite();
    sprite->setTexture (TexturePool::getInstance ()->createOrGetTexture ("./res/texture/mygame/fps/cross_hair.png"));
    sprite->setCamera (scene->guiCamera ());
    sprite->setPos (QVector3D(1024/2-sprite->texture ()->width ()/2,768/2-sprite->texture ()->height ()/2,0));
    scene->root ()->addChild (sprite);

    scene->setCamera (camera);

    scene->root ()->addChild (camera);

    //create a box
    auto the_head = new Entity("./res/model/scan_head/Infinite-Level_02.obj",Entity::LoadPolicy::LoadFromLoader);
    the_head->setShaderProgram (ShaderPool::getInstance ()->get ("deferred"));
    the_head->scale (30,30,30);
    //set it's normal map
    auto material = the_head->getMesh (0)->getMaterial ();
    material->setNormalMap (TexturePool::getInstance ()->createOrGetTexture ("./res/model/scan_head/Infinite-Level_02_Tangent_SmoothUV.jpg"));
    the_head->setCamera (camera);
    //this sample we don't use shadow
    the_head->setIsEnableShadow (false);

    //bob
    auto the_bob2 = new Entity("./res/model/bob/boblampclean.md5mesh",Entity::LoadPolicy::LoadFromLoader);
    the_bob2->setShaderProgram (ShaderPool::getInstance ()->get ("deferred"));
    the_bob2->setCamera (camera);
    the_bob2->setPos (QVector3D(-2,0,-10));
    the_bob2->setRotation (QVector3D(-90,0,0));
    scene->root ()->addChild (the_bob2);
    the_bob2->setIsEnableShadow (false);
    the_bob2->scale (0.05,0.05,0.05);
    the_bob2->getSkeleton ()->getEntityNodeRoot ()->findNode ("lamp")->addChild (the_head);
    //this sample we don't use shadow
    the_bob2->setIsEnableShadow (false);
    the_bob2->onRender = [](Entity * self, float dt){
        self->playAnimate (0,self->animateTime ()+ 0.02);
    };



    auto the_orc = new Entity("./res/model/orc/orc.FBX",Entity::LoadPolicy::LoadFromLoader);
    the_orc->setShaderProgram (ShaderPool::getInstance ()->get ("deferred"));
    the_orc->setCamera (camera);
    the_orc->setRotation (QVector3D(0,180,0));
    the_orc->setScalling (QVector3D(0.1,0.1,0.1));
    the_orc->setPos (QVector3D(0,0,-10));

    scene->root ()->addChild (the_orc);
    the_orc->onRender = [](Entity * self, float dt){
        self->playAnimate (0,self->animateTime () + 0.02);
    };

    auto the_orc_axe = new Entity("./res/model/orc/axe.FBX",Entity::LoadPolicy::LoadFromLoader);
    the_orc_axe->setShaderProgram (ShaderPool::getInstance ()->get ("deferred"));
    the_orc_axe->setCamera (camera);
    the_orc->getSkeleton ()->getEntityNodeRoot ()->findNode ("Bip001 R Hand")->addChild (the_orc_axe);

    //then add  a Directional light
    auto directional_light = scene->getDirectionalLight ();
    directional_light->setIntensity (1);
    directional_light->setColor (QVector3D(1,1,1));
    directional_light->setDirection (QVector3D(0,0,-1));

}

void NormalMappingDelegate::onRender()
{

}

void NormalMappingDelegate::onResize(int w, int h)
{

    // Calculate aspect ratio
    float aspect = float(w) / float(h ? h : 1);

    const float zNear = 0.01, zFar = 100.0, fov = 45.0;

    // Reset projection
    camera->setPerspective(fov,aspect,zNear,zFar);
}
