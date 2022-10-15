#pragma once
#include "2D/TileMap2DMgr.h"
#include "Engine/EngineDef.h"
#include "RLDef.h"

#include "RLPlayerController.h"
#include "2D/QuadTree2D.h"
#include "b2_world.h"
#include "RLInteraction.h"


namespace tzw
{
	class LabelNew;
	class RLHero;
	class Camera;
	class RLCollectible;
	class RLInteraction;
	class RLContactListener : public b2ContactListener
	{
	public:
		void BeginContact(b2Contact* contact) override;
		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
	};
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
		void goToPause();
		void resumeGame();
		void endGame();
		void goToPerk();
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
		void setCurrGameState(RL_GameState currGameState);
		void generateLevelUpPerk();
		void clearLevelUpPerk();
		size_t getHeroesCount();
		vec2 getRandomPos();
		float getScale() {return m_scale;};
		float getInvScale() { return 1.f / m_scale;};
		b2World * getB2DWorld() { return m_b2dWorld;}
		b2Body * getGroundBody() {return m_groundBody;}
		bool onMouseMove(vec2 pos) override;
		void generateDoors();

		void onSubWaveFinished();
		void startNextSubWave();
		void clearAllInteractions();
		const std::vector<RLInteraction *> & getPossibleInteraction() {return m_possibleInteractions;}
		void showTempTips(std::string str);
	protected:
		std::vector<RLInteraction *> m_possibleInteractions;
		std::vector<RLInteraction *> m_interactions;
		TileMap2DMgr * m_tileMgr;
		Node * m_mapRootNode = nullptr;
		RLPlayerController * m_playerController = nullptr;
		std::vector<RLHero * > m_heroes;
		QuadTree2D * m_quadTree = nullptr;
		RL_GameState m_currGameState = RL_GameState::MainMenu;
		std::vector<RLCollectible *> m_lvUpCollectible;
		float m_scale = 1.0f;
		b2World * m_b2dWorld;
		b2Body* m_groundBody = nullptr;
		RLContactListener * m_contactListener;
		Sprite * m_crossHairsprite;
		LabelNew * m_centerTips = nullptr;
	};
}

