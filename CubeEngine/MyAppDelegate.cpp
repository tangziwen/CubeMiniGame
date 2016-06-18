#include "MyAppDelegate.h"

#include "EngineSrc/Font/Font.h"

#include "EngineSrc/CubeEngine.h"
#include "CubeGame/Block.h"
#include "CubeGame/Chunk.h"
#include <QDebug>
#include "CubeGame/GameWorld.h"
#include "CubeGame/GameMap.h"
#include "EngineSrc/2D/Glyph.h"
#include "EngineSrc/2D/Label.h"
#include "EngineSrc/Game/ConsolePanel.h"
#include "CubeGame/Player.h"
#include "TUtility/TUtility.h"
#include "EngineSrc/Texture/TextureAtlas.h"
using namespace tzw;
MyAppDelegate::MyAppDelegate()
{

}

Sprite * crossHair = nullptr;
#define WIDTH 100
#define HEIGHT 100
static bool testGUI = false;
void MyAppDelegate::onStart()
{
    if (testGUI)
    {
        Renderer::shared()->setEnable3DRender(false);

    }
    GameWorld::shared()->loadBlockSheet();
    crossHair = Sprite::create("./Res/User/CubeGame/texture/GUI/cross_hair.png");
    auto size = crossHair->contentSize();
    crossHair->setPos2D(Engine::shared()->windowWidth()/2 - size.x/2,Engine::shared()->windowHeight()/2 - size.y/2);
    SceneMgr::shared()->currentScene()->addNode(crossHair);
    if (!testGUI)
    {
        GameMap::shared()->setMapType(GameMap::MapType::Plain);
        GameMap::shared()->setMaxHeight(4);
        auto player = new Player();
        GameWorld::shared()->createWorld(SceneMgr::shared()->currentScene(),4,4,0.1);
        GameWorld::shared()->setPlayer(player);
        Cube* skybox = Cube::create("./Res/User/CubeGame/texture/SkyBox/left.jpg","./Res/User/CubeGame/texture/SkyBox/right.jpg",
                     "./Res/User/CubeGame/texture/SkyBox/top.jpg","./Res/User/CubeGame/texture/SkyBox/bottom.jpg",
                     "./Res/User/CubeGame/texture/SkyBox/back.jpg","./Res/User/CubeGame/texture/SkyBox/front.jpg");
        skybox->setIsAccpectOCTtree(false);
        skybox->setScale(80,80,80);
        SceneMgr::shared()->currentScene()->addNode(skybox);
    }
}

void MyAppDelegate::onExit()
{

}

void MyAppDelegate::onUpdate(float delta)
{
}

