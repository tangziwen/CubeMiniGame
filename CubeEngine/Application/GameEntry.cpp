#include "Application/GameEntry.h"
#include "CubeGame/GameWorld.h"
#include "Application/CubeGame/GameScriptBinding.h"

using namespace tzw;
using namespace std;

GameEntry::GameEntry()
{
	m_ticks = 0;
}

void GameEntry::onStart()
{ 

#if CURRGAME == GAME_MODE_PLAYGROUND
	GameWorld::shared();
#endif
	g_binding_game_objects();
}

void GameEntry::onExit()
{

}

void GameEntry::onUpdate(float delta)
{
	
}