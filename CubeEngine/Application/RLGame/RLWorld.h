#pragma once
#include "2D/TileMap2DMgr.h"
#include "Engine/EngineDef.h"

#define ARENA_MAP_SIZE 96
#include "RLPlayerController.h"
namespace tzw
{
	class RLHero;
	class RLWorld : public Singleton<RLWorld>,  public EventListener
	{

	public:
		void start();
		Node * getRootNode();
		void onFrameUpdate(float dt) override;

		RLHero* spawnHero(int heroType);
	protected:
		TileMap2DMgr * m_tileMgr;
		Node * m_mapRootNode = nullptr;
		RLPlayerController * m_playerController = nullptr;
		std::vector<RLHero * > m_heroes;
	};
}

