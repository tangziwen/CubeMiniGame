#include "Application/GameEntry.h"

#include "EngineSrc/Font/Font.h"

#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/2D/TableView.h"
#include "CubeGame/GameWorld.h"
#include "EngineSrc/Action/RepeatForever.h"
#include "EngineSrc/Action/RotateBy.h"
using namespace tzw;
GameEntry::GameEntry()
{
    m_ticks = 0;
}
void GameEntry::onStart()
{
    auto camera = SceneMgr::shared()->currentScene()->defaultCamera();
    auto model = Model::create("./Res/TestRes/phoenix_ugv.tzw");
    model->runAction(new RepeatForever(new RotateBy(30.0,vec3(0,360,0))));
    model->setScale(vec3(0.1,0.1,0.1));
    SceneMgr::shared()->currentScene()->addNode(model);

    camera->setPos(vec3(0,10,10));
    camera->lookAt(model->getPos(),vec3(0,1,0));
    //GameWorld::shared();
}

void GameEntry::onExit()
{

}

void GameEntry::onUpdate(float delta)
{
    m_ticks += delta;

//    if (m_ticks > 1.0) //每2秒过一天
//    {
//        tzwS::GameSystem::shared()->update();
//        m_ticks = 0;
//    }

}

