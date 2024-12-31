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
#include "RLPerk.h"
#include "RLShopMgr.h"
#include "2D/LabelNew.h"
#include "Action/TintTo.h"
#include "Action/ActionSequence.h"
#include "Action/ActionCalFunc.h"
#include "2D/GUIFrame.h"
#include "Font/FontMgr.h"
#include "RLSFX.h"
namespace tzw
{


void RLContactListener::BeginContact(b2Contact* contact)
{

}

void RLContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	
	RLUserDataWrapper * wrapperA = reinterpret_cast<RLUserDataWrapper*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	if(wrapperA && wrapperA->m_cb)
	{

		wrapperA->m_cb(contact->GetFixtureA()->GetBody(), contact->GetFixtureB()->GetBody(), contact);
	}

	RLUserDataWrapper * wrapperB = reinterpret_cast<RLUserDataWrapper*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);
	if(wrapperB && wrapperB->m_cb)
	{

		wrapperB->m_cb(contact->GetFixtureB()->GetBody(), contact->GetFixtureA()->GetBody(), contact);
	}
}

void RLWorld::start()
{
	m_crossHairsprite = Sprite::create("RL/CrossHair.png");
	m_crossHairsprite->setLocalPiority(9999);

	m_centerTips = LabelNew::create("Press E To Use");
	m_centerTips->setLocalPiority(999);
	vec2 size = Engine::shared()->winSize();
	vec2 cs = m_centerTips->getContentSize();
	m_centerTips->setPos2D(size.x / 2.f - cs.x * 0.5f, size.y * .5f - 200);
	g_GetCurrScene()->addNode(m_centerTips);

	g_GetCurrScene()->addNode(m_crossHairsprite);
	//Engine::shared()->setUnlimitedCursor(true);
	EventMgr::shared()->addFixedPiorityListener(this);
	RLWeaponCollection::shared()->loadConfig();
	RLShopMgr::shared()->init();
	RLHeroCollection::shared()->loadConfig();
	RLEffectMgr::shared()->loadConfig();
	RLPerkMgr::shared()->loadPerkList();
	RLRewardItemMgr::shared()->loadConfig();
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
	m_scale = 1.0f;
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
	RLPerkMgr::shared()->startDeck();
	m_contactListener = new RLContactListener();
	m_b2dWorld->SetContactListener(m_contactListener);
	{
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
	}
	{
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
	}


	{
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
	}


	{
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
	}




	RLHero * hero = spawnHero(heroStr);
	hero->setPosition(vec2(ARENA_MAP_SIZE * 32 * 0.5f, 128.f));
	hero->getWeapon()->setIsAutoFiring(false);
	m_playerController = new RLPlayerController();
	m_playerController->possess(hero);
	hero->getWeapon()->setFullAmmo();
	setCurrGameState(RL_GameState::Playing);
	RLPlayerState::shared()->reset();
	RLDirector::shared()->generateWave();
	generateLevelUpPerk();
	showTempTips("Fight!!!");

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
void RLWorld::goToPerk()
{
	setCurrGameState(RL_GameState::PerkSlect);
}
void RLWorld::setCurrGameState(RL_GameState currGameState)
{

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
		bool isNeedShowTips = false;
		m_possibleInteractions.clear();
		for(auto iter = m_interactions.begin();iter != m_interactions.end();)
		{
			RLInteraction * interaction = *iter;
			interaction->tick(dt);
			if(interaction->getIsAlive())
			{
				if(interaction->isInteractiveable())
				{
					isNeedShowTips = true;
					m_possibleInteractions.push_back(interaction);
				}
				++iter;
			}
			else
			{
				delete interaction;
				iter = m_interactions.erase(iter);
			}
		}
		m_centerTips->setIsVisible(isNeedShowTips);
		m_b2dWorld->Step(dt, velocityIterations, positionIterations);

		RLDirector::shared()->tick(dt);
		RLSFXMgr::shared()->tick(dt);
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
	vec2 topCenter(ARENA_MAP_SIZE * 32 * 0.5f, ARENA_MAP_SIZE * 32);
	float heightOffset = 96;
	float slotSize = 64.f;
	int width = 4;
	int height = 3;
	vec2 basePos = vec2(topCenter.x - width * slotSize * 0.5, topCenter.y - heightOffset);
	for(int i = 0; i < width; i++)
	{
		for(int j = 0; j < height; j++)
		{
			posList.push_back(basePos + vec2(i * slotSize, -j * slotSize));
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

bool RLWorld::onMouseMove(vec2 pos)
{
	m_crossHairsprite->setPos2D(pos);
	return false;
}

void RLWorld::generateDoors()
{
	vec2 leftDoorPos = vec2(ARENA_MAP_SIZE * 32 * 0.5f - 200, ARENA_MAP_SIZE * 32 - 160);
	vec2 RightDoorPos = vec2(ARENA_MAP_SIZE * 32 * 0.5f + 200, ARENA_MAP_SIZE * 32 - 160);
	auto leftDoor = new RLStair(leftDoorPos);
	leftDoor->initGraphics();
	auto rightDoor = new RLStair(RightDoorPos);
	rightDoor->initGraphics();
	m_interactions.push_back(leftDoor);
	m_interactions.push_back(rightDoor);
	vec2 center(ARENA_MAP_SIZE * 32 * 0.5f, ARENA_MAP_SIZE * 32 * 0.5f);

	
	for(int i = 0; i < 3; i++)
	{
		auto reward = new RLReward(RLRewardItemMgr::shared()->getNextItem(),vec2(center.x + i * 64 - 100, center.y));
		reward->initGraphics();
		m_interactions.push_back(reward);
	}

}

void RLWorld::onSubWaveFinished()
{
	generateDoors();
	showTempTips("Clear!!!");
}

void RLWorld::startNextSubWave()
{


	auto splashSprite = GUIFrame::create(vec2(Engine::shared()->windowWidth(), Engine::shared()->windowHeight()));

	 
    //splashSprite->setPos2D(Engine::shared()->windowWidth()/2 - size.x/2,Engine::shared()->windowHeight()/2 - size.y/2);
	splashSprite->setLocalPiority(999);
	splashSprite->setColor(vec4(0, 0, 0, 0));
    g_GetCurrScene()->addNode(splashSprite);

	auto actionLambda = [splashSprite, this]
	{
		clearAllInteractions();
		m_playerController->getPossessHero()->setPosition(vec2(ARENA_MAP_SIZE * 32 * 0.5f, 128.f), true);
		RLDirector::shared()->startNextSubWave();
	};

	auto finishedLambda = [splashSprite, this]
	{
		splashSprite->removeFromParent();
		std::string tipsStr = "Floor";
		char tmp[256];
		sprintf(tmp, "Floor %d Fight!", RLDirector::shared()->getCurrentSubWave() + 1);
		tipsStr = tmp;
		showTempTips(tipsStr);
	};
	std::vector<Action *> actionList;
	actionList.push_back(new TintTo(0.2, vec4(0, 0, 0, 0),vec4(0, 0, 0, 1)));
	actionList.push_back(new ActionCallFunc(actionLambda));
	actionList.push_back(new ActionInterval(0.2f));
	actionList.push_back(new TintTo(0.2, vec4(0, 0, 0, 1),vec4(0, 0, 0, 0)));
	actionList.push_back(new ActionCallFunc(finishedLambda));
	splashSprite->runAction(new ActionSequence(actionList));

}

void RLWorld::clearAllInteractions()
{
	for(auto interaction : m_interactions)
	{
		delete interaction;
	}
	m_interactions.clear();
	m_possibleInteractions.clear();
}

void RLWorld::showTempTips(std::string str)
{
	auto splashSprite = LabelNew::create(str, FontMgr::shared()->getTitleFont());

	 
    //splashSprite->setPos2D(Engine::shared()->windowWidth()/2 - size.x/2,Engine::shared()->windowHeight()/2 - size.y/2);
	splashSprite->setLocalPiority(999);
	splashSprite->setColor(vec4(1, 1, 1, 0));
    g_GetCurrScene()->addNode(splashSprite);
	vec2 size = Engine::shared()->winSize();
	vec2 cs = m_centerTips->getContentSize();

	splashSprite->setPos2D(size.x / 2.f - cs.x * 0.5f, size.y/ 2.f + 200);
	//splashSprite->setScale(scale, scale, scale);
	auto finishedLambda = [splashSprite]
	{
		splashSprite->removeFromParent();
	};
	std::vector<Action *> actionList;
	actionList.push_back(new TintTo(0.2f, vec4(1, 1, 1, 0),vec4(1, 1, 1, 1)));
	actionList.push_back(new ActionInterval(0.5f));
	actionList.push_back(new TintTo(0.2f, vec4(1, 1, 1, 1),vec4(1, 1, 1, 0)));
	actionList.push_back(new ActionInterval(0.1));
	actionList.push_back(new ActionCallFunc(finishedLambda));
	splashSprite->runAction(new ActionSequence(actionList));
}






}
