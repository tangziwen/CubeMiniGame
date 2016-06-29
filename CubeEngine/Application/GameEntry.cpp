#include "Application/GameEntry.h"

#include "EngineSrc/Font/Font.h"

#include "EngineSrc/CubeEngine.h"

using namespace tzw;
GameEntry::GameEntry()
{
}

void GameEntry::onStart()
{
    auto theModel = Model::create("./Res/TestRes/duck.tzw");
    theModel->setPos(vec3(0,0,-50));
    theModel->setScale(0.1,0.1,0.1);
    theModel->runAction(new RepeatForever(new RotateBy(5.0,vec3(0,360,0))));
    SceneMgr::shared()->currentScene()->addNode(theModel);


    auto frame = GUIWindow::create("划水",vec2(450,600));
    GUIWindowMgr::shared()->add(frame);
    Button * startBtn = Button::create("开始游戏");
    startBtn->setPos2D(100,340);
    frame->addChild(startBtn);
}

void GameEntry::onExit()
{


}

void GameEntry::onUpdate(float delta)
{

}

