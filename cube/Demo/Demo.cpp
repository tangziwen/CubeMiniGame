#include "Demo.h"
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
#include "Entity/waterNaive.h"
#include "Entity/waterprojectgrid.h"
#include "GUI/button.h"
#include "GUI/Label.h"
#include "Demo/NormalMapping/normalmappingdelegate.h"
Demo::Demo()
{

}

void Demo::onInit()
{
    scene = new Scene();

    auto sprite = new Sprite();
    sprite->setTexture (TexturePool::getInstance ()->createOrGetTexture ("./res/texture/mygame/fps/cross_hair.png"));
    sprite->setCamera (scene->guiCamera ());
    sprite->setPos (QVector3D(1024/2-sprite->texture ()->width ()/2,768/2-sprite->texture ()->height ()/2,0));
    scene->root ()->addChild (sprite);

    Button* button = new Button();
    button->init ("./res/texture/GUI/button.png","./res/texture/GUI/button_2.png");
    button->setCamera (scene->guiCamera ());
    button->setPos (QVector3D(100,100,0));
    scene->root ()->addChild (button);
    button->onTouch = [](){
        auto scene = NormalMappingDelegate::createScene ();
        scene->setAsCurrentScene ();
    };

    Label * label = new Label();
    label->setCamera (scene->guiCamera ());
    label->setPos (QVector3D(200,400,0));
    label->setText ("Cube Engine Demo");
    scene->root ()->addChild (label);

    //set this scene as current scene
    scene->setAsCurrentScene();

}

void Demo::onRender()
{

}

void Demo::onResize(int w, int h)
{

    // Calculate aspect ratio
    float aspect = float(w) / float(h ? h : 1);

    const float zNear = 0.01, zFar = 100.0, fov = 45.0;

    // Reset projection
}

void Demo::onKeyPress(int key_code)
{

}

void Demo::onKeyRelease(int key_code)
{

}

void Demo::onTouchBegin(int x, int y)
{
}

void Demo::onTouchMove(int x, int y)
{

}

void Demo::onTouchEnd(int x, int y)
{

}
