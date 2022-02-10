#include "Application/GameEntry.h"
#include "CubeGame/GameWorld.h"
#include "Application/CubeGame/GameScriptBinding.h"
#include "Action/TintTo.h"
#include "Action/ActionSequence.h"
#include "Action/ActionCalFunc.h"
#include "PTMGame/PTMWorld.h"

namespace tzw {
	class ActionSequence;
}

using namespace tzw;
using namespace std;

GameEntry::GameEntry()
{
	m_ticks = 0;
}
static void showSplash()
{
	GameWorld::shared()->setCurrentState(GAME_STATE_SPLASH);
	//splash
	auto splashSprite = Sprite::create("logo_small.png");
    auto size = splashSprite->getContentSize();
	 
    splashSprite->setPos2D(Engine::shared()->windowWidth()/2 - size.x/2,Engine::shared()->windowHeight()/2 - size.y/2);
	splashSprite->setLocalPiority(999);
	splashSprite->setColor(vec4(1, 1, 1, 0));
    g_GetCurrScene()->addNode(splashSprite);

	auto lambda = [splashSprite]
	{
		GameWorld::shared()->setCurrentState(GAME_STATE_MAIN_MENU);
		splashSprite->removeFromParent();
	};
	std::vector<Action *> actionList;
	actionList.push_back(new ActionInterval(2.0));
	actionList.push_back(new TintTo(0.3, vec4(1, 1, 1, 0),vec4(1, 1, 1, 1)));
	actionList.push_back(new ActionInterval(1.5f));
	actionList.push_back(new TintTo(0.3, vec4(1, 1, 1, 1),vec4(1, 1, 1, 0)));
	actionList.push_back(new ActionInterval(0.1));
	actionList.push_back(new ActionCallFunc(lambda));
	splashSprite->runAction(new ActionSequence(actionList));
}

static void showSplash2()
{
	GameWorld::shared()->setCurrentState(GAME_STATE_SPLASH);
	//splash
	auto splashSprite = Sprite::create("logo_small.png");
    auto size = splashSprite->getContentSize();
	 
    splashSprite->setPos2D(Engine::shared()->windowWidth()/2 - size.x/2,Engine::shared()->windowHeight()/2 - size.y/2);
	splashSprite->setLocalPiority(999);
	splashSprite->setColor(vec4(1, 1, 1, 0));
    g_GetCurrScene()->addNode(splashSprite);


	auto splashSprite2 = Sprite::create("logo_small.png");
    auto size2 = splashSprite2->getContentSize();
	 
    splashSprite2->setPos2D(Engine::shared()->windowWidth()/2 - size2.x/2 + 150,Engine::shared()->windowHeight()/2 - size2.y/2);
	splashSprite2->setLocalPiority(999);
	splashSprite2->setColor(vec4(1, 1, 1, 0));
    g_GetCurrScene()->addNode(splashSprite2);
}

void GameEntry::onStart()
{ 
	g_binding_game_objects();
	GameWorld::shared();
	GameWorld::shared()->init();	
	showSplash();
}

void GameEntry::onExit()
{

}

void GameEntry::onUpdate(float delta)
{
	
}

TestVulkanEntry::TestVulkanEntry()
{
}

void TestVulkanEntry::onStart()
{
	g_binding_game_objects();
	GameWorld::shared();
	GameWorld::shared()->init();	
	showSplash();
}

void TestVulkanEntry::onExit()
{
}

void TestVulkanEntry::onUpdate(float delta)
{
}


PTMEntry::PTMEntry()
{
}

void PTMEntry::onStart()
{

	showSplash();
	//PTMWorld::shared()->initMap();
}

void PTMEntry::onExit()
{
}

void PTMEntry::onUpdate(float delta)
{
}