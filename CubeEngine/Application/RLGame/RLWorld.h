#pragma once
#include "2D/TileMap2DMgr.h"
#include "Engine/EngineDef.h"

#define ARENA_MAP_SIZE 96
#include "RLPlayerController.h"
#include "2D/QuadTree2D.h"
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
		QuadTree2D * getQuadTree();
	protected:
		TileMap2DMgr * m_tileMgr;
		Node * m_mapRootNode = nullptr;
		RLPlayerController * m_playerController = nullptr;
		std::vector<RLHero * > m_heroes;
		QuadTree2D * m_quadTree = nullptr;
	};
}

