#pragma once
#include "2D/TileMap2DMgr.h"
#include "Engine/EngineDef.h"
#include "RLDef.h"

#include "RLPlayerController.h"
#include "2D/QuadTree2D.h"



namespace tzw
{
	class RLHero;
	class Camera;
	class RLCollectible;
	class RLWorld : public Singleton<RLWorld>,  public EventListener
	{
	public:


		void start();
		void startGame(std::string heroID);
		void goToMainMenu();
		void goToAfterMath();
		void goToWin();
		void goToPurchase();
		void goToPrepare();
		Node * getRootNode();
		void onFrameUpdate(float dt) override;

		RLHero* spawnHero(int heroType);
		RLHero* spawnHero(std::string heroName);
		RLHero* spawnEnemy(int heroType);
		RLHero* spawnEnemy(std::string heroName);
		QuadTree2D * getQuadTree();
		vec2 getMapSize();
		RLPlayerController * getPlayerController();
		void getRandomBoundaryPos(int count, std::vector<vec2>& posList);
		RL_GameState getCurrGameState() const {return m_currGameState;}
		void setCurrGameState(RL_GameState currGameState) { m_currGameState = currGameState;}
		void generateLevelUpPerk();
		void clearLevelUpPerk();
		size_t getHeroesCount();
		
	protected:
		TileMap2DMgr * m_tileMgr;
		Node * m_mapRootNode = nullptr;
		RLPlayerController * m_playerController = nullptr;
		std::vector<RLHero * > m_heroes;
		QuadTree2D * m_quadTree = nullptr;
		RL_GameState m_currGameState = RL_GameState::MainMenu;
		std::vector<RLCollectible *> m_lvUpCollectible;
	};
}

