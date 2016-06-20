#include "Application/GameEntry.h"

#include "EngineSrc/Font/Font.h"

#include "EngineSrc/CubeEngine.h"
#include <QDebug>
#include "EngineSrc/Game/ConsolePanel.h"
#include "TUtility/TUtility.h"
#include "EngineSrc/Texture/TextureAtlas.h"
#include "EngineSrc/Font/FontMgr.h"
#include "EngineSrc/2D/GUIFrame.h"
#include "EngineSrc/2D/Button.h"
#include "EngineSrc/2D/GUITitledFrame.h"
#include "EngineSrc/Scene/SceneMgr.h"
#include "EngineSrc/3D/Primitive/Cube.h"


#include "QMatrix4x4"
#include <QDebug>
#include "EngineSrc/Engine/EngineDef.h"
#include "EngineSrc/2D/GUIWindowMgr.h"

using namespace tzw;
GameEntry::GameEntry()
{
}

void GameEntry::onStart()
{
    auto frame = GUIWindow::create("划水",vec2(450,600));
    GUIWindowMgr::shared()->add(frame);
    Button * startBtn = Button::create("开始游戏");
    startBtn->setPos2D(100,340);
    frame->addChild(startBtn);


    auto frame2 = GUIWindow::create("Cube Engine",vec2(450,600));
    GUIWindowMgr::shared()->add(frame2);
    Button * startBtn2 = Button::create("GUI组件");
    startBtn2->setPos2D(100,340);
    frame2->addChild(startBtn2);
    frame2->setPos2D(500,0);
//    frame2->setIsDragable(false);
}

void GameEntry::onExit()
{


}

void GameEntry::onUpdate(float delta)
{

}

