#pragma once
#include "2D/TileMap2DMgr.h"
#include "Engine/EngineDef.h"

#define ARENA_MAP_SIZE 32
#define AREAN_COLLISION_MAP_PADDING 32
#include "RLPlayerController.h"
#include "2D/QuadTree2D.h"

enum RL_OBJECT_TYPE
{
	RL_OBJECT_TYPE_HERO,
	RL_OBJECT_TYPE_MONSTER
};

namespace tzw
{
	class RLHero;
	class Camera;
	class RLWorld : public Singleton<RLWorld>,  public EventListener
	{

	public:
		void start();
		Node * getRootNode();
		void onFrameUpdate(float dt) override;

		RLHero* spawnHero(int heroType);
		QuadTree2D * getQuadTree();
		vec2 getMapSize();
	protected:
		TileMap2DMgr * m_tileMgr;
		Node * m_mapRootNode = nullptr;
		RLPlayerController * m_playerController = nullptr;
		std::vector<RLHero * > m_heroes;
		QuadTree2D * m_quadTree = nullptr;

	};
}

