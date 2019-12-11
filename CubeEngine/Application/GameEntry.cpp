#include "Application/GameEntry.h"
#include "CubeGame/GameWorld.h"
#include "Application/CubeGame/GameScriptBinding.h"
#include "Action/TintTo.h"
#include "Action/ActionSequence.h"
#include "Action/ActionCalFunc.h"

namespace tzw {
	class ActionSequence;
}

using namespace tzw;
using namespace std;

GameEntry::GameEntry()
{
	m_ticks = 0;
}

void GameEntry::onStart()
{ 

	g_binding_game_objects();
	GameWorld::shared();
	GameWorld::shared()->init();
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
	actionList.push_back(new ActionInterval(1.5));
	actionList.push_back(new TintTo(1.5, vec4(1, 1, 1, 0),vec4(1, 1, 1, 1)));
	actionList.push_back(new ActionInterval(2.5));
	actionList.push_back(new TintTo(1.5, vec4(1, 1, 1, 1),vec4(1, 1, 1, 0)));
	actionList.push_back(new ActionInterval(0.1));
	actionList.push_back(new ActionCallFunc(lambda));
	splashSprite->runAction(new ActionSequence(actionList));
}

void GameEntry::onExit()
{

}

void GameEntry::onUpdate(float delta)
{
	
}