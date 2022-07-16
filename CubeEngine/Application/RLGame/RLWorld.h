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

enum class RL_GameState
{
	MainMenu,
	Playing,
	AfterMath,
};

namespace tzw
{
	class RLHero;
	class Camera;
	class RLWorld : public Singleton<RLWorld>,  public EventListener
	{
	public:


		void start();
		void startGame();
		void goToMainMenu();
		void goToAfterMath();
		Node * getRootNode();
		void onFrameUpdate(float dt) override;

		RLHero* spawnHero(int heroType);
		QuadTree2D * getQuadTree();
		vec2 getMapSize();
		RLPlayerController * getPlayerController();
		void getRandomBoundaryPos(int count, std::vector<vec2>& posList);
		RL_GameState getCurrGameState() const {return m_currGameState;}
		void setCurrGameState(RL_GameState currGameState) { m_currGameState = currGameState;}
	protected:
		TileMap2DMgr * m_tileMgr;
		Node * m_mapRootNode = nullptr;
		RLPlayerController * m_playerController = nullptr;
		std::vector<RLHero * > m_heroes;
		QuadTree2D * m_quadTree = nullptr;
		RL_GameState m_currGameState = RL_GameState::MainMenu;

	};
}

