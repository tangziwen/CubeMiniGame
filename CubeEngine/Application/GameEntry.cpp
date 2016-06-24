#include "Application/GameEntry.h"

#include "EngineSrc/Font/Font.h"

#include "EngineSrc/CubeEngine.h"

using namespace tzw;
GameEntry::GameEntry()
{
}

void GameEntry::onStart()
{

    auto theModel = Model::create("./test.tzw");
    theModel->setPos(vec3(0,0,-50));
    theModel->setScale(0.1,0.1,0.1);
    theModel->runAction(new RotateBy(5.0,vec3(0,360,0)));
    SceneMgr::shared()->currentScene()->addNode(theModel);
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
    frame2->setPos2D(500,100);
}

void GameEntry::onExit()
{


}

void GameEntry::onUpdate(float delta)
{

}

