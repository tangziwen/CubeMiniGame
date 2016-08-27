#include "Application/GameEntry.h"

#include "EngineSrc/Font/Font.h"

#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/2D/TableView.h"
#include "CubeGame/GameWorld.h"
#include "EngineSrc/Action/RepeatForever.h"
#include "EngineSrc/Action/RotateBy.h"
#include "EngineSrc/3D/Primitive/CubePrimitive.h"
using namespace tzw;
GameEntry::GameEntry()
{
    m_ticks = 0;
}
void GameEntry::onStart()
{
    FPSCamera * camera = FPSCamera::create(SceneMgr::shared()->currentScene()->defaultCamera());
    SceneMgr::shared()->currentScene()->setDefaultCamera(camera);
    SceneMgr::shared()->currentScene()->addNode(camera);
    camera->setIsEnableGravity(false);
    for (int i = 0; i < 5; i++)
    {
        auto model = Model::create("./Res/TestRes/duck.tzw");
        model->runAction(new RepeatForever(new RotateBy(30.0,vec3(0,360,0))));
        model->setScale(vec3(0.03,0.03,0.03));
        model->setPos(vec3(-10 + i * 5,0,0));
        model->technique()->setVar("TU_roughness",0.2f + i * 0.2f);
        SceneMgr::shared()->currentScene()->addNode(model);
    }
    auto theCube = new CubePrimitive(20,20,2);
    SceneMgr::shared()->currentScene()->addNode(theCube);
    camera->setPos(vec3(0,15,-15));
    camera->lookAt(vec3(0,0,0),vec3(0,1,0));


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

