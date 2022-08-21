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
	g_GetCurrScene()->addNode(m_mapRootNode);
	m_mapRootNode->addChild(m_tileMgr);

	RLSpritePool::shared()->init(m_mapRootNode);
	RLBulletPool::shared()->initSpriteList(m_mapRootNode);
	m_playerController = new RLPlayerController();



	//init GUI

	RLGUI::shared()->init();

}

void RLWorld::startGame(std::string heroStr)
{
	RLHero * hero = spawnHero(heroStr);
	hero->setPosition(vec2(ARENA_MAP_SIZE * 32 * 0.5f, ARENA_MAP_SIZE * 32 * 0.5f));
	hero->equipWeapon(new RLWeapon("SMG"));
	hero->getWeapon()->setIsAutoFiring(true);
	m_playerController->possess(hero);
	setCurrGameState(RL_GameState::Playing);
	RLPlayerState::shared()->reset();
	RLDirector::shared()->generateWave();
	generateLevelUpPerk();
}

void RLWorld::goToMainMenu()
{
	m_currGameState = RL_GameState::MainMenu;
}

void RLWorld::goToAfterMath()
{
	m_currGameState = RL_GameState::AfterMath;
}

void RLWorld::goToWin()
{
	m_currGameState = RL_GameState::Win;
}

void RLWorld::goToPurchase()
{
	m_currGameState = RL_GameState::Purchase;
}

void RLWorld::goToPrepare()
{
	m_currGameState = RL_GameState::Prepare;
}

Node* RLWorld::getRootNode()
{
	return m_mapRootNode;
}

void RLWorld::onFrameUpdate(float dt)
{
	if(m_currGameState ==  RL_GameState::Playing)
	{
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
