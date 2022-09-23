#include "RLWorld.h"
#include "RLHero.h"
#include "Event/EventMgr.h"
#include "RLSpritePool.h"
#include "RLBulletPool.h"
#include "RLDirector.h"
#include "RLGUI.h"
#include "RLPlayerState.h"
#include "RLHeroCollection.h"
#include "RLWeaponCollection.h"
#include "RLCollectible.h"
#include "RLEffectMgr.h"
#include "RLAIController.h"
#include "Engine/Engine.h"
#include "box2d.h"
namespace tzw
{
void RLWorld::start()
{
	EventMgr::shared()->addFixedPiorityListener(this);
	RLWeaponCollection::shared()->loadConfig();
	RLHeroCollection::shared()->loadConfig();
	RLEffectMgr::shared()->loadConfig();
	m_tileMgr = new TileMap2DMgr();
	m_tileMgr->initMap(ARENA_MAP_SIZE, ARENA_MAP_SIZE);


	std::vector<int> grassTileList;
	grassTileList.push_back (m_tileMgr->addTileType("RL/Sprites/Grass_0.png"));
	grassTileList.push_back (m_tileMgr->addTileType("RL/Sprites/Grass_1.png"));
	grassTileList.push_back (m_tileMgr->addTileType("RL/Sprites/Grass_2.png"));
	grassTileList.push_back (m_tileMgr->addTileType("RL/Sprites/Grass_3.png"));
	grassTileList.push_back (m_tileMgr->addTileType("RL/Sprites/Grass_4.png"));
	grassTileList.push_back (m_tileMgr->addTileType("RL/Sprites/Grass_5.png"));

	 std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> d({600, 5, 5, 5, 5, 5});
	for(int i = 0; i < ARENA_MAP_SIZE; i++)
	{
		for(int j = 0; j < ARENA_MAP_SIZE; j++)
		{
			int idx = d(gen);
			m_tileMgr->addTile(grassTileList[idx], i, j);
		}
	}
	m_quadTree = new QuadTree2D();
	m_quadTree->init(vec2(-AREAN_COLLISION_MAP_PADDING, -AREAN_COLLISION_MAP_PADDING), vec2(ARENA_MAP_SIZE * 32 + AREAN_COLLISION_MAP_PADDING, ARENA_MAP_SIZE * 32 + AREAN_COLLISION_MAP_PADDING));
	
	m_mapRootNode = Node::create();
	m_scale = 1.5f;
	m_mapRootNode->setScale(vec3(m_scale, m_scale, m_scale));
	g_GetCurrScene()->addNode(m_mapRootNode);
	m_mapRootNode->addChild(m_tileMgr);

	RLSpritePool::shared()->init(m_mapRootNode);
	RLBulletPool::shared()->initSpriteList(m_mapRootNode);
	



	//init GUI

	RLGUI::shared()->init();

}

void RLWorld::startGame(std::string heroStr)
{
	m_b2dWorld = new b2World(b2Vec2(0,-0.0));

	//Ground
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(ARENA_MAP_SIZE * 0.5f, -10.0f);
	m_groundBody = m_b2dWorld->CreateBody(&groundBodyDef);
	b2PolygonShape groundBox;
	groundBox.SetAsBox(ARENA_MAP_SIZE, 10.0f);

	b2FixtureDef fixtureGroundDef;
	fixtureGroundDef.shape = &groundBox;
	fixtureGroundDef.density = 0.0f;
	fixtureGroundDef.filter.categoryBits = RL_OBSTACLE;
	fixtureGroundDef.filter.maskBits = 0xFFFF;
	m_groundBody->CreateFixture(&fixtureGroundDef);

	//left
	b2BodyDef groundBodyDef2;
	groundBodyDef2.position.Set(-10, ARENA_MAP_SIZE * .5f);
	b2Body* groundBodyLeft = m_b2dWorld->CreateBody(&groundBodyDef2);
	b2PolygonShape groundBoxLeft;
	groundBoxLeft.SetAsBox(10, ARENA_MAP_SIZE);

	b2FixtureDef fixtureGroundLeftDef;
	fixtureGroundLeftDef.shape = &groundBoxLeft;
	fixtureGroundLeftDef.density = 0.0f;
	fixtureGroundLeftDef.filter.categoryBits = RL_OBSTACLE;
	fixtureGroundLeftDef.filter.maskBits = 0xFFFF;
	groundBodyLeft->CreateFixture(&fixtureGroundLeftDef);


	//right
	b2BodyDef groundBodyDefRight;
	groundBodyDefRight.position.Set(ARENA_MAP_SIZE + 10, ARENA_MAP_SIZE * 0.5f);
	b2Body* groundBodyRight = m_b2dWorld->CreateBody(&groundBodyDefRight);
	b2PolygonShape groundBoxRight;
	groundBoxRight.SetAsBox(10.f, ARENA_MAP_SIZE);
	b2FixtureDef fixtureGroundRightDef;
	fixtureGroundRightDef.shape = &groundBoxRight;
	fixtureGroundRightDef.density = 0.0f;
	fixtureGroundRightDef.filter.categoryBits = RL_OBSTACLE;
	fixtureGroundRightDef.filter.maskBits = 0xFFFF;
	groundBodyRight->CreateFixture(&fixtureGroundRightDef);

	//Top
	b2BodyDef groundBodyDefTop;
	groundBodyDefTop.position.Set(ARENA_MAP_SIZE * 0.5f, ARENA_MAP_SIZE + 10.0f);
	b2Body* groundBodyTop = m_b2dWorld->CreateBody(&groundBodyDefTop);
	b2PolygonShape groundBoxTop;
	groundBoxTop.SetAsBox(ARENA_MAP_SIZE, 10.0f);
	b2FixtureDef fixtureGroundTopDef;
	fixtureGroundTopDef.shape = &groundBoxTop;
	fixtureGroundTopDef.density = 0.0f;
	fixtureGroundTopDef.filter.categoryBits = RL_OBSTACLE;
	fixtureGroundTopDef.filter.maskBits = 0xFFFF;
	groundBodyTop->CreateFixture(&fixtureGroundTopDef);



	RLHero * hero = spawnHero(heroStr);
	hero->setPosition(vec2(ARENA_MAP_SIZE * 32 * 0.5f, ARENA_MAP_SIZE * 32 * 0.5f));
	hero->getWeapon()->setIsAutoFiring(false);
	m_playerController = new RLPlayerController();
	m_playerController->possess(hero);
	hero->getWeapon()->setFullAmmo();
	setCurrGameState(RL_GameState::Playing);
	RLPlayerState::shared()->reset();
	RLDirector::shared()->generateWave();
	generateLevelUpPerk();
}

void RLWorld::goToMainMenu()
{

	setCurrGameState(RL_GameState::MainMenu);
}

void RLWorld::goToAfterMath()
{

	setCurrGameState(RL_GameState::AfterMath);
}

void RLWorld::goToWin()
{

	setCurrGameState(RL_GameState::Win);
}

void RLWorld::goToPurchase()
{
	setCurrGameState(RL_GameState::Purchase);
}

void RLWorld::goToPrepare()
{
	setCurrGameState(RL_GameState::Prepare);
}
void RLWorld::goToPause()
{
	setCurrGameState(RL_GameState::Pause);
}
void RLWorld::resumeGame()
{
	setCurrGameState(RL_GameState::Playing);
}
void RLWorld::endGame()
{
	RLPlayerState::shared()->writePersistent();
	exit(0);
}
void RLWorld::setCurrGameState(RL_GameState currGameState)
{
	if(currGameState == RL_GameState::Playing)
	{
		Engine::shared()->setUnlimitedCursor(true);
	}
	if(m_currGameState == RL_GameState::Playing && m_currGameState != currGameState)
	{
	
		Engine::shared()->setUnlimitedCursor(false);
	}
	m_currGameState = currGameState;
	RLPlayerState::shared()->writePersistent();

}
Node* RLWorld::getRootNode()
{
	return m_mapRootNode;
}

void RLWorld::onFrameUpdate(float dt)
{
	int32 velocityIterations = 6;
	int32 positionIterations = 2;
	if(m_currGameState ==  RL_GameState::Playing)
	{

		m_b2dWorld->Step(dt, velocityIterations, positionIterations);

		RLDirector::shared()->tick(dt);
		for(auto iter = m_heroes.begin();iter != m_heroes.end();)
		{
			RLHero * hero = *iter;
			hero->onTick(dt);
			if(!hero->isAlive())
			{
				iter = m_heroes.erase(iter);
				delete hero;
			}
			else
			{
				++iter;
			}
		}
		RLBulletPool::shared()->tick(dt);
		RLCollectibleMgr::shared()->tick(dt);
		m_quadTree->tick(dt);
	}

}

RLHero* RLWorld::spawnHero(int heroType)
{
	RLHero * hero = new RLHero(heroType);
	m_heroes.push_back(hero);
	return hero;
}

RLHero* RLWorld::spawnHero(std::string heroName)
{
	int id = RLHeroCollection::shared()->getHeroIDByName(heroName);

	return spawnHero(id);
}

RLHero* RLWorld::spawnEnemy(int heroType)
{
	RLHero * hero = spawnHero(heroType);
	
	RLAIController * controller =  CreateAIController(hero->getHeroData()->m_aiType);
	if(controller) controller->possess(hero);
	return hero;
}

RLHero* RLWorld::spawnEnemy(std::string heroName)
{
	int id = RLHeroCollection::shared()->getHeroIDByName(heroName);
	return spawnEnemy(id);
}

QuadTree2D* RLWorld::getQuadTree()
{
	return m_quadTree;
}

vec2 RLWorld::getMapSize()
{
	return vec2(ARENA_MAP_SIZE * 32, ARENA_MAP_SIZE * 32);
}

RLPlayerController* RLWorld::getPlayerController()
{
	return m_playerController;
}

void RLWorld::getRandomBoundaryPos(int count, std::vector<vec2>& posList)
{
	for(int i = 0; i < count; i++)
	{
		int rndSide = rand() %4;
		//bottom
		switch(rndSide)
		{
		case 0://bottom
			{
				int idx = rand() % ARENA_MAP_SIZE;
				posList.push_back(vec2(idx * 32, 0));
			}
		break;

		case 1://top
			{
				int idx = rand() % ARENA_MAP_SIZE;
				posList.push_back(vec2(idx * 32, (ARENA_MAP_SIZE - 1) * 32));
			}
		break;
		case 2://left
			{
				int idx = rand() % ARENA_MAP_SIZE;
				posList.push_back(vec2(0, idx * 32));
			}
		break;
		case 3://right
		{
			int idx = rand() % ARENA_MAP_SIZE;
			posList.push_back(vec2((ARENA_MAP_SIZE - 1) * 32, idx * 32));
		}
		break;
		default:
		break;
		}

	}
	
}

void RLWorld::generateLevelUpPerk()
{
	m_lvUpCollectible.clear();
	return;
	float offset = 128;
	vec2 center(ARENA_MAP_SIZE * 32 * 0.5f, ARENA_MAP_SIZE * 32 * 0.5f);
	m_lvUpCollectible.push_back( 
		RLCollectibleMgr::shared()->addCollectiblePerkEffect(RLEffectMgr::shared()->get("GreenPotion"), center +vec2(-offset, -offset)));
	m_lvUpCollectible.push_back( RLCollectibleMgr::shared()->addCollectiblePerkEffect(RLEffectMgr::shared()->get("MoveSpeedUp"), center +vec2(offset, -offset)));
	m_lvUpCollectible.push_back( RLCollectibleMgr::shared()->addCollectiblePerkEffect(RLEffectMgr::shared()->get("HeartRecover"), center +vec2(-offset, offset)));
	m_lvUpCollectible.push_back( RLCollectibleMgr::shared()->addCollectiblePerkEffect(RLEffectMgr::shared()->get("DamageUp"), center +vec2(offset, offset)));


}

void RLWorld::clearLevelUpPerk()
{
	for(auto collectible : m_lvUpCollectible)
	{
		collectible->setIsAlive(false);
	}
	m_lvUpCollectible.clear();
}

size_t RLWorld::getHeroesCount()
{
	return m_heroes.size();
}

vec2 RLWorld::getRandomPos()
{
	float offset = 16.0f;
	return vec2(TbaseMath::randRange(offset, ARENA_MAP_SIZE * 32.f - offset), TbaseMath::randRange(offset, ARENA_MAP_SIZE * 32.f - offset));
}



}
